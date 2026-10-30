// uart1_rx_isr @ 9d0512d8
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Przerwanie RX UART1: przepisuje FIFO do bufora portu 0. */

void uart1_rx_isr(void)

{
  int iVar1;
  undefined1 local_20 [20];
  
  iVar1 = FUN_9d06df88(0x4d);
  if (iVar1 != 0) {
    FUN_9d06df30(0x4d);
    iVar1 = 0;
    while ((*(uint *)(uart_base_table + 0x10) & 1) != 0) {
      local_20[iVar1] = (char)*(undefined4 *)(uart_base_table + 0x30);
      iVar1 = iVar1 + 1;
    }
    if (iVar1 != 0) {
      rb_write_isr(g_uart_rxbuf,local_20,iVar1);
    }
  }
  iVar1 = FUN_9d06df88(0x53);
  if (iVar1 != 0) {
    FUN_9d06df30(0x53);
  }
  return;
}

