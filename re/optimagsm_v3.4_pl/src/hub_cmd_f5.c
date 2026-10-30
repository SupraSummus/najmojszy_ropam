// hub_cmd_f5 @ 9d04f8f8
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Ramka 0xB0/0xF5, 2 B. */

void hub_cmd_f5(char param_1)

{
  g_rnet_tx = 0xb0;
  DAT_a0016f11 = 0xf5;
  DAT_a0016f12 = 2;
  DAT_a0016f13 = param_1 == -1;
  DAT_a0016f14 = param_1;
  rnet_send_frame(0,&g_rnet_tx);
  return;
}

