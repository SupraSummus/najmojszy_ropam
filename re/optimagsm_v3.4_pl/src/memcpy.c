// memcpy @ 9d06a5e4
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

int * memcpy(int *param_1,int *param_2,uint param_3)

{
  uint *puVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  int *piVar5;
  uint uVar6;
  uint uVar7;
  int *piVar8;
  int *piVar9;
  int iVar10;
  int iVar11;
  
  if (param_3 != 0) {
    piVar5 = param_1;
    if (0xb < param_3) {
      uVar7 = -(int)param_1 & 3;
      if ((((uint)param_2 ^ (uint)param_1) & 3) == 0) {
        uVar6 = param_3 - uVar7;
        if (uVar7 != 0) {
          uVar2 = (uint)param_2 & 3;
          puVar1 = (uint *)((int)param_2 - uVar2);
          param_2 = (int *)((int)param_2 + uVar7);
          uVar3 = (uint)param_1 & 3;
          *(uint *)((int)param_1 - uVar3) =
               *(uint *)((int)param_1 - uVar3) & 0xffffffffU >> (4 - uVar3) * 8 |
               (*puVar1 >> uVar2 * 8) << uVar3 * 8;
          piVar5 = (int *)((int)param_1 + uVar7);
        }
        iVar4 = uVar6 - (uVar6 & 0xf);
        if (iVar4 != 0) {
          piVar8 = (int *)(iVar4 + (int)param_2);
          piVar9 = piVar5;
          do {
            iVar4 = param_2[1];
            iVar10 = param_2[2];
            iVar11 = param_2[3];
            *piVar9 = *param_2;
            piVar9[1] = iVar4;
            piVar9[2] = iVar10;
            param_2 = param_2 + 4;
            piVar5 = piVar9 + 4;
            piVar9[3] = iVar11;
            piVar9 = piVar5;
          } while (param_2 != piVar8);
        }
        param_3 = uVar6 & 3;
        iVar4 = (uVar6 & 0xf) - param_3;
        if (iVar4 != 0) {
          piVar8 = (int *)(iVar4 + (int)param_2);
          piVar9 = piVar5;
          do {
            iVar4 = *param_2;
            param_2 = param_2 + 1;
            piVar5 = piVar9 + 1;
            *piVar9 = iVar4;
            piVar9 = piVar5;
          } while (param_2 != piVar8);
        }
      }
      else {
        uVar6 = param_3 - uVar7;
        if (uVar7 != 0) {
          iVar4 = *param_2;
          param_2 = (int *)((int)param_2 + uVar7);
          uVar2 = (uint)param_1 & 3;
          *(uint *)((int)param_1 - uVar2) =
               *(uint *)((int)param_1 - uVar2) & 0xffffffffU >> (4 - uVar2) * 8 | iVar4 << uVar2 * 8
          ;
          piVar5 = (int *)((int)param_1 + uVar7);
        }
        iVar4 = uVar6 - (uVar6 & 0xf);
        if (iVar4 != 0) {
          piVar8 = (int *)(iVar4 + (int)param_2);
          piVar9 = piVar5;
          do {
            iVar4 = param_2[1];
            iVar10 = param_2[2];
            iVar11 = param_2[3];
            *piVar9 = *param_2;
            piVar9[1] = iVar4;
            piVar9[2] = iVar10;
            param_2 = param_2 + 4;
            piVar5 = piVar9 + 4;
            piVar9[3] = iVar11;
            piVar9 = piVar5;
          } while (param_2 != piVar8);
        }
        param_3 = uVar6 & 3;
        iVar4 = (uVar6 & 0xf) - param_3;
        if (iVar4 != 0) {
          piVar8 = (int *)(iVar4 + (int)param_2);
          piVar9 = piVar5;
          do {
            iVar4 = *param_2;
            param_2 = param_2 + 1;
            piVar5 = piVar9 + 1;
            *piVar9 = iVar4;
            piVar9 = piVar5;
          } while (param_2 != piVar8);
        }
      }
    }
    piVar9 = (int *)(param_3 + (int)param_2);
    if (param_3 != 0) {
      do {
        iVar4 = *param_2;
        param_2 = (int *)((int)param_2 + 1);
        *(char *)piVar5 = (char)iVar4;
        piVar5 = (int *)((int)piVar5 + 1);
      } while (param_2 != piVar9);
    }
  }
  return param_1;
}

