// vsprintf @ 9d06d6a8
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

int vsprintf(undefined1 *param_1)

{
  undefined1 *local_20;
  undefined4 local_1c;
  undefined1 *local_18;
  undefined2 local_14;
  undefined4 local_10;
  
  local_10 = 0x7fff;
  local_1c = 0;
  local_14 = 0xc2;
  local_20 = param_1;
  local_18 = param_1;
  FUN_9d038ccc(&local_20);
  *local_20 = 0;
  return (int)local_20 - (int)param_1;
}

