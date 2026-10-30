// hub_cmd_f2 @ 9d04f7bc
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Ramka 0xB0/0xF2, 4 B. */

void hub_cmd_f2(undefined1 param_1,undefined4 param_2)

{
  undefined1 local_res4;
  
  g_rnet_tx = 0xb0;
  DAT_a0016f11 = 0xf2;
  DAT_a0016f12 = 4;
  local_res4 = (undefined1)param_2;
  DAT_a0016f14 = local_res4;
  DAT_a0016f15 = (undefined1)((uint)param_2 >> 0x18);
  DAT_a0016f16 = (undefined1)((uint)param_2 >> 0x10);
  DAT_a0016f13 = param_1;
  rnet_send_frame(0,&g_rnet_tx);
  return;
}

