// zone_outputs_off @ 9d004c2c
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Wyłącza aktywne wyjścia danej strefy (sygnalizatory ❓); komenda 0x31. */

undefined1 zone_outputs_off(undefined4 param_1)

{
  int iVar1;
  int local_10;
  undefined1 local_c;
  
  local_c = 0;
  for (local_10 = 0; local_10 < 0x20; local_10 = local_10 + 1) {
    iVar1 = FUN_9d0082ac(param_1,local_10);
    if ((iVar1 != 0) &&
       ((((*(uint *)(&DAT_a0002948 + local_10 * 0x10) & 7) >> 2 != 0 ||
         ((*(uint *)(&DAT_a0002948 + local_10 * 0x10) & 0x1fff) >> 0xc != 0)) &&
        (iVar1 = output_is_on(local_10), iVar1 != 0)))) {
      local_c = 1;
      output_force_off(local_10);
    }
  }
  if (((DAT_a000277c._1_1_ & 0x1f) >> 4 != 0) && ((DAT_a0015a74 & 0x1fff) >> 0xc != 0)) {
    FUN_9d01fa70();
    FUN_9d030148();
  }
  return local_c;
}

