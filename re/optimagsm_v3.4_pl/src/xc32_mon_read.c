// xc32_mon_read @ 9d06aa0c
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* WARNING: Removing unreachable block (ram,0x9d06aa4c) */
/* WARNING: Removing unreachable block (ram,0x9d06aa64) */
/* libc XC32: odczyt stdin z UART1 albo UART2 (wg __XC_UART), odpytywaniem. */

int xc32_mon_read(int param_1,undefined1 *param_2,int param_3)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  uint *puVar5;
  undefined4 *puVar6;
  
  if (param_2 == (undefined1 *)0x0) {
    return 0;
  }
  iVar1 = 0;
  if ((param_3 != 0) && (iVar1 = param_3, param_1 == 0)) {
    if (__XC_UART == 2) {
      uVar2 = DAT_bf806800;
      puVar6 = &DAT_bf806830;
      puVar5 = &DAT_bf806810;
    }
    else {
      uVar2 = DAT_bf806000;
      puVar6 = &DAT_bf806030;
      puVar5 = &DAT_bf806010;
    }
    iVar3 = param_3;
    if ((uVar2 & 0x8000) == 0) {
      DAT_bf806008 = 0x8000;
    }
    do {
      iVar4 = 0xa0;
      if ((*puVar5 & 1) == 0) {
        do {
          iVar4 = iVar4 + -1;
          if ((*puVar5 & 1) != 0) break;
        } while (iVar4 != 0);
      }
      if ((*puVar5 & 1) != 0) {
        return param_3 - iVar3;
      }
      iVar3 = iVar3 + -1;
      *param_2 = (char)*puVar6;
      param_2 = param_2 + 1;
    } while (iVar3 != 0);
  }
  return iVar1;
}

