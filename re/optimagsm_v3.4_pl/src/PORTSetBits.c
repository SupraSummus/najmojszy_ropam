// PORTSetBits @ 9d06e150
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Zapis maski do LATxSET; (5, 8) ustawia RF3 = nadawanie RS-485. */

void PORTSetBits(int param_1,undefined4 param_2)

{
  *(undefined4 *)((&PTR_DAT_9d06d600)[param_1 * 3] + 0x28) = param_2;
  return;
}

