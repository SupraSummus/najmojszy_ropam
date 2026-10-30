// memset @ 9d06c324
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

uint * memset(uint *param_1,uint param_2,uint param_3)

{
  uint uVar1;
  uint *puVar2;
  uint *puVar3;
  int iVar4;
  uint *puVar5;
  
  if (param_3 != 0) {
    param_2 = param_2 & 0xff;
    puVar2 = param_1;
    if (0xf < param_3) {
      param_2 = param_2 | param_2 << 8;
      param_2 = param_2 | param_2 << 0x10;
      if (((uint)param_1 & 3) != 0) {
        iVar4 = 4 - ((uint)param_1 & 3);
        param_3 = param_3 - iVar4;
        uVar1 = (uint)param_1 & 3;
        *(uint *)((int)param_1 - uVar1) =
             *(uint *)((int)param_1 - uVar1) & 0xffffffffU >> (4 - uVar1) * 8 | param_2 << uVar1 * 8
        ;
        puVar2 = (uint *)((int)param_1 + iVar4);
      }
      uVar1 = param_3 & 0xf;
      iVar4 = param_3 - uVar1;
      if (iVar4 != 0) {
        puVar5 = (uint *)(iVar4 + (int)puVar2);
        puVar3 = puVar2;
        do {
          *puVar3 = param_2;
          puVar3[1] = param_2;
          puVar3[2] = param_2;
          puVar2 = puVar3 + 4;
          puVar3[3] = param_2;
          puVar3 = puVar2;
        } while (puVar2 != puVar5);
      }
      param_3 = param_3 & 3;
      iVar4 = uVar1 - param_3;
      if (iVar4 != 0) {
        puVar5 = (uint *)(iVar4 + (int)puVar2);
        puVar3 = puVar2;
        do {
          puVar2 = puVar3 + 1;
          *puVar3 = param_2;
          puVar3 = puVar2;
        } while (puVar2 != puVar5);
      }
    }
    puVar3 = puVar2;
    if (param_3 != 0) {
      do {
        puVar5 = (uint *)((int)puVar3 + 1);
        *(char *)puVar3 = (char)param_2;
        puVar3 = puVar5;
      } while (puVar5 != (uint *)(param_3 + (int)puVar2));
    }
  }
  return param_1;
}

