// PORTClearBits @ 9d06e128
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Zapis maski do LATxCLR; (5, 8) zeruje RF3 = odbiór RS-485. */

void PORTClearBits(int param_1,undefined4 param_2)

{
  *(undefined4 *)((&PTR_DAT_9d06d600)[param_1 * 3] + 0x24) = param_2;
  return;
}

