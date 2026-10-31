// fgr4_channel_cmd @ 9d0249b4
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Port 1, moduł FGR-4 (adres 0xEE): cmd 2 z bitem kanału i czasem, cmd 1 odczyt; komendy
   serwisowe 0x26–0x29. */

void fgr4_channel_cmd(byte param_1)

{
  char cVar1;
  int iVar2;
  int local_30;
  uint local_28 [4];
  undefined1 local_18;
  undefined1 local_17;
  undefined1 local_16;
  undefined1 local_15;
  undefined1 local_14;
  undefined1 local_13;
  undefined1 local_10;
  undefined1 local_f;
  undefined1 local_e;
  undefined1 local_d;
  undefined1 local_c;
  
  rtc_get(&local_10);
  local_18 = (undefined1)(1 << (param_1 & 0x1f));
  local_17 = local_10;
  local_16 = local_f;
  local_15 = local_e;
  local_14 = local_d;
  local_13 = local_c;
  rnet_build_frame(&g_rnet_tx,0xee,2,&local_18,6);
  iVar2 = exp_send_wake(&g_rnet_tx);
  if (iVar2 != 0) {
    local_30 = 0;
    do {
      if (99 < local_30) break;
      cVar1 = rnet_recv_frame(1,&g_rnet_rx);
      local_30 = local_30 + 1;
    } while (cVar1 != '\x01');
    os_delay(100);
    rnet_build_frame(&g_rnet_tx,0xee,1,&local_18,0);
    iVar2 = exp_send_wake(&g_rnet_tx);
    if ((iVar2 != 0) && (iVar2 = rnet_recv_frame(1,&g_rnet_rx), iVar2 != 0)) {
      if (g_rnet_rx._2_1_ < 9) {
        local_28[0] = (uint)CONCAT11((byte)DAT_a001700c,g_rnet_rx._3_1_);
        local_28[1] = (uint)CONCAT11(DAT_a001700c._2_1_,DAT_a001700c._1_1_);
        local_28[2] = (uint)CONCAT11((byte)DAT_a0017010,DAT_a001700c._3_1_);
        local_28[3] = (uint)CONCAT11(DAT_a0017010._2_1_,DAT_a0017010._1_1_);
      }
      else {
        local_28[0] = (uint)DAT_a0017010._3_1_ << 0x10 | (uint)(byte)DAT_a001700c << 8 |
                      (uint)g_rnet_rx._3_1_;
        local_28[1] = (uint)(byte)DAT_a0017014 << 0x10 | (uint)DAT_a001700c._2_1_ << 8 |
                      (uint)DAT_a001700c._1_1_;
        local_28[2] = (uint)DAT_a0017014._1_1_ << 0x10 | (uint)(byte)DAT_a0017010 << 8 |
                      (uint)DAT_a001700c._3_1_;
        local_28[3] = (uint)DAT_a0017014._2_1_ << 0x10 | (uint)DAT_a0017010._2_1_ << 8 |
                      (uint)DAT_a0017010._1_1_;
      }
      FUN_9d050a30(2,local_28 + param_1,4);
      if (local_28[param_1] != 0) {
        os_delay(100);
        FUN_9d023660(2,(int)(char)param_1,local_28[param_1]);
      }
    }
  }
  return;
}

