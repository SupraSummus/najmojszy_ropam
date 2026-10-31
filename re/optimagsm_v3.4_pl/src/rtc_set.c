// rtc_set @ 9d041f64
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Ustawienie zegara (komenda 0x0D, 8 B). */

void rtc_set(undefined4 param_1)

{
  FUN_9d05e5c0(DAT_a00000fc,param_1);
  FUN_9d0425a4(param_1);
  if ((DAT_a0015a74 & 0x1ffffff) >> 0x18 != 1) {
    FUN_9d042a30((int)DAT_a0015a7d);
  }
  return;
}

