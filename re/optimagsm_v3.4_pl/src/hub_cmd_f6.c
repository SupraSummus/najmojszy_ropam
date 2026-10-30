// hub_cmd_f6 @ 9d04f994
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Ramka 0xB0/0xF6, 6 B. */

void hub_cmd_f6(undefined4 param_1)

{
  g_rnet_tx = 0xb0;
  DAT_a0016f11 = 0xf6;
  DAT_a0016f12 = 6;
  memcpy(&DAT_a0016f13,param_1,6);
  rnet_send_frame(0,&g_rnet_tx);
  return;
}

