// svc_putc @ 9d04e420
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Bajt na port 2 albo 3 (wg trybu lokalny/zdalny). */

void svc_putc(char param_1)

{
  if ((DAT_a000277c._1_1_ & 0x7f) >> 6 == 0) {
    uart_putc(2,(int)param_1);
  }
  else {
    uart_putc(3,(int)param_1);
  }
  return;
}

