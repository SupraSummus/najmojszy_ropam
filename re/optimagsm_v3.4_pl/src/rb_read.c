// rb_read @ 9d036828
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Odczyt bajtów z bufora. ❓ */

void rb_read(int param_1,undefined4 *param_2,int param_3)

{
  int iVar1;
  uint uVar2;
  undefined4 *puVar3;
  int iVar4;
  int iVar5;
  
  *DAT_a0000058 = DAT_a0000204;
  iVar1 = *(int *)(param_1 + 0x24) - *(int *)(param_1 + 0x28);
  iVar4 = param_3 * (uint)*(ushort *)(param_1 + 0x2e);
  if (((int)(-(iVar1 >> 0x1f) * (uint)*(ushort *)(param_1 + 0x40) + iVar1) < iVar4) ||
     (*(short *)(param_1 + 0x38) != 0)) {
    FUN_9d035f8c(param_1,param_2,param_3,2);
    *DAT_a0000054 = DAT_a0000204;
    return;
  }
  puVar3 = *(undefined4 **)(param_1 + 0x28);
  iVar1 = *(int *)(param_1 + 0x20) - (int)puVar3;
  if (iVar1 <= iVar4) {
    uVar2 = (uint)*(ushort *)(param_1 + 0x42);
    iVar5 = iVar1;
    if (uVar2 == 4) {
      while( true ) {
        *param_2 = *puVar3;
        param_2 = param_2 + 1;
        if (iVar5 + -4 == 0) break;
        puVar3 = puVar3 + 1;
        iVar5 = iVar5 + -4;
      }
      iVar4 = iVar4 - iVar1;
    }
    else if (uVar2 == 2) {
      while( true ) {
        *(undefined2 *)param_2 = *(undefined2 *)puVar3;
        param_2 = (undefined4 *)((int)param_2 + 2);
        if (iVar5 + -2 == 0) break;
        puVar3 = (undefined4 *)((int)puVar3 + 2);
        iVar5 = iVar5 + -2;
      }
      iVar4 = iVar4 - iVar1;
    }
    else {
      iVar5 = iVar1 - uVar2;
      while( true ) {
        iVar5 = iVar5 - uVar2;
        *(undefined1 *)param_2 = *(undefined1 *)puVar3;
        param_2 = (undefined4 *)((int)param_2 + uVar2);
        if (iVar5 + uVar2 == 0) break;
        puVar3 = (undefined4 *)((int)puVar3 + uVar2);
      }
      iVar4 = iVar4 - iVar1;
    }
    puVar3 = *(undefined4 **)(param_1 + 0x1c);
    if (iVar4 == 0) goto LAB_9d036934;
  }
  uVar2 = (uint)*(ushort *)(param_1 + 0x42);
  if (iVar4 != 0) {
    if (uVar2 == 4) {
      while( true ) {
        iVar4 = iVar4 + -4;
        *param_2 = *puVar3;
        puVar3 = puVar3 + 1;
        if (iVar4 == 0) break;
        param_2 = param_2 + 1;
      }
    }
    else if (uVar2 == 2) {
      while( true ) {
        iVar4 = iVar4 + -2;
        *(undefined2 *)param_2 = *(undefined2 *)puVar3;
        puVar3 = (undefined4 *)((int)puVar3 + 2);
        if (iVar4 == 0) break;
        param_2 = (undefined4 *)((int)param_2 + 2);
      }
    }
    else {
      iVar4 = iVar4 - uVar2;
      while( true ) {
        iVar4 = iVar4 - uVar2;
        *(undefined1 *)param_2 = *(undefined1 *)puVar3;
        puVar3 = (undefined4 *)((int)puVar3 + uVar2);
        if (iVar4 + uVar2 == 0) break;
        param_2 = (undefined4 *)((int)param_2 + uVar2);
      }
    }
  }
LAB_9d036934:
  *(undefined4 **)(param_1 + 0x28) = puVar3;
  *DAT_a0000054 = DAT_a0000204;
  return;
}

