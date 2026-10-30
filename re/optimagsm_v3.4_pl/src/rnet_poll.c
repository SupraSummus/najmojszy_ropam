// rnet_poll @ 9d01ce74
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Odpytanie urządzenia: [adres][0x2B][0]. */

void rnet_poll(undefined1 param_1)

{
  DAT_a0016f11 = 0x2b;
  DAT_a0016f12 = 0;
  g_rnet_tx = param_1;
  rnet_send_frame(0,&g_rnet_tx);
  return;
}

