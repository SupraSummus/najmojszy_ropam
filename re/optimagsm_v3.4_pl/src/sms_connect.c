// sms_connect @ 9d013a20
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* Polecenie SMS connect [host:port]: tylko bez czuwania, kod serwisu albo 0, flaga COMM bit 1 albo
   tryb serwisowy. */

undefined4 sms_connect(int param_1,int param_2)

{
  int iVar1;
  undefined1 *puVar2;
  undefined4 local_58;
  uint local_4c [2];
  uint local_44;
  uint local_3c;
  uint local_34;
  undefined4 local_2c;
  undefined1 local_28;
  undefined1 local_27;
  undefined1 local_26;
  undefined1 local_25;
  undefined1 local_24;
  undefined1 local_23;
  char local_20 [4];
  char local_1c [4];
  char local_18 [2];
  char local_16;
  undefined1 local_15;
  undefined1 local_14;
  undefined1 local_13;
  undefined1 local_12;
  undefined1 local_11;
  undefined1 local_10;
  undefined1 local_f;
  undefined1 local_e;
  undefined1 local_d;
  
  local_58 = 0x32;
  local_2c = DAT_9d0571f0;
  local_28 = DAT_9d0571f4;
  local_27 = 0;
  local_26 = 0;
  local_25 = 0;
  local_24 = 0;
  local_23 = 0;
  local_20 = (char  [4])s_iropam_com_9d0571fc._0_4_;
  local_1c = (char  [4])s_iropam_com_9d0571fc._4_4_;
  local_18 = (char  [2])s_iropam_com_9d0571fc._8_2_;
  local_16 = s_iropam_com_9d0571fc[10];
  local_15 = 0;
  local_14 = 0;
  local_13 = 0;
  local_12 = 0;
  local_11 = 0;
  local_10 = 0;
  local_f = 0;
  local_e = 0;
  local_d = 0;
  FUN_9d05e3e4(g_zones_state,local_4c,0);
  if (((((local_4c[0] & 0x7ff) >> 10 == 0) && ((local_44 & 0x7ff) >> 10 == 0)) &&
      ((local_3c & 0x7ff) >> 10 == 0)) && ((local_34 & 0x7ff) >> 10 == 0)) {
    iVar1 = FUN_9d06d1d8(param_1 + 8,0x2e);
    puVar2 = (undefined1 *)FUN_9d06d1d8(param_1 + 8,0x3a);
    if (puVar2 != (undefined1 *)0x0) {
      strcpy(&local_2c,puVar2 + 1);
      *puVar2 = 0;
    }
    if ((param_2 == 100) || (param_2 == 0)) {
      if ((DAT_a000278c & 1) != 0) {
        FUN_9d03e0a0(1);
      }
      FUN_9d03de1c();
      if (iVar1 != 0) {
        strcpy(local_20,param_1 + 8);
      }
      if ((((g_cfg_comm & 3) >> 1 != 1) && ((_g_sys_flags & 1) != 1)) ||
         ((iVar1 = remote_connect(local_20,&local_2c), iVar1 == 0 &&
          (iVar1 = remote_connect(local_20,&local_2c), iVar1 == 0)))) {
        local_58 = 0x27;
      }
    }
    else {
      local_58 = 0x26;
    }
  }
  else {
    local_58 = 0x27;
  }
  return local_58;
}

