// aero_read_dc @ 9d030b30
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* AP-AERO: 0xDC (1 B) → 64 B do 0xA0001234 (opis modułu ❓). */

undefined4 aero_read_dc(void)

{
  int iVar1;
  undefined4 uVar2;
  undefined1 auStack_c [4];
  
  rnet_build_frame(&g_rnet_tx,DAT_a00159e2,0xdc,auStack_c,1);
  uart_rx_flush(0);
  iVar1 = rnet_send_frame(0,&g_rnet_tx);
  if (((iVar1 == 0) || (iVar1 = rnet_recv_frame(0,&g_rnet_rx), iVar1 == 0)) ||
     (g_rnet_rx._1_1_ != -0x24)) {
    uVar2 = 0;
  }
  else {
    memcpy(&DAT_a0001234,0xa001700b,0x40);
    uVar2 = 1;
  }
  return uVar2;
}

