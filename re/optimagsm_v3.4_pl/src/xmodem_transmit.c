// xmodem_transmit @ 9d04ebc8
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Nadajnik XMODEM; rozmiar bloku wg długości: ≤64 → 0x19, <1024 → SOH, inaczej STX. */

int xmodem_transmit(int param_1,int param_2)

{
  byte bVar1;
  undefined2 uVar2;
  int iVar3;
  int local_28;
  byte local_20;
  int local_1c;
  int local_18;
  int local_14;
  
  local_20 = 1;
  local_18 = 0;
  local_14 = 0;
  do {
    if (0xf < local_14) {
      svc_putc(0x18);
      svc_putc(0x18);
      svc_putc(0x18);
      svc_flush();
      return -2;
    }
    iVar3 = svc_getc(2000);
    if (DAT_a0000078 == 0) {
      if (iVar3 == 0x18) {
        iVar3 = svc_getc(1000);
        if (iVar3 == 0x18) {
          svc_putc(6);
          svc_flush();
          return -1;
        }
      }
      else {
        if (iVar3 == 0x43) {
          bVar1 = 1;
          svc_flush();
          break;
        }
        if (iVar3 == 0x15) {
          bVar1 = 0;
          break;
        }
      }
    }
    local_14 = local_14 + 1;
  } while( true );
LAB_9d04ecdc:
  if (param_2 < 0x400) {
    if (param_2 < 0x41) {
      g_xmodem_buf = 0x19;
      local_28 = 0x40;
    }
    else {
      g_xmodem_buf = 1;
      local_28 = 0x80;
    }
  }
  else {
    g_xmodem_buf = 2;
    local_28 = 0x400;
  }
  DAT_a0000b69 = local_20;
  DAT_a0000b6a = ~local_20;
  local_1c = param_2 - local_18;
  if (local_28 < local_1c) {
    local_1c = local_28;
  }
  if (-1 < local_1c) {
    memset(&DAT_a0000b6b,0,local_28);
    if (local_1c == 0) {
      DAT_a0000b6b = 0x1a;
    }
    else {
      memcpy(&DAT_a0000b6b,param_1 + local_18,local_1c);
      if (local_1c < local_28) {
        (&DAT_a0000b6b)[local_1c] = 0x1a;
      }
    }
    uVar2 = crc16_xmodem(&DAT_a0000b6b,local_28);
    (&DAT_a0000b6b)[local_28] = (char)((ushort)uVar2 >> 8);
    *(char *)(local_28 + -0x5ffff494) = (char)uVar2;
    local_14 = 0;
    do {
      if (1 < local_14) {
        svc_putc(0x18);
        svc_putc(0x18);
        svc_putc(0x18);
        svc_flush();
        return -4;
      }
      FUN_9d04e540(&g_xmodem_buf,local_28 + 4 + (uint)bVar1);
      iVar3 = svc_getc(1000);
      if (DAT_a0000078 == 0) {
        if (iVar3 == 6) goto LAB_9d04eee8;
        if ((iVar3 == 0x18) && (iVar3 = svc_getc(1000), iVar3 == 0x18)) {
          svc_putc(6);
          svc_flush();
          return -1;
        }
      }
      local_14 = local_14 + 1;
    } while( true );
  }
  local_14 = 0;
  goto LAB_9d04efc4;
LAB_9d04eee8:
  local_20 = local_20 + 1;
  local_18 = local_18 + local_28;
  goto LAB_9d04ecdc;
LAB_9d04efc4:
  if (9 < local_14) {
LAB_9d04efd4:
    svc_flush();
    if (local_1c == 6) {
      return local_18;
    }
    return -5;
  }
  svc_putc(4);
  local_1c = svc_getc(2000);
  if (local_1c == 6) goto LAB_9d04efd4;
  local_14 = local_14 + 1;
  goto LAB_9d04efc4;
}

