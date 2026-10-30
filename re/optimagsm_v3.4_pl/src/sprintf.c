// sprintf @ 9d06d240
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

int sprintf(undefined1 *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  undefined4 local_res8;
  undefined4 local_resc;
  undefined1 *local_20;
  undefined4 local_1c;
  undefined1 *local_18;
  undefined2 local_14;
  undefined4 local_10;
  undefined4 *local_c;
  
  local_c = &local_res8;
  local_10 = 0x7fff;
  local_1c = 0;
  local_14 = 0xc2;
  local_res8 = param_3;
  local_resc = param_4;
  local_20 = param_1;
  local_18 = param_1;
  FUN_9d045bf8(&local_20,param_2,local_c);
  *local_20 = 0;
  return (int)local_20 - (int)param_1;
}

