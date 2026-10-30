// exp_send_wake @ 9d053b2c
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Port 1: ramka budząca (cmd 0x9B, bez danych), 150 ms przerwy, właściwa ramka. */

undefined4 exp_send_wake(undefined1 *param_1)

{
  int iVar1;
  undefined1 local_100;
  undefined1 local_ff;
  undefined1 local_fe;
  
  local_100 = *param_1;
  local_ff = 0x9b;
  local_fe = 0xaa;
  iVar1 = rnet_send_frame(1,&local_100);
  if (iVar1 != 0) {
    os_delay(0x96);
    iVar1 = rnet_send_frame(1,param_1);
    if (iVar1 != 0) {
      return 1;
    }
  }
  return 0;
}

