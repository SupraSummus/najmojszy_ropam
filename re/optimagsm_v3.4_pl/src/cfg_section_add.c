// cfg_section_add @ 9d04cb2c
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* Dopisuje sekcję do katalogu: przesunięcie = poprzednie + rozmiar + 8, najwyżej 28 sekcji. */

undefined4 cfg_section_add(undefined4 param_1,undefined2 param_2)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = (int)g_cfg_nsections;
  if (iVar1 < 0x1c) {
    strcpy(iVar1 * 0xe + -0x5fff7af2,param_1);
    if (iVar1 == 0) {
      _DAT_a0008516 = 0;
    }
    else {
      *(short *)(&DAT_a0008516 + iVar1 * 0xe) =
           *(short *)(&DAT_a0008516 + (iVar1 + -1) * 0xe) +
           *(short *)(&DAT_a0008518 + (iVar1 + -1) * 0xe) + 8;
    }
    *(undefined2 *)(&DAT_a0008518 + iVar1 * 0xe) = param_2;
    g_cfg_nsections = g_cfg_nsections + '\x01';
    uVar2 = 1;
  }
  else {
    uVar2 = 0;
  }
  return uVar2;
}

