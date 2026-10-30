// busy_wait_t1 @ 9d067640
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Aktywne czekanie na Timer1; używane przy przełączaniu kierunku RS-485. ❓ jednostka */

void busy_wait_t1(uint param_1)

{
  uint uVar1;
  
  DAT_bf881064 = 0x100;
  DAT_bf881068 = 0;
  DAT_bf800800 = 0;
  DAT_bf800800 = 0x40;
  DAT_bf800810 = 0;
  DAT_bf800820 = 0xffff;
  DAT_bf800808 = 0x8000;
  DAT_bf800810 = 0;
  if (param_1 < 2) {
    FUN_9d06783c(2000);
  }
  else {
    do {
      uVar1 = DAT_bf800810;
    } while (uVar1 < param_1 * 5 - 5);
  }
  return;
}

