// psu_service @ 9d06311c
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Gniazdo 7, zasilacz buforowy (PSR-ECO ❓): 0xDD (4 B) → stan 230 V, usterki, napięcia,
   akumulator; zdarzenia 0x29, 0x2B. */

undefined4 psu_service(void)

{
  int iVar1;
  undefined4 uVar2;
  byte local_c [4];
  
  local_c[1] = 0;
  local_c[2] = 0;
  local_c[3] = 0;
  local_c[0] = (byte)((uint)(DAT_a0015a74 << 10) >> 0x1f);
  rnet_build_frame(&g_rnet_tx,DAT_a00159ed,0xdd,local_c,4);
  uart_rx_flush(0);
  iVar1 = rnet_send_frame(0,&g_rnet_tx);
  if (((iVar1 == 0) || (iVar1 = rnet_recv_frame(0,&g_rnet_rx), iVar1 == 0)) ||
     (g_rnet_rx._1_1_ != -0x23)) {
    uVar2 = 0;
  }
  else {
    DAT_a0001eb5 = g_rnet_rx._3_1_ & 1;
    DAT_a0001eb6 = g_rnet_rx._3_1_;
    DAT_a0001eb7 = (undefined1)DAT_a001700c;
    DAT_a0001eb8 = CONCAT11(DAT_a001700c._1_1_,DAT_a001700c._2_1_);
    DAT_a0001eba = CONCAT11(DAT_a001700c._3_1_,(undefined1)DAT_a0017010);
    DAT_a0001ebc = CONCAT11(DAT_a0017010._1_1_,DAT_a0017010._2_1_);
    DAT_a0001ebe = CONCAT11(DAT_a0017010._3_1_,(undefined1)DAT_a0017014);
    DAT_a0001ec0 = CONCAT11(DAT_a0017014._1_1_,DAT_a0017014._2_1_);
    DAT_a0001ec2 = CONCAT11(DAT_a0017014._3_1_,DAT_a0017018);
    DAT_a0001ec4 = DAT_a0017019;
    FUN_9d036f80(&DAT_9d06e660,DAT_a0001ebc);
    iVar1 = shared_lock(DAT_a00001b0);
    if (DAT_a000032c != '\x01') {
      if ((DAT_a0001eb6 & 0x38) == 0) {
        *(byte *)(iVar1 + 1) = *(byte *)(iVar1 + 1) & 0xfb;
      }
      else {
        *(byte *)(iVar1 + 1) = *(byte *)(iVar1 + 1) & 0xfb | 4;
      }
      if ((DAT_a0001eb5 == 0) && (DAT_a000008a != '\x01')) {
        DAT_a000008a = '\x01';
        g_sys_flags = g_sys_flags & 0xef | 0x10;
        DAT_a0000308 = 0;
        FUN_9d054ee4(4,1,500,500,500);
      }
      else if ((DAT_a0001eb5 == 1) && (DAT_a000008a != '\0')) {
        DAT_a000008a = '\0';
        g_sys_flags = g_sys_flags & 0xef;
        if ((*(byte *)(iVar1 + 2) & 0xf) >> 3 != 0) {
          event_log(0x29,0);
          FUN_9d01f758(&DAT_a001608c);
        }
        *(byte *)(iVar1 + 2) = *(byte *)(iVar1 + 2) & 0xf7;
        FUN_9d054d48(4);
      }
      if ((DAT_a000008a != '\0') && (DAT_a0015a78 < DAT_a0000308)) {
        if ((*(byte *)(iVar1 + 2) & 0xf) >> 3 != 1) {
          event_log(0x29,1);
          FUN_9d01f758(&DAT_a0016050);
        }
        *(byte *)(iVar1 + 2) = *(byte *)(iVar1 + 2) & 0xf7 | 8;
      }
      if ((DAT_a0001ec4 == '\x01') || (DAT_a0001ec4 == '\x02')) {
        if ((*(byte *)(iVar1 + 3) & 0xf) >> 3 != 1) {
          *(byte *)(iVar1 + 3) = *(byte *)(iVar1 + 3) & 0xf7 | 8;
          event_log(0x2b,1);
          FUN_9d01f758(&DAT_a00160d0);
        }
      }
      else if (((*(byte *)(iVar1 + 3) & 0xf) >> 3 != 0) && (DAT_a0001ec4 == '\0')) {
        *(byte *)(iVar1 + 3) = *(byte *)(iVar1 + 3) & 0xf7;
        event_log(0x2b,0);
        FUN_9d01f758(&DAT_a001610c);
      }
    }
    shared_unlock(DAT_a00001b0);
    uVar2 = 1;
  }
  return uVar2;
}

