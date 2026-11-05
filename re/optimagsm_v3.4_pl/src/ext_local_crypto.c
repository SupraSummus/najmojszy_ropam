// ext_local_crypto @ 9d0199fc
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* Komenda 0x42: w trybie serwisowym odpowiada 0 i ustawia bit 5 pod 0xA000277E (VMPC na porcie
   lokalnym, restart po bezczynności); inaczej 1. */

undefined4 ext_local_crypto(undefined4 param_1,undefined1 *param_2)

{
  if ((_g_sys_flags & 1) == 0) {
    *param_2 = 1;
  }
  else {
    *param_2 = 0;
    DAT_a000277e = DAT_a000277e & 0xdf | 0x20;
  }
  xmodem_transmit(param_2,1);
  return 0;
}

