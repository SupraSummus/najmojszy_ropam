// remote_connect @ 9d03ec88
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Połączenie GPRS (AT+CSTT, CIICR, CIPSTART TCP), handshake, sesja zdalna, potem restart
   centrali. */

undefined4 remote_connect(undefined4 param_1,undefined4 param_2)

{
  char cVar1;
  uint uVar2;
  int iVar3;
  int local_98;
  undefined1 auStack_90 [112];
  undefined1 auStack_20 [24];
  
  FUN_9d054d48(1);
  cVar1 = FUN_9d02fd80(0xc);
  if (cVar1 != '\0') {
    os_delay(10000);
    uart_rx_flush(3);
    uVar2 = FUN_9d06a74c(&DAT_a00151dc);
    if (uVar2 < 2) {
      sprintf(auStack_90,s_AT_CSTT___s__9d068164,&DAT_a00151bc);
    }
    else {
      sprintf(auStack_90,s_AT_CSTT___s____s____s__9d06814c,&DAT_a00151bc,&DAT_a00151dc,&DAT_a00151fc
             );
    }
    iVar3 = FUN_9d02e7b4(auStack_90,6,&DAT_9d066238);
    if ((iVar3 != 0) && (iVar3 = FUN_9d02e7b4(s_CIICR_9d068174,0x14,&DAT_9d066238), iVar3 != 0)) {
      uart_printf(3,s_AT_CIFSR_9d06817c);
      os_delay(5000);
      uart_rx_flush(3);
      FUN_9d02e7b4(s_CDNSCFG__8_8_8_8___8_8_4_4__9d0681c0,1,&DAT_9d066238);
    }
    for (local_98 = 0; local_98 < 2; local_98 = local_98 + 1) {
      sprintf(auStack_90,s_AT_CIPSTART__TCP____s____s__9d068188,param_1,param_2);
      iVar3 = FUN_9d02e7b4(auStack_90,5,&DAT_9d066238);
      if (iVar3 != 0) {
        iVar3 = FUN_9d030514(s_CONNECT_9d066298,15000);
        if (iVar3 != 0) {
          FUN_9d064c44(DAT_a0000368,1,2);
          os_delay(1000);
          sprintf(auStack_20,s__02X_02X_02X_02X_02X_02X_02X_02X_9d0681dc,(undefined1)g_panel_id,
                  g_panel_id._1_1_,g_panel_id._2_1_,g_panel_id._3_1_,(undefined1)DAT_a0000300,
                  DAT_a0000300._1_1_,DAT_a0000300._2_1_,DAT_a0000300._3_1_);
          iVar3 = FUN_9d03eae4(auStack_20);
          if (iVar3 != 0) {
            while ((DAT_a000277c._1_1_ & 0x7f) >> 6 != 0) {
              os_delay(5000);
            }
            FUN_9d02e7b4(s_CIPCLOSE_9d068140,10,s_CLOSE_OK_9d0662fc);
            sys_restart();
          }
        }
        os_delay(20000);
        FUN_9d02e7b4(s_CIPCLOSE_9d068140,10,s_CLOSE_OK_9d0662fc);
      }
    }
  }
  FUN_9d03e0a0(1);
  FUN_9d054dac(1);
  return 0;
}

