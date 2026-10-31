// expo_service @ 9d0635f0
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Gniazda 13, 14, ekspander wyjść EXP-O8x-RN ❓. */

undefined4 expo_service(byte param_1)

{
  bool bVar1;
  byte bVar2;
  int iVar3;
  undefined4 uVar4;
  byte local_18;
  char local_17;
  byte local_10 [8];
  
  local_10[0] = 0;
  local_10[1] = 0;
  local_10[2] = 0;
  local_10[3] = 0;
  for (local_18 = 0; local_18 < 8; local_18 = local_18 + 1) {
    bVar2 = (&DAT_a00159a7)[(uint)param_1 * 0xb];
    local_17 = output_is_on((uint)bVar2 + (uint)local_18);
    if ((*(uint *)(&DAT_a0006914 + ((uint)bVar2 + (uint)local_18) * 0xe0) & 0x3ff) >> 8 == 1) {
      local_17 = local_17 == '\0';
    }
    local_10[0] = local_10[0] | local_17 << (local_18 & 0x1f);
  }
  rnet_build_frame(&g_rnet_tx,(&g_modules)[(uint)param_1 * 0xb],0x25,local_10,1);
  uart_rx_flush(0);
  iVar3 = rnet_send_frame(0,&g_rnet_tx);
  if ((iVar3 == 0) || (iVar3 = rnet_recv_frame(0,&g_rnet_rx), iVar3 == 0)) {
    uVar4 = 0;
  }
  else {
    if (g_rnet_rx._1_1_ == -0x23) {
      (&DAT_a00159a1)[(uint)param_1 * 0xb] =
           (&DAT_a00159a1)[(uint)param_1 * 0xb] & 0xfb | ((g_rnet_rx._3_1_ & 1) != 0) << 2;
      (&DAT_a00159a1)[(uint)param_1 * 0xb] =
           (&DAT_a00159a1)[(uint)param_1 * 0xb] & 0xef | ((g_rnet_rx._3_1_ & 8) != 0) << 4;
      (&DAT_a00159a8)[(uint)param_1 * 0xb] = g_rnet_rx._3_1_ & 8;
    }
    bVar1 = param_1 != 0xd;
    if ((((byte)(&DAT_a00159a1)[(uint)param_1 * 0xb] & 0x1f) >> 4 == 0) ||
       ((&DAT_a00000c4)[bVar1] == '\x01')) {
      if ((((byte)(&DAT_a00159a1)[(uint)param_1 * 0xb] & 0x1f) >> 4 != 1) &&
         ((&DAT_a00000c4)[bVar1] != '\0')) {
        event_log_ex(0x81,0,param_1);
      }
    }
    else {
      event_log_ex(0x81,1,param_1);
    }
    (&DAT_a00000c4)[bVar1] =
         (byte)(((uint)(byte)(&DAT_a00159a1)[(uint)param_1 * 0xb] << 0x1b) >> 0x1f);
    uVar4 = 1;
  }
  return uVar4;
}

