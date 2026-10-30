// rb_reset @ 9d036428
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Opróżnienie bufora. ❓ */

void rb_reset(int *param_1)

{
  int iVar1;
  
  *DAT_a0000058 = DAT_a0000204;
  *(undefined2 *)(param_1 + 0xf) = 0xffff;
  if ((int *)*param_1 == param_1) {
    iVar1 = param_1[7];
  }
  else {
    do {
      FUN_9d035ecc(param_1[1]);
    } while ((int *)*param_1 != param_1);
    iVar1 = param_1[7];
  }
  param_1[9] = iVar1;
  param_1[10] = iVar1;
  *(undefined2 *)(param_1 + 0xf) = 0x7fff;
  *DAT_a0000054 = DAT_a0000204;
  return;
}

