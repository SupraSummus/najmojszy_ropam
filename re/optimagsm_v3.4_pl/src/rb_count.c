// rb_count @ 9d036284
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Liczba bajtów w buforze. ❓ */

int rb_count(int param_1)

{
  int iVar1;
  
  iVar1 = *(int *)(param_1 + 0x24) - *(int *)(param_1 + 0x28);
  if (*(ushort *)(param_1 + 0x2e) == 0) {
    trap(7);
  }
  return (int)(-(iVar1 >> 0x1f) * (uint)*(ushort *)(param_1 + 0x40) + iVar1) /
         (int)(uint)*(ushort *)(param_1 + 0x2e);
}

