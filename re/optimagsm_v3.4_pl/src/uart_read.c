// uart_read @ 9d050d94
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Odczyt N bajtów z portu z limitem czasu. */

undefined4 uart_read(char param_1,undefined4 param_2,undefined4 param_3,int param_4)

{
  undefined4 uVar1;
  
  FUN_9d05a028((param_4 * 1000) / 100 + (uint)((param_4 * 1000) % 100 != 0));
  uVar1 = rb_read((&g_uart_rxbuf)[param_1],param_2,param_3);
  FUN_9d05a090();
  return uVar1;
}

