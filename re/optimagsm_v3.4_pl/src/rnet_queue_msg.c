// rnet_queue_msg @ 9d05366c
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Wysyła do paneli komunikat (np. 0x30) ❓. */

void rnet_queue_msg(undefined1 param_1,undefined4 param_2)

{
  rnet_build_frame(&g_rnet_tx,0xfe,param_1,param_2,0x1f);
  rnet_send_frame(0,&g_rnet_tx);
  return;
}

