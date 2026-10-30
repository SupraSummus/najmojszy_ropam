// svc_stream_to_modem @ 9d01b2b8
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Pobiera z portu 2 bloki ('s', długość 2 B, dane) i przekazuje je DMA do modemu; na końcu 'e'.
    */

undefined4 svc_stream_to_modem(void)

{
  int iVar1;
  int local_18;
  ushort local_10 [4];
  
  local_18 = 0;
  local_10[0] = 0;
  do {
    if (9 < local_18) break;
    uart_rx_flush(2);
    uart_putc(2,0x73);
    iVar1 = uart_read(2,local_10,2,2000);
    if (iVar1 == 2) {
      if ((local_10[0] < 0xc351) && (local_10[0] != 0)) {
        FUN_9d06dca4(0x50,0);
        dma_uart4_rx_start(&DAT_a0008696,local_10[0]);
        os_delay(2000);
        dma_uart_tx(3,&DAT_a0008696,local_10[0],6000);
        FUN_9d06ddcc(0);
        FUN_9d06dca4(0x61,0);
        FUN_9d06dca4(0x50,1);
      }
    }
    else {
      local_18 = local_18 + 1;
    }
  } while (local_10[0] != 0);
  uart_putc(2,0x65);
  return 1;
}

