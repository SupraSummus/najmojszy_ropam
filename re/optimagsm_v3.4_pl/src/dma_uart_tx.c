// dma_uart_tx @ 9d023060
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Wysyłka bufora kanałem DMA 0 na UART2 (1), UART4 (2) albo UART5 (3) z oczekiwaniem. */

bool dma_uart_tx(int param_1,undefined4 param_2,undefined4 param_3,int param_4)

{
  bool bVar1;
  int iVar2;
  int local_10;
  
  local_10 = 0;
  FUN_9d06b898(0,1,0);
  if (param_1 == 3) {
    FUN_9d06df00(0,0x4b10);
    FUN_9d06c700(0,param_2,&DAT_bf806a20,param_3,1,1);
  }
  else if (param_1 == 2) {
    FUN_9d06df00(0,0x4510);
    FUN_9d06c700(0,param_2,&DAT_bf806220,param_3,1,1);
  }
  else if (param_1 == 1) {
    FUN_9d06df00(0,0x2a10);
    FUN_9d06c700(0,param_2,&DAT_bf806820,param_3,1,1);
  }
  FUN_9d06e0b0(0,8);
  FUN_9d06dca4(0x61,0);
  FUN_9d06dae4(0x2d,5);
  FUN_9d06db24(0x2d,3);
  FUN_9d06dca4(0x61,1);
  DAT_a00000a8 = 0;
  FUN_9d06e0d8(0);
  FUN_9d06ade4(0,0,0);
  while ((DAT_a00000a8 == 0 &&
         (iVar2 = local_10 + 1, bVar1 = local_10 < param_4 / 10, local_10 = iVar2, bVar1))) {
    os_delay(100);
    FUN_9d064c44(DAT_a0000368,1,2);
  }
  FUN_9d06ddcc(0);
  FUN_9d06dca4(0x61,0);
  return local_10 < param_4 / 10;
}

