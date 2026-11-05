// Podsłuch portu szeregowego przez LD_PRELOAD.
// Zapisuje każdy read/write na deskryptorach prowadzących do ścieżek
// zawierających TTYSNIFF_MATCH (domyślnie "ttyUSB") do pliku TTYSNIFF_LOG.
// Deskryptor sprawdzamy przy pierwszym użyciu przez /proc/self/fd,
// bo Wine dostaje go od wineservera przez gniazdo, a nie z open().
//
// Kompilacja: gcc -shared -fPIC -O2 -o ttysniff.so ttysniff.c -ldl
// Użycie:     TTYSNIFF_LOG=log.txt LD_PRELOAD=./ttysniff.so program …
//
// Format linii: czas_s kierunek(>> zapis, << odczyt) bajty_hex
//
// TTYSNIFF_MATCH zaczynające się od '/' to dokładna ścieżka (np. /dev/pts/3),
// inaczej wystarczy, że ścieżka ją zawiera.
//
// Pseudoterminal zamiast portu (udawana centrala, tools/udawana_centrala.py):
// pty nie obsługuje ioctl-i linii modemu (TIOCMGET itp.),
// a bez nich Wine nie otwiera COM-a (GetCommState).
// Dla podsłuchiwanych deskryptorów udajemy, że te ioctl-e działają.
//
// TTYSNIFF_BLOCK: przedrostki ścieżek oddzielone dwukropkiem
// (np. /dev/ttyUSB:/dev/ttyACM), których program nie otworzy
// odmianami open() ani fopen(), także przez dowiązanie
// (bezpośredniego wywołania systemowego shim nie widzi).
// Chroni prawdziwą centralę, gdy Manager rozmawia z udawaną.
#define _GNU_SOURCE
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <linux/serial.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define MAX_FD 4096

static char watched[MAX_FD];
static int log_fd = -1;
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

static ssize_t (*real_read)(int, void *, size_t);
static ssize_t (*real_write)(int, const void *, size_t);
static int (*real_openat)(int, const char *, int, ...);
static int (*real_close)(int);
static FILE *(*real_fopen)(const char *, const char *);
static int (*real_ioctl)(int, unsigned long, ...);

static void init(void)
{
    if (real_read)
        return;
    real_read = dlsym(RTLD_NEXT, "read");
    real_write = dlsym(RTLD_NEXT, "write");
    real_openat = dlsym(RTLD_NEXT, "openat");
    real_close = dlsym(RTLD_NEXT, "close");
    real_fopen = dlsym(RTLD_NEXT, "fopen");
    real_ioctl = dlsym(RTLD_NEXT, "ioctl");
    const char *path = getenv("TTYSNIFF_LOG");
    if (path)
        log_fd = real_openat(AT_FDCWD, path, O_WRONLY | O_CREAT | O_APPEND,
                             0600);
}

static void log_data(const char *dir, const void *buf, ssize_t n)
{
    if (log_fd < 0 || n <= 0)
        return;
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    char line[64 + 3 * 4096];
    int len = snprintf(line, 64, "%ld.%06ld %s", (long)ts.tv_sec,
                       ts.tv_nsec / 1000, dir);
    const unsigned char *p = buf;
    for (ssize_t i = 0; i < n && i < 4096; i++)
        len += snprintf(line + len, 4, " %02x", p[i]);
    line[len++] = '\n';
    pthread_mutex_lock(&lock);
    real_write(log_fd, line, len);
    pthread_mutex_unlock(&lock);
}

// watched[fd]: 0 = jeszcze nie sprawdzony, 1 = podsłuchiwany, 2 = inny
static void track(int fd, const char *path)
{
    if (fd < 0 || fd >= MAX_FD || fd == log_fd)
        return;
    const char *match = getenv("TTYSNIFF_MATCH");
    if (!match)
        match = "ttyUSB";
    if (path && (match[0] == '/' ? strcmp(path, match) == 0
                                 : strstr(path, match) != NULL)) {
        watched[fd] = 1;
        char msg[512];
        int n = snprintf(msg, sizeof msg, "# fd=%d %s\n", fd, path);
        if (log_fd >= 0)
            real_write(log_fd, msg, n);
    } else {
        watched[fd] = 2;
    }
}

static int is_watched(int fd)
{
    if (fd < 0 || fd >= MAX_FD)
        return 0;
    if (watched[fd] == 0) {
        char link[64], path[512];
        snprintf(link, sizeof link, "/proc/self/fd/%d", fd);
        ssize_t n = readlink(link, path, sizeof path - 1);
        path[n > 0 ? n : 0] = '\0';
        track(fd, n > 0 ? path : NULL);
    }
    return watched[fd] == 1;
}

static void note(const char *fmt, unsigned long a, unsigned long b)
{
    if (log_fd < 0)
        return;
    char msg[128];
    int n = snprintf(msg, sizeof msg, fmt, a, b);
    real_write(log_fd, msg, n);
}

static int prefix_blocked(const char *path)
{
    const char *list = getenv("TTYSNIFF_BLOCK");
    if (!list || !path)
        return 0;
    while (*list) {
        const char *end = strchrnul(list, ':');
        size_t len = end - list;
        if (len && strncmp(path, list, len) == 0)
            return 1;
        list = *end ? end + 1 : end;
    }
    return 0;
}

// Ścieżka po rozwinięciu dowiązań (np. dosdevices/com34 -> /dev/ttyUSB0),
// także względna wobec katalogu dirfd.
static int blocked(int dirfd, const char *path)
{
    if (!getenv("TTYSNIFF_BLOCK") || !path)
        return 0;
    char full[2 * PATH_MAX], real[PATH_MAX];
    if (path[0] == '/' || dirfd == AT_FDCWD) {
        snprintf(full, sizeof full, "%s", path);
    } else {
        char link[64], dir[PATH_MAX];
        snprintf(link, sizeof link, "/proc/self/fd/%d", dirfd);
        ssize_t n = readlink(link, dir, sizeof dir - 1);
        dir[n > 0 ? n : 0] = '\0';
        snprintf(full, sizeof full, "%s/%s", dir, path);
    }
    return prefix_blocked(full)
        || (realpath(full, real) && prefix_blocked(real));
}

static int refuse(const char *path)
{
    if (log_fd >= 0) {
        char msg[512];
        int n = snprintf(msg, sizeof msg, "# zablokowano %s\n", path);
        real_write(log_fd, msg, n);
    }
    errno = EACCES;
    return -1;
}

// Wszystkie odmiany open() przechodzą tędy.
// Po otwarciu sprawdzamy jeszcze, dokąd naprawdę prowadzi deskryptor.
static int guarded_open(int dirfd, const char *path, int flags, mode_t mode)
{
    init();
    if (blocked(dirfd, path))
        return refuse(path);
    int fd = real_openat(dirfd, path, flags, mode);
    if (fd >= 0 && getenv("TTYSNIFF_BLOCK")) {
        char link[64], real[PATH_MAX];
        snprintf(link, sizeof link, "/proc/self/fd/%d", fd);
        ssize_t n = readlink(link, real, sizeof real - 1);
        real[n > 0 ? n : 0] = '\0';
        if (prefix_blocked(real)) {
            real_close(fd);
            return refuse(real);
        }
    }
    track(fd, path);
    return fd;
}

static mode_t get_mode(int flags, va_list ap)
{
    return (flags & (O_CREAT | O_TMPFILE)) ? va_arg(ap, mode_t) : 0;
}

int open(const char *path, int flags, ...)
{
    va_list ap;
    va_start(ap, flags);
    mode_t mode = get_mode(flags, ap);
    va_end(ap);
    return guarded_open(AT_FDCWD, path, flags, mode);
}

int open64(const char *path, int flags, ...)
{
    va_list ap;
    va_start(ap, flags);
    mode_t mode = get_mode(flags, ap);
    va_end(ap);
    return guarded_open(AT_FDCWD, path, flags, mode);
}

int openat(int dirfd, const char *path, int flags, ...)
{
    va_list ap;
    va_start(ap, flags);
    mode_t mode = get_mode(flags, ap);
    va_end(ap);
    return guarded_open(dirfd, path, flags, mode);
}

int openat64(int dirfd, const char *path, int flags, ...)
{
    va_list ap;
    va_start(ap, flags);
    mode_t mode = get_mode(flags, ap);
    va_end(ap);
    return guarded_open(dirfd, path, flags, mode);
}

// Wersje z _FORTIFY_SOURCE.
int __open_2(const char *path, int flags)
{
    return guarded_open(AT_FDCWD, path, flags, 0);
}

int __open64_2(const char *path, int flags)
{
    return guarded_open(AT_FDCWD, path, flags, 0);
}

int __openat_2(int dirfd, const char *path, int flags)
{
    return guarded_open(dirfd, path, flags, 0);
}

int __openat64_2(int dirfd, const char *path, int flags)
{
    return guarded_open(dirfd, path, flags, 0);
}

// fopen() otwiera plik wewnątrz glibc, z pominięciem open().
FILE *fopen(const char *path, const char *mode)
{
    init();
    if (blocked(AT_FDCWD, path)) {
        refuse(path);
        return NULL;
    }
    return real_fopen(path, mode);
}

FILE *fopen64(const char *path, const char *mode)
{
    return fopen(path, mode);
}

int close(int fd)
{
    init();
    if (fd >= 0 && fd < MAX_FD)
        watched[fd] = 0;
    return real_close(fd);
}

ssize_t read(int fd, void *buf, size_t n)
{
    init();
    ssize_t r = real_read(fd, buf, n);
    if (is_watched(fd))
        log_data("<<", buf, r);
    return r;
}

ssize_t write(int fd, const void *buf, size_t n)
{
    init();
    ssize_t r = real_write(fd, buf, n);
    if (is_watched(fd))
        log_data(">>", buf, r);
    return r;
}

// ioctl-e linii modemu, których pseudoterminal nie obsługuje.
static int fake_ioctl(unsigned long req, void *arg)
{
    switch (req) {
    case TIOCMGET:
        *(int *)arg = TIOCM_DTR | TIOCM_RTS | TIOCM_CTS | TIOCM_DSR | TIOCM_CD;
        return 1;
    case TIOCMSET:
    case TIOCMBIS:
    case TIOCMBIC:
    case TIOCSBRK:
    case TIOCCBRK:
    case TIOCSSERIAL:
        return 1;
    case TIOCGSERIAL:
        memset(arg, 0, sizeof(struct serial_struct));
        return 1;
    case TIOCGICOUNT:
        memset(arg, 0, sizeof(struct serial_icounter_struct));
        return 1;
    case TIOCSERGETLSR:
        *(int *)arg = TIOCSER_TEMT;
        return 1;
    }
    return 0;
}

int ioctl(int fd, unsigned long req, ...)
{
    init();
    va_list ap;
    va_start(ap, req);
    void *arg = va_arg(ap, void *);
    va_end(ap);
    int r = real_ioctl(fd, req, arg);
    if (r == -1 && (errno == ENOTTY || errno == EINVAL || errno == EIO)
        && is_watched(fd) && fake_ioctl(req, arg)) {
        note("# ioctl 0x%lx udany na niby (fd=%lu)\n", req, (unsigned long)fd);
        return 0;
    }
    return r;
}
