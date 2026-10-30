// hub_cmd_f1 @ 9d04f668
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Ramka 0xB0/0xF1, 8 B stanu; odpowiedź 0xF1, 84 B. */

undefined4 hub_cmd_f1(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  undefined4 uVar2;
  
  g_rnet_tx = 0xb0;
  DAT_a0016f11 = 0xf1;
  DAT_a0016f12 = 8;
  memcpy(&DAT_a0016f13,param_1,8);
  uart_rx_flush(0);
  iVar1 = rnet_send_frame(0,&g_rnet_tx);
  if (((iVar1 == 0) || (iVar1 = rnet_recv_frame(0,&g_rnet_rx), iVar1 == 0)) ||
     (g_rnet_rx._1_1_ != -0xf)) {
    uVar2 = 0;
  }
  else {
    memcpy(param_2,0xa001700b,0x54);
    uVar2 = 1;
  }
  return uVar2;
}

