// hub_cmd_f3 @ 9d04f740
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Ramka 0xB0/0xF3, 32 B. */

void hub_cmd_f3(undefined4 param_1)

{
  g_rnet_tx = 0xb0;
  DAT_a0016f11 = 0xf3;
  DAT_a0016f12 = 0x20;
  memcpy(&DAT_a0016f13,param_1,0x20);
  rnet_send_frame(0,&g_rnet_tx);
  return;
}

