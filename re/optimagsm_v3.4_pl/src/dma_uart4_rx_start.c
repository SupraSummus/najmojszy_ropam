// dma_uart4_rx_start @ 9d01b1ec
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Odbiór kanałem DMA 1 z U4RXREG. */

void dma_uart4_rx_start(undefined4 param_1,undefined4 param_2)

{
  FUN_9d06b898(1,2,0);
  FUN_9d06df00(1,0x4410);
  FUN_9d06c700(1,&DAT_bf806230,param_1,1,param_2,1);
  FUN_9d06e0b0(1,8);
  FUN_9d06dca4(0x62,0);
  FUN_9d06dae4(0x2e,1);
  FUN_9d06dca4(0x62,1);
  DAT_a00000a4 = 0;
  FUN_9d06e0d8(1);
  return;
}

