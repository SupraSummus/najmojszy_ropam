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
#define _GNU_SOURCE
#include <dlfcn.h>
#include <fcntl.h>
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
static int (*real_open)(const char *, int, ...);
static int (*real_open64)(const char *, int, ...);
static int (*real_openat)(int, const char *, int, ...);
static int (*real_close)(int);

static void init(void)
{
    if (real_read)
        return;
    real_read = dlsym(RTLD_NEXT, "read");
    real_write = dlsym(RTLD_NEXT, "write");
    real_open = dlsym(RTLD_NEXT, "open");
    real_open64 = dlsym(RTLD_NEXT, "open64");
    real_openat = dlsym(RTLD_NEXT, "openat");
    real_close = dlsym(RTLD_NEXT, "close");
    const char *path = getenv("TTYSNIFF_LOG");
    if (path)
        log_fd = real_open(path, O_WRONLY | O_CREAT | O_APPEND, 0600);
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
    if (path && strstr(path, match ? match : "ttyUSB")) {
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

static mode_t get_mode(int flags, va_list ap)
{
    return (flags & (O_CREAT | O_TMPFILE)) ? va_arg(ap, mode_t) : 0;
}

int open(const char *path, int flags, ...)
{
    init();
    va_list ap;
    va_start(ap, flags);
    int fd = real_open(path, flags, get_mode(flags, ap));
    va_end(ap);
    track(fd, path);
    return fd;
}

int open64(const char *path, int flags, ...)
{
    init();
    va_list ap;
    va_start(ap, flags);
    int fd = real_open64(path, flags, get_mode(flags, ap));
    va_end(ap);
    track(fd, path);
    return fd;
}

int openat(int dirfd, const char *path, int flags, ...)
{
    init();
    va_list ap;
    va_start(ap, flags);
    int fd = real_openat(dirfd, path, flags, get_mode(flags, ap));
    va_end(ap);
    track(fd, path);
    return fd;
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
