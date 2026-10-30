// hub_cmd_f7 @ 9d04fa08
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Ramka 0xB0/0xF7, bez danych. */

undefined4 hub_cmd_f7(undefined4 param_1)

{
  int iVar1;
  undefined4 uVar2;
  
  g_rnet_tx = 0xb0;
  DAT_a0016f11 = 0xf7;
  DAT_a0016f12 = 0;
  iVar1 = rnet_send_frame(0,&g_rnet_tx);
  if (((iVar1 == 0) || (iVar1 = rnet_recv_frame(0,&g_rnet_rx), iVar1 == 0)) ||
     (g_rnet_rx._1_1_ != -9)) {
    uVar2 = 0;
  }
  else {
    memcpy(param_1,0xa001700b,6);
    uVar2 = 1;
  }
  return uVar2;
}

