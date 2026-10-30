// rnet_read_byte @ 9d052bc0
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Odczyt bajtu z limitem czasu; ustawia g_rnet_timeout. */

undefined1 rnet_read_byte(char param_1)

{
  int iVar1;
  undefined1 local_10 [8];
  
  iVar1 = uart_read((int)param_1,local_10,1,0x32);
  g_rnet_timeout = iVar1 == 0;
  return local_10[0];
}

