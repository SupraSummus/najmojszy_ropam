// rnet_bcast_status @ 9d01cde0
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Ramka 0x10 z 220-bajtowym nagłówkiem stanu (status_header), zwykle na adres 0xFE. */

void rnet_bcast_status(undefined1 param_1)

{
  undefined1 auStack_f8 [240];
  
  DAT_a0016f11 = 0x10;
  g_rnet_tx = param_1;
  DAT_a0016f12 = status_header(param_1,auStack_f8);
  memcpy(&DAT_a0016f13,auStack_f8,DAT_a0016f12);
  rnet_send_frame(0,&g_rnet_tx);
  return;
}

