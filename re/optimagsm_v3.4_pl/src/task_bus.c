// task_bus @ 9d02ace0
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* WARNING: Removing unreachable block (ram,0x9d02adb8) */
/* Zadanie magistrali: zlecenia innych zadań, obieg odpytywania, co 50 obiegów rozgłoszenie stanu
   (0x10), co 410 ramka 0x3C. */

void task_bus(void)

{
  byte bVar1;
  bool bVar2;
  char cVar3;
  undefined1 uVar4;
  undefined4 uVar5;
  int iVar6;
  undefined4 uVar7;
  uint uVar8;
  int local_90;
  int local_8c;
  undefined4 local_58 [4];
  char local_48;
  char local_47;
  char local_46;
  undefined1 auStack_14 [12];
  
  local_8c = 0;
  os_delay(5000);
  FUN_9d028c38();
  FUN_9d01b014();
  FUN_9d02a344();
  DAT_a000029c = FUN_9d05e6c0(&DAT_9d06b35c);
  DAT_a00159ea = 0x76;
  os_delay(10000);
  uVar5 = FUN_9d0649dc(&DAT_9d06b364);
  FUN_9d02a584();
  if ((DAT_a00159f9 & 1) != 0) {
    FUN_9d02472c();
  }
  do {
    iVar6 = shared_lock(DAT_a00001b0);
    bVar1 = *(byte *)(iVar6 + 3);
    shared_unlock(DAT_a00001b0);
    FUN_9d064c44(uVar5,1,8);
    FUN_9d05a028(1);
    uVar7 = FUN_9d05dbf8();
    iVar6 = FUN_9d05a090();
    if (iVar6 == 0) {
      uVar8 = FUN_9d05da08(uVar7);
      bVar2 = false;
      switch(uVar8) {
      case 1:
        local_58[0] = FUN_9d04c318(s_M_TPR1_9d06b36c);
        local_58[1] = FUN_9d04c318(s_M_TPR2_9d06b374);
        local_58[2] = FUN_9d04c318(s_M_TPR3_9d06b37c);
        local_58[3] = FUN_9d04c318(s_M_TPR4_9d06b384);
        for (local_90 = 1; local_90 < 5; local_90 = local_90 + 1) {
          if (((&DAT_a00159a1)[local_90 * 0xb] & 1) == 0) {
            bVar2 = false;
          }
          else {
            iVar6 = rnet_send_multi(0,(&g_modules)[local_90 * 0xb],0,local_58[local_90 + -1],0xb68);
            bVar2 = iVar6 == 0;
            os_delay(200);
          }
        }
        break;
      case 2:
        thunk_FUN_9d05d558(uVar7,&local_48,0xc);
        iVar6 = FUN_9d058f0c(&local_48,10);
        bVar2 = iVar6 == 0;
        break;
      case 3:
        thunk_FUN_9d05d558(uVar7,&local_48,0x1a);
        iVar6 = FUN_9d03226c(&local_48,0x1a);
        bVar2 = iVar6 == 0;
        break;
      case 4:
        FUN_9d02a678();
        memcpy(&g_svc_buf,&g_modules,0xc6);
        bVar2 = false;
        break;
      case 5:
        uVar4 = FUN_9d05db40(uVar7);
        FUN_9d0249b4(uVar4);
        break;
      case 6:
        thunk_FUN_9d05d558(uVar7,&local_48,0x20);
        iVar6 = FUN_9d0506e8(&local_48);
        bVar2 = iVar6 == 0;
        break;
      case 7:
        iVar6 = FUN_9d02472c();
        bVar2 = iVar6 == 0;
        break;
      case 8:
        thunk_FUN_9d05d558(uVar7,auStack_14,8);
        DAT_a000009e = FUN_9d024450(auStack_14);
        bVar2 = DAT_a000009e == '\0';
        break;
      case 9:
        iVar6 = FUN_9d05db40(uVar7);
        DAT_a0000180 = iVar6 != 0;
        bVar2 = false;
        break;
      case 0xb:
        thunk_FUN_9d05d558(uVar7,&local_48,0x20);
        switch(local_48) {
        case '\x01':
          FUN_9d01f1f4();
          break;
        case '\x02':
          FUN_9d01f3d8();
          break;
        case '\x03':
          FUN_9d01f24c((int)local_47,(int)local_46);
          break;
        case '\x04':
          local_48 = '\x01';
          rnet_queue_msg(0x30,&local_48);
        }
        break;
      case 0xc:
        thunk_FUN_9d05d558(uVar7,&local_48,2);
        iVar6 = FUN_9d058f78((int)local_48,(int)local_47);
        bVar2 = iVar6 == 0;
        break;
      case 0xe:
        FUN_9d02a584();
        bVar2 = false;
        break;
      case 0x4a:
        thunk_FUN_9d05d558(uVar7,&local_48,1);
        iVar6 = FUN_9d01f448((int)local_48);
        bVar2 = iVar6 == 0;
        break;
      case 0x4b:
        thunk_FUN_9d05d558(uVar7,&local_48,3);
        cVar3 = local_48 + '\x01';
        iVar6 = rnet_send_multi(0,(&g_modules)[cVar3 * 0xb],0xf,&g_svc_buf,
                                (short)local_46 << 8 | (short)local_47);
        bVar2 = iVar6 == 0;
        *(undefined4 *)(&DAT_a0003300 + cVar3 * 0x10) = 0x9c4;
      }
      FUN_9d05d9cc(uVar7,uVar8 & 0xff);
      if (bVar2) {
        FUN_9d05da84(uVar7,0);
      }
      else {
        FUN_9d05da84(uVar7,0x30);
      }
      FUN_9d05dbd4(uVar7);
    }
    else {
      bus_poll_round((byte)(((uint)bVar1 << 0x1b) >> 0x1f));
    }
    FUN_9d065794();
    local_8c = local_8c + 1;
    if (local_8c == (local_8c / 0x32) * 0x32) {
      FUN_9d05a028(1);
      iVar6 = rb_read(DAT_a000026c,&local_48,1);
      FUN_9d05a090();
      if (iVar6 != 0) {
        rnet_queue_msg(local_48,&local_47);
      }
      os_delay(100);
      rnet_bcast_status(0xfe);
    }
    if (local_8c % 0x19a == 0) {
      rnet_bcast_3c(0xfe);
    }
    os_delay(100);
  } while( true );
}

