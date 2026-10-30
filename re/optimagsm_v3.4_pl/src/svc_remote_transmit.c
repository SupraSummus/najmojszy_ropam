// svc_remote_transmit @ 9d04f3bc
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Wysyłka przez modem. ❓ */

uint svc_remote_transmit(int param_1,uint param_2)

{
  ushort uVar1;
  int iVar2;
  uint local_20;
  uint local_1c;
  undefined4 local_10;
  undefined4 local_c;
  
  local_1c = 0xffffffff;
  local_10 = DAT_9d069180;
  local_c = DAT_9d069184;
  FUN_9d062524(param_1,param_2,&DAT_a0015d35,&local_10);
  svc_flush();
  local_20 = 0;
  while( true ) {
    if (3 < (int)local_20) goto LAB_9d04f55c;
    iVar2 = svc_getc(2000);
    if ((DAT_a0000078 == 0) && (iVar2 == 0x43)) break;
    local_20 = local_20 + 1;
  }
  svc_flush();
  svc_putc(0x53);
  svc_putc(param_2 & 0xff);
  svc_putc((int)param_2 >> 8 & 0xff);
  for (local_20 = 0; (int)local_20 < (int)param_2; local_20 = local_20 + 1) {
    svc_putc(*(undefined1 *)(param_1 + local_20));
    if (((local_20 & 0xff) == 0) && (0 < (int)local_20)) {
      os_delay(5000);
    }
  }
  uVar1 = crc16_xmodem(param_1,param_2);
  svc_putc(uVar1 & 0xff);
  svc_putc(uVar1 >> 8);
  os_delay(5000);
  local_1c = param_2;
LAB_9d04f55c:
  FUN_9d062524(param_1,param_2,&DAT_a0015d35,&local_10);
  return local_1c;
}

