// _reset @ 9d001000
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* WARNING: This function may have set the stack pointer */
/* WARNING: Removing unreachable block (ram,0x9d001124) */
/* Start aplikacji za bootloaderem (crt0 XC32): $sp=0xA001FFE8, $gp=0xA0007FF0. */

void _reset(void)

{
  undefined1 uVar1;
  undefined4 *puVar2;
  undefined **ppuVar3;
  undefined **ppuVar4;
  undefined1 *puVar5;
  undefined *puVar6;
  
  FUN_9d001008();
  if ((int)(Status << 0xc) < 0) {
    (*(code *)&LAB_9d06e474)();
    return;
  }
  setCopReg(0,SRSCtl,SRSCtl & 0xfffffc3f | ((SRSCtl & 0x3fffffff) >> 0x1a) << 6);
  setShadow(0xa0007ff0,0xa0007ff0);
  setCopReg(0,SRSCtl,SRSCtl);
  FUN_9d06e630();
  for (puVar2 = (undefined4 *)&DAT_a0000074; puVar2 < (undefined4 *)0xa00169d0; puVar2 = puVar2 + 4)
  {
    *puVar2 = 0;
    puVar2[1] = 0;
    puVar2[2] = 0;
    puVar2[3] = 0;
  }
  ppuVar3 = &PTR_DAT_9d0590bc;
  do {
    puVar5 = *ppuVar3;
    if (puVar5 == (undefined1 *)0x0) break;
    puVar6 = ppuVar3[1];
    ppuVar4 = ppuVar3 + 3;
    if (ppuVar3[2] == (undefined *)0x0) {
      do {
        *puVar5 = 0;
        puVar6 = puVar6 + -1;
        puVar5 = puVar5 + 1;
      } while (puVar6 != (undefined *)0x0);
    }
    else {
      do {
        uVar1 = *(undefined1 *)ppuVar4;
        puVar6 = puVar6 + -1;
        ppuVar4 = (undefined **)((int)ppuVar4 + 1);
        *puVar5 = uVar1;
        puVar5 = puVar5 + 1;
      } while (puVar6 != (undefined *)0x0);
    }
    ppuVar3 = (undefined **)((int)ppuVar4 + 3U & 0xfffffffc);
  } while (*ppuVar3 != (undefined *)0x0);
  setCopReg(0,Count,0);
  setCopReg(0,Compare,0xffffffff);
  setCopReg(0,Status,Status | 0x400000);
  setCopReg(0,EBase,0x9d000000);
  setCopReg(0,IntCtl,0x20);
  setCopReg(0,Cause,0x800000);
  setCopReg(0,Status,((Config & 0x7fffff) >> 0x16) << 0x11 | Status & 0x580000);
  FUN_9d06e638();
  setCopReg(0,Status,Status & 0xffbfffff);
  FUN_9d06d558(0,0);
  return;
}

