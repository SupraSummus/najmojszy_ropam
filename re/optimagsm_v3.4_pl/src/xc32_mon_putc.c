// xc32_mon_putc @ 9d06bdec
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* WARNING: Removing unreachable block (ram,0x9d06beb0) */
/* WARNING: Removing unreachable block (ram,0x9d06be14) */
/* WARNING: Removing unreachable block (ram,0x9d06be20) */
/* libc XC32: zapis znaku stdout na UART1 albo UART2. */

void xc32_mon_putc(char param_1)

{
  undefined4 *puVar1;
  uint *puVar2;
  int *piVar3;
  undefined4 *puVar4;
  
  if ((__XC_UART == 0) || (__XC_UART != 1)) {
    piVar3 = &DAT_bf806820;
    puVar1 = &DAT_bf806818;
    puVar4 = &DAT_bf806808;
    puVar2 = &DAT_bf806810;
  }
  else {
    piVar3 = &DAT_bf806020;
    puVar1 = &DAT_bf806018;
    puVar4 = &DAT_bf806008;
    puVar2 = &DAT_bf806010;
  }
  *puVar4 = 0x8000;
  *puVar1 = 0x400;
  do {
  } while ((*puVar2 & 0x100) == 0);
  *piVar3 = (int)param_1;
  return;
}

