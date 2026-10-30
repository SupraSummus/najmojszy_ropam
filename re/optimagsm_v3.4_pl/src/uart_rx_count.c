// uart_rx_count @ 9d0507d8
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Liczba bajtów w buforze odbiorczym portu. */

void uart_rx_count(int param_1)

{
  rb_count((&g_uart_rxbuf)[param_1]);
  return;
}

