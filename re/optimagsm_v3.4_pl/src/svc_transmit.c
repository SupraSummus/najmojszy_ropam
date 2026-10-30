// svc_transmit @ 9d04f00c
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Wysyłka danych: XMODEM lokalnie albo ramki 'S' przez modem. */

undefined4 svc_transmit(undefined4 param_1,undefined4 param_2)

{
  uint uVar1;
  undefined4 local_18;
  undefined4 local_10;
  undefined4 local_c;
  
  local_10 = DAT_9d069180;
  local_c = DAT_9d069184;
  uVar1 = (uint)DAT_a000277e;
  if ((DAT_a000277c._1_1_ & 0x7f) >> 6 == 0) {
    if ((int)(uVar1 << 0x1a) < 0) {
      FUN_9d062524(param_1,param_2,&DAT_a0015d35,&local_10);
    }
    local_18 = xmodem_transmit(param_1,param_2);
    if ((int)(uVar1 << 0x1a) < 0) {
      FUN_9d062524(param_1,param_2,&DAT_a0015d35,&local_10);
    }
  }
  else {
    local_18 = svc_remote_transmit(param_1,param_2);
  }
  return local_18;
}

