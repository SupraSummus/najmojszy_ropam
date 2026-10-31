// expi_service @ 9d066c0c
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Gniazda 11, 12, ekspander wejść EXP-I8-RN: 0xDD (1 B) → 8 × u16 wartości linii i bajt
   statusu. */

undefined4 expi_service(byte param_1)

{
  int iVar1;
  undefined4 uVar2;
  undefined1 local_20 [4];
  undefined1 auStack_1c [20];
  
  local_20[0] = 0;
  rnet_build_frame(&g_rnet_tx,(&g_modules)[(uint)param_1 * 0xb],0xdd,local_20,1);
  uart_rx_flush(0);
  iVar1 = rnet_send_frame(0,&g_rnet_tx);
  if ((iVar1 == 0) || (iVar1 = rnet_recv_frame(0,&g_rnet_rx), iVar1 == 0)) {
    uVar2 = 0;
  }
  else {
    if (g_rnet_rx._1_1_ == -0x23) {
      memcpy(auStack_1c,0xa001700b,0x10);
      if ((uint)(byte)(&DAT_a00159a5)[(uint)param_1 * 0xb] +
          (uint)(byte)(&DAT_a00159a4)[(uint)param_1 * 0xb] < 0x30) {
        inputs_set_lines((&DAT_a00159a5)[(uint)param_1 * 0xb],(&DAT_a00159a4)[(uint)param_1 * 0xb],
                         auStack_1c);
      }
      (&DAT_a00159a1)[(uint)param_1 * 0xb] =
           (&DAT_a00159a1)[(uint)param_1 * 0xb] & 0xfb | ((DAT_a001701a._1_1_ & 1) != 0) << 2;
      (&DAT_a00159a1)[(uint)param_1 * 0xb] =
           (&DAT_a00159a1)[(uint)param_1 * 0xb] & 0xdf | ((DAT_a001701a._1_1_ & 4) != 0) << 5;
      (&DAT_a00159a1)[(uint)param_1 * 0xb] =
           (&DAT_a00159a1)[(uint)param_1 * 0xb] & 0xef | ((DAT_a001701a._1_1_ & 8) != 0) << 4;
      (&DAT_a00159a1)[(uint)param_1 * 0xb] =
           (&DAT_a00159a1)[(uint)param_1 * 0xb] & 0xbf | ((DAT_a001701a._1_1_ & 0x10) != 0) << 6;
      (&DAT_a00159a8)[(uint)param_1 * 0xb] = DAT_a001701a._1_1_;
    }
    uVar2 = 1;
  }
  return uVar2;
}

