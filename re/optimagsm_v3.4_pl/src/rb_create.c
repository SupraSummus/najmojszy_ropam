// rb_create @ 9d0362f0
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Tworzy bufor/kolejkę bajtów. ❓ FreeRTOS */

int rb_create(undefined4 param_1,int param_2,uint param_3,int param_4,undefined4 param_5)

{
  short sVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  
  iVar2 = FUN_9d0667b8(param_1,6,0x4c);
  iVar4 = param_3 * param_2 + 4;
  iVar3 = FUN_9d06d5ac(iVar4,4);
  *(int *)(iVar2 + 0x24) = iVar3;
  *(bool *)(iVar2 + 0x39) = param_4 != 0;
  *(int *)(iVar2 + 0x28) = iVar3;
  *(int *)(iVar2 + 0x1c) = iVar3;
  sVar1 = (short)iVar4 + -4;
  *(undefined4 *)(iVar2 + 0x34) = param_5;
  *(short *)(iVar2 + 0x40) = (short)iVar4;
  *(short *)(iVar2 + 0x2c) = sVar1;
  *(int *)(iVar2 + 0x20) = iVar3 + iVar4;
  *(short *)(iVar2 + 0x2e) = (short)param_3;
  *(int *)(iVar2 + 0x30) = param_4;
  *(undefined1 *)(iVar2 + 0x38) = 0;
  *(short *)(iVar2 + 0x3e) = sVar1;
  *(undefined2 *)(iVar2 + 0x3c) = 0x7fff;
  if ((param_3 & 3) == 0) {
    *(undefined2 *)(iVar2 + 0x42) = 4;
  }
  else if ((param_3 & 1) == 0) {
    *(undefined2 *)(iVar2 + 0x42) = 2;
  }
  else {
    *(undefined2 *)(iVar2 + 0x42) = 1;
  }
  FUN_9d06a3c8(0xf,0,iVar2 + 0x44);
  *(uint *)(iVar2 + 0x18) = *(uint *)(iVar2 + 0x18) & 0xfffffeff | 0x100;
  *DAT_a0000054 = DAT_a0000204;
  return iVar2;
}

