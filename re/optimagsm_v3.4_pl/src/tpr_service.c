// tpr_service @ 9d01d264
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* Obsługa panelu TPR: odpytanie 0x2B, odbiór żądania panelu i odpowiedź (0x11 stan, 0x1C
   uzbrojenie kodem, …). */

undefined4 tpr_service(int param_1)

{
  byte bVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined1 uVar6;
  byte bVar7;
  char cVar8;
  char cVar9;
  int iVar10;
  uint uVar11;
  uint uVar12;
  uint uVar13;
  uint uVar14;
  char local_138;
  undefined4 local_128;
  undefined1 local_124 [4];
  undefined1 auStack_120 [4];
  undefined1 auStack_11c [4];
  undefined1 auStack_118 [4];
  undefined1 auStack_114 [4];
  undefined1 auStack_110 [4];
  undefined1 auStack_10c [4];
  undefined1 auStack_108 [128];
  undefined1 auStack_88 [32];
  undefined1 auStack_68 [48];
  byte local_38 [4];
  uint local_34;
  uint local_30 [2];
  undefined4 local_28;
  undefined4 local_24;
  undefined8 local_20;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10 [2];
  
  local_38[0] = 0;
  cVar9 = (&g_modules)[param_1 * 0xb];
  local_34 = param_1 - 1;
  uart_rx_flush(0);
  iVar10 = rnet_poll((int)cVar9);
  if (iVar10 == 0) {
    return 0;
  }
  iVar10 = rnet_recv_frame(0,&g_rnet_rx);
  uVar5 = DAT_a001701e;
  uVar4 = DAT_a001701a;
  uVar3 = ram0xa0017016;
  bVar7 = DAT_a001700c._2_1_;
  bVar1 = DAT_a001700c._1_1_;
  if (iVar10 == 0) {
    return 1;
  }
  switch(g_rnet_rx._1_1_) {
  case 0x11:
    memcpy(&DAT_a0001ec8 + local_34 * 0xc,0xa001700b,10);
    if ((&DAT_a00159a4)[param_1 * 0xb] != '\0') {
      FUN_9d00199c((&DAT_a00159a5)[param_1 * 0xb],1,&DAT_a0001ec8 + local_34 * 0xc);
      FUN_9d00199c((byte)(&DAT_a00159a5)[param_1 * 0xb] + 1,1,local_34 * 0xc + -0x5fffe136);
      if (*(ushort *)(&DAT_a0001ecc + local_34 * 0xc) < 0x352) {
        (&DAT_a00159a1)[param_1 * 0xb] = (&DAT_a00159a1)[param_1 * 0xb] & 0xdf | 0x20;
      }
      else if (0x366 < *(ushort *)(&DAT_a0001ecc + local_34 * 0xc)) {
        (&DAT_a00159a1)[param_1 * 0xb] = (&DAT_a00159a1)[param_1 * 0xb] & 0xdf;
      }
    }
    (&DAT_a00159a1)[param_1 * 0xb] =
         (&DAT_a00159a1)[param_1 * 0xb] & 0xfb | (((&DAT_a0001ece)[local_34 * 0xc] & 1) != 0) << 2;
    (&DAT_a00159a1)[param_1 * 0xb] =
         (&DAT_a00159a1)[param_1 * 0xb] & 0xbf | (((&DAT_a0001ed1)[local_34 * 0xc] & 2) != 0) << 6;
    FUN_9d037010(&DAT_9d06e03c,local_34,(&DAT_a0001ed1)[local_34 * 0xc] & 1);
    break;
  case 0x12:
    uVar6 = code_lookup(0xa001700d,(int)&local_128 + 1);
    local_128 = CONCAT22(local_128._2_2_,CONCAT11(local_128._1_1_,uVar6)) & 0xffff0fff;
    rnet_build_frame(&g_rnet_tx,cVar9,0x12,&local_128,2);
    rnet_send_frame(0,&g_rnet_tx);
    break;
  case 0x13:
    local_38[0] = FUN_9d05f258(g_rnet_rx._3_1_ + -0x5ffe8ff3,(int)(char)DAT_a001700c._1_1_,0,
                               0xa001700e);
    rnet_build_frame(&g_rnet_tx,cVar9,0x13,local_38,1);
    rnet_send_frame(0,&g_rnet_tx);
    event_log(0x62,0);
    break;
  case 0x14:
    uVar6 = FUN_9d05f4d8(0xa001700d,g_rnet_rx._3_1_ + -0x5ffe8ff3);
    local_128 = CONCAT31(local_128._1_3_,uVar6);
    rnet_build_frame(&g_rnet_tx,cVar9,0x14,&local_128,1);
    rnet_send_frame(0,&g_rnet_tx);
    event_log(100,0);
    break;
  case 0x15:
    memcpy(local_30,0xa001700d,4);
    local_38[0] = FUN_9d05f3bc(local_30[0]);
    rnet_build_frame(&g_rnet_tx,cVar9,0x15,local_38,1);
    rnet_send_frame(0,&g_rnet_tx);
    event_log(99,0);
    g_rnet_rx._3_1_ = '\x01';
    memcpy(&DAT_a001700c,local_30,4);
    rnet_queue_msg(0x30,0xa001700b);
    break;
  case 0x16:
    iVar10 = FUN_9d00530c(DAT_a001700c._2_1_);
    if (iVar10 == 0) {
      local_38[0] = 0xfe;
    }
    else if ((bVar1 == 1) && (cVar8 = FUN_9d01d0f0((int)(char)bVar7), cVar8 != '\x01')) {
      local_38[0] = 0x65;
    }
    else {
      local_38[0] = 1;
    }
    rnet_build_frame(&g_rnet_tx,cVar9,0x16,local_38,2);
    rnet_send_frame(0,&g_rnet_tx);
    if (local_38[0] == 1) {
      zones_arm(5,local_34,bVar7,bVar1,1,1);
    }
    break;
  case 0x18:
    memcpy(&local_28,0xa001700d,8);
    event_log(0xd,1);
    iVar10 = FUN_9d01ced4(local_28,local_24);
    if (iVar10 == 0) {
      local_128 = (uint)local_128._1_3_ << 8;
    }
    else {
      local_128 = CONCAT31(local_128._1_3_,1);
    }
    rnet_build_frame(&g_rnet_tx,cVar9,0x18,&local_128,1);
    rnet_send_frame(0,&g_rnet_tx);
    break;
  case 0x1c:
    bVar7 = *(byte *)((int)&DAT_a001700c + g_rnet_rx._3_1_ + 1) & 0xf;
    bVar1 = *(byte *)((int)&DAT_a001700c + g_rnet_rx._3_1_ + 1) >> 4;
    local_38[0] = code_lookup(0xa001700d,(int)&local_128 + 1);
    local_128 = local_128 & 0xffff0fff;
    iVar10 = FUN_9d00530c(bVar7);
    if (iVar10 != 0) {
      if ((bVar1 == 0) || (cVar8 = FUN_9d01d0f0(bVar7), cVar8 == '\x01')) {
        local_128 = CONCAT31(local_128._1_3_,local_38[0]);
      }
      else {
        local_128 = CONCAT31(local_128._1_3_,0x65);
      }
    }
    else {
      local_128 = CONCAT31(local_128._1_3_,0xfe);
    }
    rnet_build_frame(&g_rnet_tx,cVar9,0x1c,&local_128,2);
    rnet_send_frame(0,&g_rnet_tx);
    if (((local_38[0] < 100) && (iVar10 != 0)) && ((char)local_128 != 'e')) {
      DAT_a0000001 = local_38[0];
      zones_arm(3,local_38[0],bVar7,bVar1,1,1);
      FUN_9d036f80(&DAT_9d06e038,local_38[0]);
    }
    break;
  case 0x1d:
    bVar1 = *(byte *)((int)&DAT_a001700c + g_rnet_rx._3_1_ + 1);
    local_38[0] = code_lookup(0xa001700d,(int)&local_128 + 1);
    local_128 = CONCAT31(local_128._1_3_,local_38[0]);
    rnet_build_frame(&g_rnet_tx,cVar9,0x1d,&local_128,2);
    rnet_send_frame(0,&g_rnet_tx);
    if (local_38[0] < 100) {
      DAT_a0000001 = local_38[0];
      FUN_9d0049c4(3,local_38[0],bVar1 & 0xf,1);
      FUN_9d036f80(&DAT_9d06e038,local_38[0]);
    }
    break;
  case 0x1e:
    local_30[0] = FUN_9d052490();
    uVar11 = local_128;
    local_128._0_2_ = CONCAT11((char)local_30[0],1);
    local_128._3_1_ = SUB41(uVar11,3);
    local_128._0_3_ = CONCAT12((char)(local_30[0] >> 8),(undefined2)local_128);
    rnet_build_frame(&g_rnet_tx,cVar9,0x1e,&local_128,3);
    rnet_send_frame(0,&g_rnet_tx);
    break;
  case 0x20:
    local_30[0] = (uint)CONCAT11(DAT_a001700c._3_1_,(byte)DAT_a0017010);
    local_128 = CONCAT31(local_128._1_3_,1);
    rnet_build_frame(&g_rnet_tx,cVar9,0x20,&local_128,1);
    iVar10 = rnet_send_frame(0,&g_rnet_tx);
    if (iVar10 != 0) {
      local_138 = '\0';
      if (local_30[0] < bVar7) {
        local_30[0] = (uint)bVar7;
      }
      for (local_34 = local_30[0]; local_30[0] - bVar7 < local_34; local_34 = local_34 - 1) {
        FUN_9d0529f8(local_34,(int)&local_128 + 3);
        local_128._0_3_ = CONCAT12(local_138,0x101f);
        rnet_build_frame(&g_rnet_tx,cVar9,0x20,&local_128,0x13);
        rnet_send_frame(0,&g_rnet_tx);
        os_delay(100);
        local_138 = local_138 + '\x01';
      }
    }
    break;
  case 0x23:
    FUN_9d01d1e4((int)cVar9,0x23);
    switch(DAT_a001700c._1_1_) {
    case 1:
      rnet_send_multi(0,cVar9,1,&g_service_code4,5);
      break;
    case 5:
      memcpy(&local_128,&DAT_a0015538,4);
      memcpy(local_124,&DAT_a001553c,4);
      memcpy(auStack_120,&DAT_a0015658,4);
      memcpy(auStack_11c,&DAT_a001565c,4);
      memcpy(auStack_118,&DAT_a0015778,4);
      memcpy(auStack_114,&DAT_a001577c,4);
      memcpy(auStack_110,&DAT_a0015898,4);
      memcpy(auStack_10c,&DAT_a001589c,4);
      rnet_send_multi(0,cVar9,5,&local_128,0x20);
      break;
    case 6:
      memcpy(&local_128,&DAT_a0016798,0x68);
      rnet_send_multi(0,cVar9,6,&local_128,0x68);
      break;
    case 7:
      memcpy(&local_128,&DAT_a0016828,0x68);
      rnet_send_multi(0,cVar9,7,&local_128,0x68);
      break;
    case 8:
    case 9:
    case 10:
    case 0xb:
      local_34 = (uint)DAT_a001700c._1_1_;
      memcpy(&local_128,&DAT_a001526c + (local_34 - 8) * 0xa2,0xa2);
      rnet_send_multi(0,cVar9,local_34 & 0xff,&local_128,0xa2);
      break;
    case 0xc:
      memset(&local_128,0,0xe0);
      FUN_9d01ac90(&local_128);
      strcpy(auStack_88,&DAT_a00021ec);
      strcpy(auStack_68,&DAT_a000222b);
      rnet_send_multi(0,cVar9,0xc,&local_128,0xe0);
      break;
    case 0xd:
      iVar10 = FUN_9d019c24(0);
      if (iVar10 != 0) {
        local_128 = CONCAT31(local_128._1_3_,1);
        rnet_send_multi(0,cVar9,0xd,&local_128,1);
      }
    }
    break;
  case 0x24:
    local_128 = CONCAT31(local_128._1_3_,1);
    rnet_build_frame(&g_rnet_tx,cVar9,0x24,&local_128,1);
    iVar10 = rnet_send_frame(0,&g_rnet_tx);
    if (iVar10 == 0) {
      return 1;
    }
    cVar9 = rnet_read_multi(0,&local_128,&g_rnet_rx);
    switch(cVar9) {
    default:
      break;
    case '\x01':
      g_service_code4 = local_128;
      DAT_a0015d34 = local_124[0];
      FUN_9d04c190(&DAT_9d06e040,&g_cfg_comm);
      break;
    case '\x05':
      memcpy(&DAT_a0015538,&local_128,4);
      memcpy(&DAT_a001553c,local_124,4);
      memcpy(&DAT_a0015658,auStack_120,4);
      memcpy(&DAT_a001565c,auStack_11c,4);
      memcpy(&DAT_a0015778,auStack_118,4);
      memcpy(&DAT_a001577c,auStack_114,4);
      memcpy(&DAT_a0015898,auStack_110,4);
      memcpy(&DAT_a001589c,auStack_10c,4);
      FUN_9d04c190(&DAT_9d06e048,&DAT_a0015520);
      break;
    case '\x06':
      memcpy(&DAT_a0016798,&local_128,0x68);
      goto LAB_9d01ec50;
    case '\a':
      memcpy(&DAT_a0016828,&local_128,0x68);
LAB_9d01ec50:
      iVar10 = cfg_file_load(s_0_config_rpm_9d06b7a0);
      if (iVar10 != 0) {
        memcpy(&DAT_a0014b76,&DAT_a0016770,0x120);
        FUN_9d04cef4();
      }
      break;
    case '\b':
    case '\t':
    case '\n':
    case '\v':
      memcpy(&DAT_a001526c + (cVar9 + -8) * 0xa2,&local_128,0xa2);
      FUN_9d04c190(&DAT_9d06e050,&DAT_a001526c);
      break;
    case '\r':
      memset(&DAT_a000218e,0,0x20);
      strcpy(&DAT_a000218e,&local_128);
      memset(&DAT_a00021ae,0,0x20);
      strcpy(&DAT_a00021ae,auStack_108);
      iVar10 = FUN_9d019d64();
      if (iVar10 != 0) {
        FUN_9d04c190(&DAT_9d06e058,&DAT_a000218c);
      }
    }
    break;
  case 0x25:
    local_34 = (uint)DAT_a001700c._1_1_;
    if (DAT_a001700c._2_1_ == 0) {
      FUN_9d008838(local_34);
    }
    else {
      FUN_9d0087c4(local_34);
      *(undefined4 *)(&DAT_a0002950 + local_34 * 0x10) = 0;
    }
    local_34 = outputs_mask();
    local_128 = CONCAT31(local_128._1_3_,1);
    memcpy((int)&local_128 + 1,&local_34,4);
    rnet_build_frame(&g_rnet_tx,cVar9,0x25,&local_128,5);
    rnet_send_frame(0,&g_rnet_tx);
    break;
  case 0x27:
    local_18 = CONCAT13((byte)DAT_a0017010,
                        CONCAT12(DAT_a001700c._3_1_,CONCAT11(DAT_a001700c._2_1_,DAT_a001700c._1_1_))
                       );
    local_14 = ram0xa0017011;
    cVar9 = FUN_9d02775c(DAT_a001700c._1_1_,DAT_a001700c._2_1_,DAT_a001700c._3_1_);
    local_14 = CONCAT13(cVar9 + '\x01',(undefined3)local_14);
    FUN_9d041f64(&local_18);
    event_log(0x28,0);
    break;
  case 0x29:
    if (DAT_a001700c._1_1_ == 0) {
      _g_sys_flags = _g_sys_flags & 0xfffe;
    }
    else {
      _g_sys_flags = _g_sys_flags & 0xfffe | 1;
      FUN_9d004c2c(0);
      FUN_9d004c2c(1);
      FUN_9d004c2c(2);
      FUN_9d004c2c(3);
    }
    event_log(0x1c,_g_sys_flags & 1);
    break;
  case 0x30:
    local_30[0] = (uint)DAT_a001700c._1_1_;
    if (local_30[0] < 0x20) {
      iVar10 = local_30[0] * 0x22;
      uVar2 = CONCAT13(DAT_a0017014._1_1_,DAT_a0017010_2);
      *(uint *)((int)&DAT_a00162d0 + iVar10) =
           CONCAT13(DAT_a0017010._1_1_,
                    CONCAT12((byte)DAT_a0017010,CONCAT11(DAT_a001700c._3_1_,DAT_a001700c._2_1_)));
      *(undefined4 *)(&DAT_a00162d4 + local_30[0] * 0x11) = uVar2;
      *(undefined4 *)(&DAT_a00162d8 + iVar10) = uVar3;
      *(undefined4 *)(&DAT_a00162dc + iVar10) = uVar4;
      *(undefined4 *)(&DAT_a00162e0 + iVar10) = uVar5;
      FUN_9d05efd4(&DAT_a00162d0);
      os_delay(5000);
      g_rnet_rx._3_1_ = '\0';
      rnet_queue_msg(0x30,0xa001700b);
    }
    break;
  case 0x31:
    if ((DAT_a001700c._1_1_ < 2) && (DAT_a001700c._2_1_ < 6)) {
      FUN_9d034d70(DAT_a001700c._1_1_,DAT_a001700c._2_1_);
    }
    break;
  case 0x32:
    if (DAT_a001700c._1_1_ < 2) {
      memcpy(local_10,0xa001700e,4);
      FUN_9d034f0c(DAT_a001700c._1_1_,local_10[0]);
    }
    break;
  case 0x33:
    local_34 = (uint)DAT_a001700c._1_1_;
    if (local_34 < 2) {
      local_128 = CONCAT31(local_128._1_3_,1);
      memcpy((int)&local_128 + 1,(int)&DAT_a0016710 + local_34 * 0x2f,0x2f);
      rnet_build_frame(&g_rnet_tx,cVar9,0x33,&local_128,0x30);
      rnet_send_frame(0,&g_rnet_tx);
    }
    break;
  case 0x34:
    FUN_9d01d1e4((int)cVar9,0x34);
    FUN_9d02f538(0xa001700d);
    break;
  case 0x35:
    FUN_9d01d1e4((int)cVar9,0x35);
    FUN_9d02109c(0xa001700d,0);
    break;
  case 0x36:
    FUN_9d01d1e4((int)cVar9,0x36);
    FUN_9d01b014();
    break;
  case 0x37:
    FUN_9d01d1e4((int)cVar9,0x37);
    FUN_9d030194();
    break;
  case 0x38:
    local_128 = (uint)local_128._1_3_ << 8;
    local_20 = FUN_9d0038b0((int)(char)DAT_a001700c._1_1_);
    memcpy((int)&local_128 + 1,&local_20,8);
    rnet_build_frame(&g_rnet_tx,cVar9,0x38,&local_128,9);
    rnet_send_frame(0,&g_rnet_tx);
    break;
  case 0x39:
    FUN_9d01d1e4((int)cVar9,0x39);
    os_delay(500);
    local_128 = CONCAT31(local_128._1_3_,3);
    FUN_9d03226c(&local_128,0x1a);
    DAT_a0000098 = 2;
    break;
  case 0x3a:
    FUN_9d01d1e4((int)cVar9,0x3a);
    os_delay(500);
    local_128 = CONCAT31(local_128._1_3_,2);
    FUN_9d03226c(&local_128,0x1a);
    DAT_a0000098 = 1;
    break;
  case 0x3d:
    uVar11 = (uint)(byte)DAT_a0017010;
    uVar12 = (uint)DAT_a001700c._3_1_;
    uVar13 = (uint)DAT_a001700c._2_1_;
    uVar14 = (uint)DAT_a001700c._1_1_;
    for (local_34 = 0; local_34 < 0x20; local_34 = local_34 + 1) {
      if ((1 << (local_34 & 0x1f) & (uVar11 << 0x18 | uVar12 << 0x10 | uVar13 << 8 | uVar14)) != 0)
      {
        FUN_9d0087c4(local_34);
        *(undefined4 *)(&DAT_a0002950 + local_34 * 0x10) = 0;
      }
    }
    break;
  case 0x3e:
    uVar11 = (uint)(byte)DAT_a0017010;
    uVar12 = (uint)DAT_a001700c._3_1_;
    uVar13 = (uint)DAT_a001700c._2_1_;
    uVar14 = (uint)DAT_a001700c._1_1_;
    for (local_34 = 0; local_34 < 0x20; local_34 = local_34 + 1) {
      if ((1 << (local_34 & 0x1f) & (uVar11 << 0x18 | uVar12 << 0x10 | uVar13 << 8 | uVar14)) != 0)
      {
        FUN_9d008838(local_34);
      }
    }
    break;
  case 0x3f:
    FUN_9d01d1e4((int)cVar9,0x36);
    os_delay(10000);
    svc_end_session();
    break;
  case 0x40:
    FUN_9d01d1e4((int)cVar9,0x40);
    os_delay(500);
    local_128._0_2_ = CONCAT11(DAT_a001700c._2_1_,DAT_a001700c._1_1_);
    FUN_9d03226c(&local_128,0x1a);
    if ((char)local_128 == '\t') {
      os_delay(10000);
    }
  }
  return 1;
}

