// xmodem_receive @ 9d04e750
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Odbiornik XMODEM-CRC (bloki 64/128/1024 B), wzorowany na xmodem.c G. Menie. */

int xmodem_receive(int param_1,int param_2)

{
  char cVar1;
  undefined4 uVar2;
  int iVar3;
  int local_30;
  int local_2c;
  char local_28;
  byte local_27;
  int local_24;
  int local_20;
  int local_1c;
  int local_18;
  
  local_2c = 0;
  local_28 = 'C';
  local_27 = 1;
  local_24 = 0;
  local_1c = 2;
LAB_9d04e788:
  local_20 = 0;
  do {
    if (0xf < local_20) {
      svc_flush();
      svc_putc(0x18);
      svc_putc(0x18);
      svc_putc(0x18);
      return -2;
    }
    if (local_28 != '\0') {
      svc_putc(local_28);
    }
    uVar2 = svc_getc(2000);
    if (DAT_a0000078 == 0) {
      switch(uVar2) {
      case 1:
        goto switchD_9d04e7e8_caseD_1;
      case 2:
        local_30 = 0x400;
        goto LAB_9d04e918;
      case 4:
        svc_flush();
        svc_putc(6);
        return local_24;
      case 0x18:
        iVar3 = svc_getc(1000);
        if (iVar3 == 0x18) {
          svc_flush();
          svc_putc(6);
          return -1;
        }
        break;
      case 0x19:
        local_30 = 0x40;
        goto LAB_9d04e918;
      }
    }
    local_20 = local_20 + 1;
  } while( true );
switchD_9d04e7e8_caseD_1:
  local_30 = 0x80;
LAB_9d04e918:
  if (local_28 == 'C') {
    local_2c = 1;
  }
  local_28 = '\0';
  g_xmodem_buf = (undefined1)uVar2;
  cVar1 = svc_read(&DAT_a0000b69,(uint)(local_2c != 0) + local_30 + 3,4000);
  if (((cVar1 == '\x01') && (DAT_a0000b69 == (byte)~DAT_a0000b6a)) &&
     (iVar3 = xmodem_check(local_2c,&DAT_a0000b6b,local_30), iVar3 != 0)) {
    if (local_27 == DAT_a0000b69) {
      local_18 = param_2 - local_24;
      if (local_30 < local_18) {
        local_18 = local_30;
      }
      if (0 < local_18) {
        memcpy(param_1 + local_24,&DAT_a0000b6b,local_18);
        local_24 = local_24 + local_18;
      }
      local_27 = local_27 + 1;
      local_1c = 3;
      svc_putc(6);
    }
    else if ((uint)DAT_a0000b69 == local_27 - 1) {
      svc_putc(6);
    }
    else {
      local_1c = local_1c + -1;
      if (local_1c < 1) {
        svc_flush();
        svc_putc(0x18);
        svc_putc(0x18);
        svc_putc(0x18);
        return -3;
      }
    }
  }
  else {
    svc_flush();
    svc_putc(0x15);
  }
  goto LAB_9d04e788;
}

