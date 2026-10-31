// output_force_off @ 9d008838
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Wymusza wyłączenie wyjścia (bit 2 bajtu stanu). */

void output_force_off(int param_1)

{
  int iVar1;
  byte *pbVar2;
  
  iVar1 = shared_lock(DAT_a0000164);
  pbVar2 = (byte *)(iVar1 + param_1);
  *pbVar2 = *pbVar2 & 0xfb | 4;
  *pbVar2 = *pbVar2 & 0xf7;
  shared_unlock(DAT_a0000164);
  return;
}

