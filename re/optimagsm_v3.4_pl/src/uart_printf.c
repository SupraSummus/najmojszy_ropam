// uart_printf @ 9d050aa0
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* printf na port (bufor 64 B). */

undefined4 uart_printf(char param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  undefined4 local_res8;
  undefined4 local_resc;
  undefined1 auStack_4c [64];
  undefined4 *local_c;
  
  local_c = &local_res8;
  local_res8 = param_3;
  local_resc = param_4;
  vsprintf(auStack_4c,param_2,local_c);
  FUN_9d0509d4((int)param_1,auStack_4c);
  return 0;
}

