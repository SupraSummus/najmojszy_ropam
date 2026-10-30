// hub_cmd_f4 @ 9d04f864
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Ramka 0xB0/0xF4, 1 B. */

undefined1 hub_cmd_f4(char param_1)

{
  undefined1 uVar1;
  
  g_rnet_tx = 0xb0;
  DAT_a0016f11 = 0xf4;
  DAT_a0016f12 = 1;
  DAT_a0016f13 = param_1;
  uVar1 = rnet_send_frame(0,&g_rnet_tx);
  if (param_1 != '\x01') {
    uVar1 = 1;
  }
  return uVar1;
}

