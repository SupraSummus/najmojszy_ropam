// svc_flush @ 9d04e3c4
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Opróżnia bufor portu 2 albo 3. */

void svc_flush(void)

{
  if ((DAT_a000277c._1_1_ & 0x7f) >> 6 == 0) {
    uart_rx_flush(2);
  }
  else {
    uart_rx_flush(3);
  }
  os_delay(100);
  return;
}

