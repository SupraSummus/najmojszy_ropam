// UARTSetDataRate @ 9d06d4fc
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* PLIB: UxBRG = zegar / (16 albo 4 × prędkość) − 1. */

uint UARTSetDataRate(int param_1,uint param_2,uint param_3)

{
  uint uVar1;
  uint uVar2;
  
  uVar1 = param_2 >> 1;
  if ((*(uint *)(&uart_base_table)[param_1] & 8) == 0) {
    uVar1 = param_2 >> 3;
  }
  if (param_3 == 0) {
    trap(7);
  }
  uVar2 = uVar1 / param_3 + 1 >> 1;
  *(uint *)((int)(&uart_base_table)[param_1] + 0x40) = uVar2 - 1 & 0xffff;
  if (uVar2 == 0) {
    trap(7);
  }
  return (uVar1 >> 1) / uVar2;
}

