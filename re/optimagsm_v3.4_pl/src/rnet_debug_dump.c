// rnet_debug_dump @ 9d052b04
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Wypisuje odebraną ramkę na port 1 ("Rxframe adres:%x cmd:%x len:%x"). */

void rnet_debug_dump(uint param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  undefined1 uVar1;
  undefined4 local_res0;
  undefined4 local_res4;
  undefined4 local_res8;
  undefined4 local_resc;
  undefined2 in_stack_000000f4;
  int local_10;
  
  local_res0._1_1_ = (undefined1)(param_1 >> 8);
  uVar1 = local_res0._1_1_;
  local_res0 = param_1;
  local_res4 = param_2;
  local_res8 = param_3;
  local_resc = param_4;
  uart_printf(1,s_Rxframe_adres__x_cmd__x_len__x_c_9d06dd94,param_1 & 0xff,uVar1,
              param_1 >> 0x10 & 0xff,in_stack_000000f4);
  for (local_10 = 0; local_10 < (int)(local_res0 >> 0x10 & 0xff); local_10 = local_10 + 1) {
    uart_printf(1,&DAT_9d06ddc8,*(undefined1 *)((int)&local_res0 + local_10 + 3));
  }
  return;
}

