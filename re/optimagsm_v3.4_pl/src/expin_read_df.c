// expin_read_df @ 9d030c08
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Ekspander wejść: 0xDF (8 B) → pełny blok stanu modułu (do 0x76 B). */

undefined4 expin_read_df(int param_1,undefined4 param_2)

{
  int iVar1;
  undefined4 uVar2;
  
  rnet_build_frame(&g_rnet_tx,DAT_a00159e2,0xdf,param_2,8);
  uart_rx_flush(0);
  iVar1 = rnet_send_frame(0,&g_rnet_tx);
  if (((iVar1 == 0) || (iVar1 = rnet_recv_frame(0,&g_rnet_rx), iVar1 == 0)) ||
     (g_rnet_rx._1_1_ != -0x21)) {
    uVar2 = 0;
  }
  else {
    *(undefined1 *)(param_1 + 0x75) = 0x66;
    memcpy(param_1,0xa001700b,g_rnet_rx._2_1_);
    uVar2 = 1;
  }
  return uVar2;
}

