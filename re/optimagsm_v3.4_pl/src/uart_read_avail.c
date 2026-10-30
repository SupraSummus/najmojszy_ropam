// uart_read_avail @ 9d050eb4
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Odczyt dostępnych bajtów (najwyżej N). */

undefined4 uart_read_avail(char param_1,undefined4 param_2,int param_3)

{
  undefined4 uVar1;
  int local_10;
  
  local_10 = uart_rx_count((int)param_1);
  if (local_10 == 0) {
    uVar1 = 0;
  }
  else {
    if (param_3 < local_10) {
      local_10 = param_3;
    }
    uVar1 = rb_read((&g_uart_rxbuf)[param_1],param_2,local_10);
  }
  return uVar1;
}

