// output_force_on @ 9d0087c4
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Wymusza włączenie wyjścia (bit 3 bajtu stanu), zeruje jego czas; komenda 0x2C. */

void output_force_on(int param_1)

{
  int iVar1;
  byte *pbVar2;
  
  iVar1 = shared_lock(DAT_a0000164);
  pbVar2 = (byte *)(iVar1 + param_1);
  *pbVar2 = *pbVar2 & 0xf7 | 8;
  *pbVar2 = *pbVar2 & 0xfb;
  shared_unlock(DAT_a0000164);
  return;
}

