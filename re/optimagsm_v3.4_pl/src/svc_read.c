// svc_read @ 9d04e5a4
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Odczyt N bajtów z portu 2 albo 3. */

bool svc_read(undefined4 param_1,int param_2,undefined4 param_3)

{
  int iVar1;
  undefined1 local_10;
  
  local_10 = 2;
  DAT_a0000078 = 0;
  if ((DAT_a000277c._1_1_ & 0x7f) >> 6 != 0) {
    local_10 = 3;
  }
  iVar1 = uart_read(local_10,param_1,param_2,param_3);
  return iVar1 == param_2;
}

