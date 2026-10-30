// svc_getc @ 9d04e48c
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Bajt z portu 2 albo 3 z limitem czasu. */

undefined1 svc_getc(undefined4 param_1)

{
  int iVar1;
  undefined1 local_10 [8];
  
  local_10[0] = 0;
  DAT_a0000078 = 0;
  if ((DAT_a000277c._1_1_ & 0x7f) >> 6 == 0) {
    iVar1 = uart_read(2,local_10,1,param_1);
    if (iVar1 != 1) {
      DAT_a0000078 = 1;
    }
  }
  else {
    iVar1 = uart_read(3,local_10,1,param_1);
    if (iVar1 != 1) {
      DAT_a0000078 = 1;
    }
  }
  return local_10[0];
}

