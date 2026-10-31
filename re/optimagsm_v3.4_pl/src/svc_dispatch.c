// svc_dispatch @ 9d03c3f0
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* Odbiera pakiet komendy i wykonuje komendę wg pierwszego bajtu. */

undefined4 svc_dispatch(void)

{
  undefined1 uVar1;
  short sVar2;
  ushort uVar3;
  int iVar4;
  uint uVar5;
  undefined4 uVar6;
  int iVar7;
  int local_338;
  ushort local_334;
  bool local_332;
  byte local_318 [4];
  undefined4 local_314;
  undefined1 local_310;
  undefined1 uStack_30f;
  undefined2 uStack_30e;
  undefined1 auStack_14c [52];
  undefined4 local_118;
  undefined4 local_114;
  undefined1 auStack_110 [26];
  undefined1 auStack_f6 [162];
  undefined1 auStack_54 [24];
  undefined1 local_3c;
  undefined4 local_10;
  undefined4 local_c;
  
  local_10 = 0;
  local_c = 0;
  DAT_a001fff0 = 0;
  iVar4 = svc_receive(local_318,0xff);
  if (iVar4 < 1) {
    return 0;
  }
  local_10 = FUN_9d059e90(&DAT_9d06d950);
  iVar4 = svc_dispatch_ext(local_318,&g_svc_buf);
  if (-1 < iVar4) {
    DAT_a00000e7 = 1;
    if (iVar4 != 0) {
      xmodem_transmit(&g_svc_buf,iVar4);
    }
    if (DAT_a00000e6 != '\0') {
      svc_stream_to_modem(3);
      DAT_a00000e6 = '\0';
    }
    return 0;
  }
  switch(local_318[0]) {
  case 1:
    local_318[0] = 0;
    local_318[1] = 0x22;
    local_318[2] = (byte)g_boot_version;
    local_318[3] = 0x14;
    local_314 = g_panel_id;
    local_310 = (undefined1)DAT_a0000300;
    uStack_30f = (undefined1)((uint)DAT_a0000300 >> 8);
    uStack_30e = (undefined2)((uint)DAT_a0000300 >> 0x10);
    svc_transmit(local_318,0xc);
    if (((DAT_a000277c._1_1_ & 0x7f) >> 6 == 0) && ((DAT_a000277e & 0x3f) >> 5 == 0)) {
      event_log(0x1b,0);
    }
    else {
      event_log(0x1b,2);
    }
    FUN_9d05e3e4(DAT_a0000118,auStack_54,0);
    local_3c = 0;
    FUN_9d05e5c0(DAT_a0000118,auStack_54);
    break;
  case 2:
    if (CONCAT11(local_318[2],local_318[1]) == CONFIG_SIZE) {
      FUN_9d054ee4(1,0xe8,0x32,0x32,500);
      local_334 = svc_receive(&g_config,CONFIG_SIZE);
LAB_9d03c8d4:
      sVar2 = crc16_xmodem(&g_config,g_config + -4);
      if ((local_334 == CONFIG_SIZE) && (sVar2 == g_cfg_crc)) {
        iVar4 = cfg_file_save(s_0_config_rpm_9d06b7a0);
        if (iVar4 == 0) {
          local_332 = false;
        }
        else {
          iVar4 = cfg_file_save(s_0_cfgcpy_rpm_9d06b7b0);
          local_332 = iVar4 != 0;
        }
        if ((local_332 != false) && (iVar4 = cfg_file_load(s_0_config_rpm_9d06b7a0), iVar4 != 0)) {
          iVar4 = shared_lock(DAT_a00001b0);
          *(byte *)(iVar4 + 3) = *(byte *)(iVar4 + 3) & 0xef;
          shared_unlock(DAT_a00001b0);
          cfg_load_all();
          inputs_from_config();
          FUN_9d009454();
          DAT_a0000088 = 1;
          if (DAT_a0000264 != '\0') {
            uVar6 = FUN_9d059e90(&DAT_9d06d958);
            FUN_9d059cfc(uVar6);
          }
        }
        if ((local_332 != false) && (iVar4 = svc_send_status(local_332), 0 < iVar4)) {
          if (((DAT_a00159ac & 1) != 0) ||
             ((((DAT_a00159b7 & 1) != 0 || ((DAT_a00159c2 & 1) != 0)) || ((DAT_a00159cd & 1) != 0)))
             ) {
            local_c = FUN_9d05d918(1);
            FUN_9d05d770(local_c,local_10);
            local_c = FUN_9d05dbf8();
            uVar6 = FUN_9d05db64(local_c);
            FUN_9d05d9dc(local_c);
            svc_send_status(uVar6);
          }
          if ((DAT_a00159f9 & 1) != 0) {
            local_c = FUN_9d05d918(7);
            FUN_9d05d770(local_c,local_10);
            local_c = FUN_9d05dbf8();
            uVar6 = FUN_9d05db64(local_c);
            FUN_9d05d9dc(local_c);
            svc_send_status(uVar6);
          }
        }
      }
      else {
        os_delay(10000);
        svc_send_status(0);
      }
    }
    else {
      svc_send_status(0);
    }
    FUN_9d054e80(1);
    break;
  case 3:
    iVar4 = svc_check_code(local_318 + 1);
    if (iVar4 == 0) {
      if ((DAT_a000277c._1_1_ & 0x7f) >> 6 != 1) {
        svc_send_status(2);
      }
    }
    else {
      FUN_9d054ee4(1,0xe8,0x32,0x32,500);
      iVar4 = cfg_file_load(s_0_config_rpm_9d06b7a0);
      if ((DAT_a000277c._1_1_ & 0x7f) >> 6 != 1) {
        svc_send_status(iVar4);
      }
      if (iVar4 != 0) {
        svc_transmit(&g_config,CONFIG_SIZE);
      }
    }
    FUN_9d054e80(1);
    break;
  case 4:
    FUN_9d054ee4(1,0xe8,100,100,0x32);
    if ((DAT_a000277c._1_1_ & 0x7f) >> 6 == 0) {
      log_read_cmd(local_318[1]);
    }
    else {
      FUN_9d05290c();
    }
    break;
  case 5:
    log_clear();
    svc_send_status(1);
    outputs_selftest();
    break;
  case 7:
    local_c = FUN_9d05d918(4);
    FUN_9d05d770(local_c,local_10);
    local_c = FUN_9d05dbf8();
    iVar4 = FUN_9d05db64(local_c);
    FUN_9d05d9dc(local_c);
    if (iVar4 == 0x30) {
      svc_transmit(&g_svc_buf,0xc6);
    }
    break;
  case 8:
    local_c = FUN_9d05d918(0xe);
    FUN_9d05d770(local_c,local_10);
    local_c = FUN_9d05dbf8();
    iVar4 = FUN_9d05db64(local_c);
    FUN_9d05d9dc(local_c);
    if (iVar4 == 0x30) {
      svc_transmit(&g_modules,0xc6);
    }
    break;
  case 0xd:
    local_118 = CONCAT13((undefined1)local_314,
                         CONCAT12(local_318[3],CONCAT11(local_318[2],local_318[1])));
    local_114 = CONCAT13(local_310,local_314._1_3_);
    rtc_set(&local_118);
    svc_send_status(1);
    break;
  case 0xf:
    if (((((byte)DAT_a000277c & 3) >> 1 == 0) || ((DAT_a000277c._1_1_ & 0x3f) >> 5 == 0)) ||
       (DAT_a0015d06 == '\0')) {
      local_332 = false;
    }
    else {
      DAT_a0001acf = 1;
      strcpy(&DAT_a0001ad0,local_318 + 1);
      FUN_9d01f758(&DAT_a0001acc);
      os_delay(30000);
      while (DAT_a0002781 == '\x01') {
        os_delay(100);
      }
      if ((DAT_a0002781 == '\x02') || (DAT_a0002781 == '\0')) {
        local_332 = true;
      }
      else {
        local_332 = false;
      }
    }
    svc_send_status(local_332);
    break;
  case 0x10:
    local_318[0] = (byte)DAT_9d06d968;
    local_318[1] = (byte)((uint)DAT_9d06d968 >> 8);
    local_318[2] = (byte)((uint)DAT_9d06d968 >> 0x10);
    local_318[3] = (byte)((uint)DAT_9d06d968 >> 0x18);
    local_314 = DAT_9d06d96c;
    local_310 = (undefined1)DAT_9d06d970;
    uStack_30f = (undefined1)((uint)DAT_9d06d970 >> 8);
    uStack_30e = (undefined2)((uint)DAT_9d06d970 >> 0x10);
    iVar4 = FUN_9d02ff34(local_318,&DAT_9d066238,auStack_14c,1);
    if (iVar4 != 0) {
      DAT_a000277e = DAT_a000277e & 0xfe | 1;
    }
    svc_send_status(iVar4 != 0);
    break;
  case 0x11:
    local_318[0] = (byte)DAT_9d06d974;
    local_318[1] = (byte)((uint)DAT_9d06d974 >> 8);
    local_318[2] = (byte)((uint)DAT_9d06d974 >> 0x10);
    local_318[3] = (byte)((uint)DAT_9d06d974 >> 0x18);
    local_314 = DAT_9d06d978;
    local_310 = (undefined1)DAT_9d06d97c;
    uStack_30f = (undefined1)((ushort)DAT_9d06d97c >> 8);
    iVar4 = FUN_9d02ff34(local_318,&DAT_9d066238,auStack_14c,1);
    if (iVar4 != 0) {
      DAT_a000277e = DAT_a000277e & 0xfe;
    }
    svc_send_status(iVar4 != 0);
    break;
  case 0x12:
    if (((byte)DAT_a000277c & 0xf) >> 3 == 1) {
      local_332 = false;
    }
    else {
      sprintf(local_318,s_AT_CPIN___s__9d06d980,&DAT_a0015a68);
      local_332 = (bool)FUN_9d02ff34(local_318,&DAT_9d066238,auStack_14c,2);
    }
    svc_send_status(local_332);
    break;
  case 0x13:
    if (((byte)DAT_a000277c & 3) >> 1 == 0) {
      local_332 = false;
    }
    else {
      iVar4 = FUN_9d02109c(local_318 + 1,1);
      local_332 = iVar4 != 0;
    }
    svc_send_status(local_332);
    break;
  case 0x15:
    local_c = FUN_9d05d918(2);
    thunk_FUN_9d05d668(local_c,local_318 + 1,0xc);
    FUN_9d05d770(local_c,local_10);
    local_c = FUN_9d05dbf8();
    uVar6 = FUN_9d05db64(local_c);
    FUN_9d05d9dc(local_c);
    svc_send_status(uVar6);
    break;
  case 0x18:
    local_c = FUN_9d05d918(3);
    thunk_FUN_9d05d668(local_c,local_318 + 1,0x1a);
    FUN_9d05d770(local_c,local_10);
    local_c = FUN_9d05dbf8();
    uVar6 = FUN_9d05db64(local_c);
    FUN_9d05d9dc(local_c);
    svc_send_status(uVar6);
    break;
  case 0x19:
    DAT_a0000135 = 1;
    iVar4 = sms_execute(local_318 + 1);
    if (iVar4 != 0) {
      g_svc_buf = 0;
      for (local_338 = 0; local_338 < 3; local_338 = local_338 + 1) {
        FUN_9d05a028(10);
        iVar4 = rb_read(DAT_a00002e4,auStack_110,1);
        FUN_9d05a090();
        if (iVar4 != 0) {
          FUN_9d06daa0(&g_svc_buf,auStack_f6);
        }
      }
      iVar4 = FUN_9d06a74c(&g_svc_buf);
      svc_transmit(&g_svc_buf,iVar4 + 1);
    }
    DAT_a0000135 = 0;
    break;
  case 0x1e:
    iVar4 = FUN_9d05f67c();
    if (iVar4 == 0) {
      svc_send_status(2);
    }
    else {
      svc_send_status(1);
    }
    if (((((DAT_a00159ac & 1) != 0) || ((DAT_a00159b7 & 1) != 0)) || ((DAT_a00159c2 & 1) != 0)) ||
       ((DAT_a00159cd & 1) != 0)) {
      local_c = FUN_9d05d918(0xb);
      local_318[0] = 4;
      local_318[1] = 0xff;
      local_318[2] = 0xff;
      local_318[3] = 0xff;
      local_314 = CONCAT31(local_314._1_3_,0xff);
      thunk_FUN_9d05d668(local_c,local_318,0x20);
      FUN_9d05d770(local_c,local_10);
      local_c = FUN_9d05dbf8();
      FUN_9d05d9dc(local_c);
    }
    break;
  case 0x1f:
    DAT_a001fff0 = 1;
    sys_restart();
    break;
  case 0x21:
    FUN_9d061674();
    break;
  case 0x24:
    uVar3 = status_header(0,&g_svc_buf);
    FUN_9d0017a0();
    for (local_338 = 0; local_338 < 0x30; local_338 = local_338 + 1) {
      memcpy(&g_svc_buf + (uint)uVar3 + local_338 * 2,&g_inputs_rt + local_338 * 10,2);
    }
    memcpy(&g_svc_buf + (ushort)(uVar3 + 0x60),&g_modules,0xc6);
    memcpy(&g_svc_buf + (ushort)(uVar3 + 0x126),&DAT_a0000280,DAT_a0015a16);
    uVar3 = (ushort)DAT_a0015a16 + uVar3 + 0x126;
    memcpy(&g_svc_buf + uVar3,&DAT_a0001eb4,0x12);
    uVar3 = uVar3 + 0x12;
    memcpy(&g_svc_buf + uVar3,&DAT_a0001284,DAT_a00159ea);
    uVar3 = DAT_a00159ea + uVar3;
    memcpy(&g_svc_buf + uVar3,&DAT_a0001ec8,0x30);
    uVar3 = uVar3 + 0x30;
    memcpy(&g_svc_buf + uVar3,&DAT_a0001ef8,DAT_a0015a0b);
    uVar3 = DAT_a0015a0b + uVar3;
    memcpy(&g_svc_buf + uVar3,&DAT_a000029a,2);
    memcpy(&g_svc_buf + (ushort)(uVar3 + 2),&DAT_a0001234,0x40);
    iVar4 = svc_transmit(&g_svc_buf,0x44c);
    if ((0 < iVar4) && ((DAT_a000277e & 1) != 0)) {
      local_318[0] = (byte)DAT_9d06d95c;
      local_318[1] = (byte)((uint)DAT_9d06d95c >> 8);
      local_318[2] = (byte)((uint)DAT_9d06d95c >> 0x10);
      local_318[3] = (byte)((uint)DAT_9d06d95c >> 0x18);
      local_314 = DAT_9d06d960;
      local_310 = DAT_9d06d964;
      iVar4 = FUN_9d02ff34(local_318,&DAT_9d066238,auStack_14c,0);
      if (iVar4 != 0) {
        os_delay(5000);
        iVar4 = uart_read_avail(3,&g_svc_buf,1000);
        if (iVar4 != 0) {
          (&DAT_a00012fb)[iVar4] = 0;
          svc_transmit(&g_svc_buf,iVar4);
        }
      }
    }
    break;
  case 0x26:
  case 0x27:
  case 0x28:
  case 0x29:
    local_c = FUN_9d05d918(5);
    FUN_9d05da60(local_c,local_318[0] - 0x26);
    FUN_9d05d770(local_c,local_10);
    local_c = FUN_9d05dbf8();
    FUN_9d05d9dc(local_c);
    break;
  case 0x2a:
    local_c = FUN_9d05d918(6);
    thunk_FUN_9d05d668(local_c,local_318 + 1,0x20);
    FUN_9d05d770(local_c,local_10);
    local_c = FUN_9d05dbf8();
    iVar4 = FUN_9d05db64(local_c);
    FUN_9d05d9dc(local_c);
    svc_send_status(iVar4);
    if ((iVar4 == 0x30) && (local_318[1] == 0xf7)) {
      svc_transmit(&DAT_a00002a4,6);
    }
    break;
  case 0x2b:
    local_c = FUN_9d05d918(0xb);
    thunk_FUN_9d05d668(local_c,local_318 + 1,0x20);
    FUN_9d05d770(local_c,local_10);
    local_c = FUN_9d05dbf8();
    uVar6 = FUN_9d05db64(local_c);
    FUN_9d05d9dc(local_c);
    svc_send_status(uVar6);
    break;
  case 0x2c:
    *(undefined4 *)(&DAT_a0002950 + (uint)local_318[1] * 0x10) = 0;
    output_force_on(local_318[1]);
    break;
  case 0x2d:
    output_force_off2(local_318[1]);
    break;
  case 0x2e:
    local_318[0] = 0x78;
    uVar1 = FUN_9d02ff34(local_318,&DAT_9d066238,auStack_14c,5);
    svc_send_status(uVar1);
    break;
  case 0x2f:
    svc_send_status(1);
    sys_restart();
    break;
  case 0x30:
    uVar3 = CONCAT11(local_318[2],local_318[1]);
    if (uVar3 < 199) {
      FUN_9d054ee4(1,0xe8,100,100,0x32);
      uVar5 = svc_receive(local_318,uVar3);
      if (uVar3 == uVar5) {
        for (local_338 = 0; local_338 < (int)(uint)uVar3; local_338 = local_338 + 3) {
          if (51999 < CONCAT11(local_318[local_338 + 1],local_318[local_338])) {
            svc_send_status(0);
            FUN_9d054dac(1);
            break;
          }
          (&DAT_a0008696)[CONCAT11(local_318[local_338 + 1],local_318[local_338])] =
               local_318[local_338 + 2];
        }
        local_334 = (ushort)CONFIG_SIZE;
        g_cfg_crc = CONCAT11((undefined1)local_314,local_318[3]);
        goto LAB_9d03c8d4;
      }
    }
    svc_send_status(0);
    FUN_9d054dac(1);
    break;
  case 0x31:
    if (local_318[1] == 0) {
      _g_sys_flags = _g_sys_flags & 0xfffe;
    }
    else {
      _g_sys_flags = _g_sys_flags & 0xfffe | 1;
      zone_outputs_off(0);
      zone_outputs_off(1);
      zone_outputs_off(2);
      zone_outputs_off(3);
    }
    event_log(0x1c,_g_sys_flags & 1);
    svc_send_status(1);
    break;
  case 0x4a:
    local_c = FUN_9d05d918(0x4a);
    thunk_FUN_9d05d668(local_c,local_318 + 1,1);
    FUN_9d05d770(local_c,local_10);
    local_c = FUN_9d05dbf8();
    iVar4 = FUN_9d05db64(local_c);
    FUN_9d05d9dc(local_c);
    iVar7 = svc_send_status(iVar4);
    if ((0 < iVar7) && (iVar4 == 0x30)) {
      svc_transmit(&g_svc_buf,0x5dc);
    }
    break;
  case 0x4b:
    svc_receive(&g_svc_buf,CONCAT11(local_318[3],local_318[2]));
    local_c = FUN_9d05d918(0x4b);
    thunk_FUN_9d05d668(local_c,local_318 + 1,3);
    FUN_9d05d770(local_c,local_10);
    local_c = FUN_9d05dbf8();
    uVar6 = FUN_9d05db64(local_c);
    FUN_9d05d9dc(local_c);
    svc_send_status(uVar6);
    break;
  case 0x4c:
    uVar6 = FUN_9d028d4c(&g_svc_buf);
    svc_transmit(&g_svc_buf,uVar6);
  }
  return 1;
}

