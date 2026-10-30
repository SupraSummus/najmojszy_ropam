// rnet_bcast_3c @ 9d01cd4c
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Ramka 0x3C z danymi z FUN_9d01c9bc, zwykle na adres 0xFE. */

void rnet_bcast_3c(undefined1 param_1)

{
  undefined1 auStack_90 [136];
  
  DAT_a0016f11 = 0x3c;
  g_rnet_tx = param_1;
  DAT_a0016f12 = FUN_9d01c9bc(param_1,auStack_90);
  memcpy(&DAT_a0016f13,auStack_90,DAT_a0016f12);
  rnet_send_frame(0,&g_rnet_tx);
  return;
}

