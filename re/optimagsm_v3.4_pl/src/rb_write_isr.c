// rb_write_isr @ 9d036a34
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Zapis bajtów do bufora z przerwania. ❓ */

int rb_write_isr(int param_1,undefined4 *param_2,int param_3)

{
  short sVar1;
  ushort uVar2;
  int iVar3;
  undefined4 *puVar4;
  uint uVar5;
  uint uVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  
  sVar1 = *(short *)(param_1 + 0x3c);
  if (sVar1 < 0) {
    return -1;
  }
  uVar6 = (uint)*(ushort *)(param_1 + 0x2e);
  uVar5 = *(int *)(param_1 + 0x24) - *(int *)(param_1 + 0x28);
  iVar9 = param_3 * uVar6;
  iVar3 = -((int)~uVar5 >> 0x1f) * (uint)*(ushort *)(param_1 + 0x40) + (-4 - uVar5);
  iVar8 = iVar3 - iVar9;
  if (iVar3 < iVar9) {
    param_3 = iVar3 / (int)uVar6;
    if (uVar6 == 0) {
      trap(7);
    }
    iVar8 = 0;
    if (0 < *(short *)(param_1 + 0x3e)) {
      *(undefined2 *)(param_1 + 0x3e) = 0;
    }
  }
  else {
    iVar3 = iVar9;
    if (iVar8 < *(short *)(param_1 + 0x3e)) {
      *(short *)(param_1 + 0x3e) = (short)iVar8;
    }
  }
  if (iVar3 == 0) {
    return param_3;
  }
  puVar4 = *(undefined4 **)(param_1 + 0x24);
  iVar9 = *(int *)(param_1 + 0x20) - (int)puVar4;
  if (iVar3 < iVar9) {
LAB_9d036b10:
    uVar5 = (uint)*(ushort *)(param_1 + 0x42);
    if (uVar5 == 4) {
      while( true ) {
        iVar3 = iVar3 + -4;
        *puVar4 = *param_2;
        puVar4 = puVar4 + 1;
        if (iVar3 == 0) break;
        param_2 = param_2 + 1;
      }
      *(undefined4 **)(param_1 + 0x24) = puVar4;
      iVar8 = (uint)*(ushort *)(param_1 + 0x2c) - iVar8;
      goto joined_r0x9d036c60;
    }
    if (uVar5 != 2) {
      iVar3 = iVar3 - uVar5;
      while( true ) {
        iVar3 = iVar3 - uVar5;
        *(undefined1 *)puVar4 = *(undefined1 *)param_2;
        puVar4 = (undefined4 *)((int)puVar4 + uVar5);
        if (iVar3 + uVar5 == 0) break;
        param_2 = (undefined4 *)((int)param_2 + uVar5);
      }
      goto LAB_9d036b54;
    }
    while( true ) {
      iVar3 = iVar3 + -2;
      *(undefined2 *)puVar4 = *(undefined2 *)param_2;
      puVar4 = (undefined4 *)((int)puVar4 + 2);
      if (iVar3 == 0) break;
      param_2 = (undefined4 *)((int)param_2 + 2);
    }
    uVar2 = *(ushort *)(param_1 + 0x2c);
  }
  else {
    uVar5 = (uint)*(ushort *)(param_1 + 0x42);
    if (iVar9 != 0) {
      iVar7 = iVar9;
      if (uVar5 == 4) {
        while( true ) {
          iVar7 = iVar7 + -4;
          *puVar4 = *param_2;
          param_2 = param_2 + 1;
          if (iVar7 == 0) break;
          puVar4 = puVar4 + 1;
        }
      }
      else if (uVar5 == 2) {
        while( true ) {
          *(undefined2 *)puVar4 = *(undefined2 *)param_2;
          param_2 = (undefined4 *)((int)param_2 + 2);
          if (iVar7 + -2 == 0) break;
          puVar4 = (undefined4 *)((int)puVar4 + 2);
          iVar7 = iVar7 + -2;
        }
      }
      else {
        iVar7 = iVar9 - uVar5;
        while( true ) {
          iVar7 = iVar7 - uVar5;
          *(undefined1 *)puVar4 = *(undefined1 *)param_2;
          param_2 = (undefined4 *)((int)param_2 + uVar5);
          if (iVar7 + uVar5 == 0) break;
          puVar4 = (undefined4 *)((int)puVar4 + uVar5);
        }
      }
    }
    iVar3 = iVar3 - iVar9;
    puVar4 = *(undefined4 **)(param_1 + 0x1c);
    if (iVar3 != 0) goto LAB_9d036b10;
LAB_9d036b54:
    uVar2 = *(ushort *)(param_1 + 0x2c);
  }
  *(undefined4 **)(param_1 + 0x24) = puVar4;
  iVar8 = (uint)uVar2 - iVar8;
joined_r0x9d036c60:
  if (iVar8 < sVar1) {
    return param_3;
  }
  *(undefined2 *)(param_1 + 0x3c) = 0x7fff;
  FUN_9d0611fc(&LAB_9d035e30,param_1);
  return param_3;
}

