// uart_wait_bytes @ 9d05321c
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Czeka, aż w buforze portu będzie co najmniej N bajtów. */

bool uart_wait_bytes(char param_1,char param_2,int param_3)

{
  uint uVar1;
  int local_10;
  
  for (local_10 = 0;
      (uVar1 = uart_rx_count((int)param_1), uVar1 < (uint)(int)param_2 && (local_10 < param_3 * 10))
      ; local_10 = local_10 + 1) {
    busy_wait_t1(100);
  }
  uVar1 = uart_rx_count((int)param_1);
  return (uint)(int)param_2 <= uVar1;
}

