// uart_rx_flush @ 9d050d4c
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Opróżnia bufor odbiorczy portu. */

void uart_rx_flush(char param_1)

{
  rb_reset((&g_uart_rxbuf)[param_1]);
  return;
}

