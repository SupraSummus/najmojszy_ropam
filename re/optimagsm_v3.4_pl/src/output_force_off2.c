// output_force_off2 @ 9d008740
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Wymusza wyłączenie i kasuje stan automatyczny (bit 0); komenda 0x2D. */

void output_force_off2(int param_1)

{
  int iVar1;
  byte *pbVar2;
  
  iVar1 = shared_lock(DAT_a0000164);
  pbVar2 = (byte *)(iVar1 + param_1);
  *pbVar2 = *pbVar2 & 0xfb | 4;
  *pbVar2 = *pbVar2 & 0xf7;
  *pbVar2 = *pbVar2 & 0xfe;
  shared_unlock(DAT_a0000164);
  return;
}

