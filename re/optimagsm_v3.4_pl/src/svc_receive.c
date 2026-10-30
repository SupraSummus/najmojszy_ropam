// svc_receive @ 9d04eb24
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Odbiór danych: XMODEM lokalnie albo ramki 'S' przez modem. */

undefined4 svc_receive(undefined4 param_1,undefined4 param_2)

{
  uint uVar1;
  undefined4 local_10;
  
  uVar1 = (uint)DAT_a000277e;
  if ((DAT_a000277c._1_1_ & 0x7f) >> 6 == 0) {
    local_10 = xmodem_receive(param_1,param_2);
    if ((int)(uVar1 << 0x1a) < 0) {
      FUN_9d062524(param_1,param_2,&DAT_a0015d35,&DAT_9d068f78);
    }
  }
  else {
    local_10 = svc_remote_receive(param_1,param_2);
  }
  return local_10;
}

