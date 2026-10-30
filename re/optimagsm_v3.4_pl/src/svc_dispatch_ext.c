// svc_dispatch_ext @ 9d016a04
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* Komendy 0x32–0x45 z tablicy svc_ext_handlers, gdy ustawiona flaga. ❓ tryb ON-LINE */

undefined4 svc_dispatch_ext(char *param_1,undefined4 param_2)

{
  int iVar1;
  undefined4 local_10;
  
  local_10 = 0xffffffff;
  iVar1 = (int)*param_1;
  if (((((DAT_a0015a46 & 1) != 0) || ((_DAT_a000015a & 1) != 0)) && (0x31 < iVar1)) &&
     (iVar1 < 0x46)) {
    local_10 = (**(code **)(&svc_ext_handlers + (iVar1 + -0x32) * 4))(param_1 + 1,param_2);
    if (DAT_a00000e4 != 0) {
      DAT_a00000e4 = DAT_a00000e4 + -1;
    }
  }
  return local_10;
}

