// expin_service @ 9d030d60
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* Gniazdo 6, ekspander wejść: 0xDD (8 B stanu systemu) → 16 B stanów wejść i status; co 11.
   obieg 0xDF, co 60. 0xDC. */

undefined4 expin_service(void)

{
  bool bVar1;
  bool bVar2;
  byte bVar3;
  byte bVar4;
  byte bVar5;
  char cVar6;
  short sVar7;
  ushort uVar8;
  ushort uVar9;
  uint uVar10;
  uint uVar11;
  int iVar12;
  undefined4 uVar13;
  undefined8 uVar14;
  uint local_f8;
  uint local_f4;
  undefined2 local_f0;
  ushort local_ec;
  ushort local_ea;
  ushort local_e8;
  undefined2 auStack_c0 [16];
  undefined1 local_a0 [28];
  byte local_84;
  undefined1 local_83;
  undefined1 local_82;
  undefined1 local_81;
  undefined1 local_80;
  byte local_7f;
  undefined1 local_7e;
  undefined1 local_7d;
  undefined1 local_7c;
  undefined1 local_7b;
  undefined1 local_7a;
  undefined1 local_79;
  uint local_78 [2];
  uint local_70;
  uint local_68;
  uint local_60;
  char local_58 [80];
  
  local_84 = 0;
  local_83 = 0;
  local_82 = 0;
  local_81 = 0;
  local_80 = 0;
  local_7f = 0;
  local_7e = 0;
  local_7d = 0;
  local_7c = 0;
  local_7b = 0;
  local_7a = 0;
  local_79 = 0;
  local_ec = 0;
  memset(local_58,0,0x50);
  local_ea = 0;
  FUN_9d05e3e4(g_zones_state,local_78,0);
  uVar13 = _DAT_a000028c;
  uVar10 = (local_78[0] & 0x7ff) >> 10 | ((local_70 & 0x7ff) >> 10) << 1 |
           ((local_68 & 0x7ff) >> 10) << 2 | ((local_60 & 0x7ff) >> 10) << 3;
  for (local_f4 = 0; local_f4 < 0x10; local_f4 = local_f4 + 1) {
    iVar12 = DAT_a00159e7 + local_f4;
    uVar11 = (uint)(byte)(&DAT_a0002b6b)[iVar12 * 0x28] |
             (uint)(byte)(&DAT_a0002b6c)[iVar12 * 0x28] << 1 |
             (uint)(byte)(&DAT_a0002b6d)[iVar12 * 0x28] << 2 |
             (uint)(byte)(&DAT_a0002b6e)[iVar12 * 0x28] << 3;
    if ((uVar11 == 0) || ((uVar10 & uVar11) != uVar11)) {
      bVar1 = false;
    }
    else {
      bVar1 = true;
    }
    if ((uVar11 == 0) ||
       ((((local_78[0] & 0xfff) >> 0xb | ((local_70 & 0xfff) >> 0xb) << 1 |
          ((local_68 & 0xfff) >> 0xb) << 2 | ((local_60 & 0xfff) >> 0xb) << 3) & uVar11) != uVar11))
    {
      bVar2 = false;
    }
    else {
      bVar2 = true;
    }
    if (((bVar1 || bVar2) && ((*(uint *)(&DAT_a0002b68 + iVar12 * 0x28) & 0x7f) >> 6 == 0)) &&
       ((*(uint *)(&DAT_a0002b68 + iVar12 * 0x28) & 0xfff) >> 0xb == 0)) {
      sVar7 = 1;
    }
    else {
      sVar7 = 0;
    }
    local_ec = sVar7 << (local_f4 & 0x1f) | local_ec;
  }
  if (uVar10 == 0) {
    bVar3 = 0;
  }
  else {
    bVar3 = 8;
  }
  local_84 = uVar10 != 0 | (byte)_g_sys_flags & 2 | (byte)((_g_sys_flags & 1) << 2) | bVar3 | 0x80;
  local_83 = (undefined1)local_ec;
  local_82 = (undefined1)(local_ec >> 8);
  _DAT_a000028c =
       (uint)(byte)((byte)((local_78[0] << 0x15) >> 0x1f) | (byte)(((local_70 & 0x7ff) >> 10) << 1)
                    | (byte)(((local_68 & 0x7ff) >> 10) << 2) |
                   (byte)(((local_60 & 0x7ff) >> 10) << 3));
  if ((local_70 & 0xf) == 5) {
    bVar3 = 2;
  }
  else {
    bVar3 = 0;
  }
  if ((local_68 & 0xf) == 5) {
    bVar4 = 4;
  }
  else {
    bVar4 = 0;
  }
  if ((local_60 & 0xf) == 5) {
    bVar5 = 8;
  }
  else {
    bVar5 = 0;
  }
  uVar8 = (ushort)(byte)((byte)((local_78[0] << 0x17) >> 0x1f) |
                         (byte)(((local_70 & 0x1ff) >> 8) << 1) |
                         (byte)(((local_68 & 0x1ff) >> 8) << 2) |
                        (byte)(((local_60 & 0x1ff) >> 8) << 3)) << 4 |
          (ushort)(byte)((byte)((_DAT_a000028c << 0x1c) >> 0x1c) | (local_78[0] & 0xf) == 5 | bVar3
                         | bVar4 | bVar5);
  DAT_a000028e = SUB41(uVar13,2);
  local_7f = DAT_a000028e & 0xf0 |
             (byte)((local_78[0] << 0x14) >> 0x1f) | (byte)(((local_70 & 0xfff) >> 0xb) << 1) |
             (byte)(((local_68 & 0xfff) >> 0xb) << 2) | (byte)(((local_60 & 0xfff) >> 0xb) << 3);
  DAT_a000028e_1 = SUB41(uVar13,3);
  _DAT_a000028c = CONCAT12(local_7f,uVar8);
  uVar9 = uVar8 | (ushort)(byte)((byte)(local_78[0] >> 8) >> 7 |
                                 (byte)(((local_70 & 0xffff) >> 0xf) << 1) |
                                 (byte)(((local_68 & 0xffff) >> 0xf) << 2) |
                                (byte)(((local_60 & 0xffff) >> 0xf) << 3)) << 8 |
          (ushort)(byte)((byte)((local_78[0] << 0xf) >> 0x1f) |
                         (byte)(((local_70 & 0x1ffff) >> 0x10) << 1) |
                         (byte)(((local_68 & 0x1ffff) >> 0x10) << 2) |
                        (byte)(((local_60 & 0x1ffff) >> 0x10) << 3)) << 0xc;
  _DAT_a000028c = CONCAT22(_DAT_a000028e,uVar9);
  uVar13 = _DAT_a000028c;
  DAT_a000028c._0_1_ = (undefined1)uVar8;
  local_81 = (undefined1)DAT_a000028c;
  DAT_a000028c._1_1_ = (undefined1)(uVar9 >> 8);
  local_80 = DAT_a000028c._1_1_;
  _DAT_a000028c = uVar13;
  for (local_f8 = 0; local_f8 < 0x10; local_f8 = local_f8 + 1) {
    local_e8 = FUN_9d008a78(local_f8 + 0x10);
    if ((*(uint *)(&DAT_a0006914 + (local_f8 + 0x10) * 0xe0) & 0x3ff) >> 8 == 1) {
      local_e8 = (ushort)(local_e8 == 0);
    }
    local_ea = local_e8 << (local_f8 & 0x1f) | local_ea;
  }
  local_7e = (undefined1)local_ea;
  local_7d = (undefined1)(local_ea >> 8);
  DAT_a0000080 = DAT_a0000080 + 1;
  if (DAT_a0000084 < '\v') {
    if (';' < DAT_a0000000) {
      DAT_a0000000 = 0;
      DAT_a0000084 = DAT_a0000084 + '\x01';
      uVar13 = expin_read_dc();
      return uVar13;
    }
    DAT_a0000000 = DAT_a0000000 + '\x01';
    DAT_a0000084 = DAT_a0000084 + '\x01';
    rnet_build_frame(&g_rnet_tx,DAT_a00159e2,0xdd,&local_84,8);
    uart_rx_flush(0);
    iVar12 = rnet_send_frame(0,&g_rnet_tx);
    if (iVar12 == 0) {
      return 0;
    }
    iVar12 = rnet_recv_frame(0,&g_rnet_rx);
    uVar10 = _DAT_a0000288;
    if ((iVar12 == 0) || (g_rnet_rx._1_1_ != -0x23)) goto LAB_9d032120;
    DAT_a0001274 = ram0xa001700b;
    DAT_a0001278 = ram0xa001700f;
    DAT_a000127c = ram0xa0017013;
    DAT_a0001280 = ram0xa0017017;
    _DAT_a0000288 = CONCAT11(DAT_a001701a._2_1_,DAT_a001701a._1_1_);
    uRama000028b = SUB41(uVar10,3);
    _DAT_a0000288 = CONCAT12(DAT_a001701a._3_1_,_DAT_a0000288);
    _DAT_a0000290 = CONCAT12(DAT_a001701e._2_1_,(undefined2)DAT_a001701e);
    uVar10 = _DAT_a0000290;
    if (DAT_a0000085 == '\0') {
      DAT_a0001284 = ram0xa001700b;
      DAT_a0001288 = ram0xa001700f;
      DAT_a000128c = ram0xa0017013;
      DAT_a0001290 = ram0xa0017017;
      DAT_a0001294 = _DAT_a0000288;
      DAT_a0001296 = DAT_a001701a._3_1_;
      DAT_a0000290 = (undefined1)(undefined2)DAT_a001701e;
      DAT_a0001297 = DAT_a0000290;
      DAT_a0000291 = (undefined1)((ushort)(undefined2)DAT_a001701e >> 8);
      DAT_a0001298 = DAT_a0000291;
      DAT_a0001299 = DAT_a001701e._2_1_;
    }
    DAT_a00012f9 = 0x73;
    if (DAT_a001701e._2_1_ != 0) {
      DAT_a0000098 = (ushort)DAT_a001701e._2_1_;
    }
    for (local_f8 = 0; local_f8 < 0x10; local_f8 = local_f8 + 1) {
      if ((*(byte *)((int)&DAT_a0001274 + local_f8) & 1) == 0) {
        auStack_c0[local_f8] = 60000;
      }
      else {
        if ((_DAT_a0000288 & 0xff) >> 7 == 0) {
          if ((*(byte *)((int)&DAT_a0001274 + local_f8) & 0xf) >> 3 == 0) {
            if ((*(byte *)((int)&DAT_a0001274 + local_f8) & 3) >> 1 == 0) {
              local_f0 = 0x44c;
            }
            else {
              local_f0 = 0x898;
            }
            if ((*(byte *)((int)&DAT_a0001274 + local_f8) & 7) >> 2 != 0) {
              local_f0 = 60000;
            }
          }
          else if ((DAT_a0015a74 & 0xfffffff) >> 0x1b == 1) {
            local_f0 = 0x898;
          }
          else {
            local_f0 = 60000;
          }
        }
        else if ((DAT_a0015a74 & 0xfffffff) >> 0x1b == 1) {
          local_f0 = 0x898;
        }
        else {
          local_f0 = 60000;
        }
        auStack_c0[local_f8] = local_f0;
      }
    }
    DAT_a00159e3 = DAT_a00159e3 & 0xfb | ((char)(DAT_a001701a._1_1_ << 3) < '\0') << 2;
    _DAT_a0000290 = uVar10;
    if ((uint)DAT_a00159e7 + (uint)DAT_a00159e6 < 0x30) {
      inputs_set_lines(DAT_a00159e7,DAT_a00159e6,auStack_c0);
    }
    else {
      inputs_set_lines(DAT_a00159e7,
                       (uint)DAT_a00159e6 - (((uint)DAT_a00159e7 + (uint)DAT_a00159e6) - 0x30),
                       auStack_c0);
    }
    for (local_f8 = 0; local_f8 < 0x10; local_f8 = local_f8 + 1) {
      if ((*(byte *)((int)&DAT_a0001274 + local_f8) & 1) != 0) {
        if (((*(byte *)((int)&DAT_a0001274 + local_f8) & 0x1f) >> 4 == 1) &&
           (((&DAT_a00010b8)[local_f8] & 1) == 0)) {
          event_log(0x73,(uint)DAT_a00159e7 + (local_f8 & 0xff) & 0xff);
          if ((DAT_a0001209 & 1) == 0) {
            strcpy(local_58,&DAT_a000120a);
          }
          else {
            sprintf(local_58,s__s__i_9d06e2b0,&DAT_a000120a,DAT_a00159e7 + local_f8 + 1);
          }
          if (local_58[0] != '\0') {
            FUN_9d01f600(DAT_a0001208,local_58,0);
          }
        }
        else if (((*(byte *)((int)&DAT_a0001274 + local_f8) & 0x1f) >> 4 == 0) &&
                (((&DAT_a00010b8)[local_f8] & 1) == 1)) {
          event_log(0x74,(uint)DAT_a00159e7 + (local_f8 & 0xff) & 0xff);
        }
        if (((*(byte *)((int)&DAT_a0001274 + local_f8) & 0xf) >> 3 == 1) &&
           (((byte)(&DAT_a00010b8)[local_f8] & 3) >> 1 == 0)) {
          event_log(0x71,(uint)DAT_a00159e7 + (local_f8 & 0xff) & 0xff);
        }
        else if (((*(byte *)((int)&DAT_a0001274 + local_f8) & 0xf) >> 3 == 0) &&
                (((byte)(&DAT_a00010b8)[local_f8] & 3) >> 1 == 1)) {
          event_log(0x72,(uint)DAT_a00159e7 + (local_f8 & 0xff) & 0xff);
        }
        (&DAT_a00010b8)[local_f8] =
             (&DAT_a00010b8)[local_f8] & 0xfd |
             (byte)(((*(byte *)((int)&DAT_a0001274 + local_f8) & 0xf) >> 3) << 1);
        (&DAT_a00010b8)[local_f8] =
             (&DAT_a00010b8)[local_f8] & 0xfe |
             (byte)(((uint)*(byte *)((int)&DAT_a0001274 + local_f8) << 0x1b) >> 0x1f);
      }
    }
  }
  else {
    DAT_a0000084 = '\0';
    iVar12 = expin_read_df(&DAT_a0001284,&local_84);
    uVar10 = _DAT_a0000290;
    if (iVar12 == 0) {
      return 0;
    }
    uRama0000293 = SUB41(uVar10,3);
    _DAT_a0000290 = CONCAT12(DAT_a00012ed,CONCAT11(DAT_a00012ec,DAT_a00012eb));
    if (DAT_a00012ed != 0) {
      DAT_a0000098 = (ushort)DAT_a00012ed;
    }
    if ((DAT_a00012eb & 0x1f) == 0) {
      return 1;
    }
  }
  if (DAT_a0000085 == '\0') {
    DAT_a0001297 = DAT_a0000290;
    DAT_a0001298 = DAT_a0000291;
    DAT_a0001299 = DAT_a0000292;
  }
  if ((_DAT_a0000290 & 0x1f) != 0) {
    for (local_f8 = 0; local_f8 < 5; local_f8 = local_f8 + 1) {
      if (((int)((_DAT_a0000290 & 0x1fff) >> 8) >> (local_f8 & 0x1f) & 1U) != 0) {
        cVar6 = FUN_9d032710(local_58,(int)(char)local_f8,_DAT_a0000290 & 0x1f);
        if ((cVar6 != '\0') && (local_58[0] != '\0')) {
          FUN_9d01f600(DAT_a0001208,local_58,0);
        }
        FUN_9d037010(&DAT_9d06e2b8,local_f8,1);
        break;
      }
    }
    FUN_9d036f80(&DAT_9d06e2bc,_DAT_a0000290 & 0x1f);
    if (((_DAT_a0000290 & 0x2000) != 0) && (DAT_a000007c != '\x01')) {
      event_log(0x6a,_DAT_a0000290 & 0x1f);
      FUN_9d03267c(_DAT_a0000290 & 0x1f,1);
      DAT_a000007c = '\x01';
      FUN_9d036f80(&DAT_9d06e2c0,1);
      if ((DAT_a0001209 & 1) == 0) {
        strcpy(local_58,&DAT_a000120a);
      }
      else {
        sprintf(local_58,s__s__i_9d06e2b0,&DAT_a000120a,_DAT_a0000290 & 0x1f);
      }
      if (local_58[0] != '\0') {
        FUN_9d01f600(DAT_a0001208,local_58,0);
      }
    }
    if ((DAT_a000007c != '\0') && ((_DAT_a0000290 & 0x2000) == 0)) {
      DAT_a000007c = '\0';
      FUN_9d03267c(_DAT_a0000290 & 0x1f,0);
      FUN_9d036f80(&DAT_9d06e2c0,0);
    }
    DAT_a0000085 = DAT_a0000085 + '\x01';
    if (DAT_a0000085 == '\x01') {
      os_delay(100);
      expin_service();
    }
  }
  if ((DAT_a0000086 != 1) && ((_DAT_a0000288 & 0xff) >> 7 != 0)) {
    event_log(0x7f,DAT_a000028a);
  }
  DAT_a0000086 = (byte)_DAT_a0000288 >> 7;
  iVar12 = shared_lock(DAT_a00001b0);
  *(byte *)(iVar12 + 2) = *(byte *)(iVar12 + 2) & 0xfe | (byte)_DAT_a0000288 >> 7;
  shared_unlock(DAT_a00001b0);
  DAT_a0000085 = '\0';
  uVar14 = FUN_9d0322e4();
  DAT_a00010ac = (int)((ulonglong)uVar14 >> 0x20);
  DAT_a00010a8 = (int)uVar14;
  uVar14 = FUN_9d0324a0();
  DAT_a00010a4 = (int)((ulonglong)uVar14 >> 0x20);
  DAT_a00010a0 = (int)uVar14;
  FUN_9d036f80(&DAT_9d06e2c0,DAT_a00010a8 != 0 || DAT_a00010ac != 0);
  FUN_9d036f80(&DAT_9d06e2c4,DAT_a00010a0 != 0 || DAT_a00010a4 != 0);
  _DAT_a0000290 = _DAT_a0000290 & 0xffffe0ff;
LAB_9d032120:
  if (((_DAT_a0000288 & 0x40) != 0) && (0x707 < DAT_a0000080)) {
    local_a0[0] = 2;
    FUN_9d03226c(local_a0,0x1a);
  }
  return 1;
}

