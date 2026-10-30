// uart_init_all @ 9d050f58
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Konfiguracja UART1, 2, 4, 5: 115200 b/s przy 80 MHz, przerwania RX, bufory 512/128/1500/1024 B.
    */

void uart_init_all(void)

{
  FUN_9d06dce0(0,0);
  FUN_9d06e178(0,0);
  FUN_9d06e1a0(0,0);
  UARTSetDataRate(0,80000000,0x1c200);
  FUN_9d06cb10(0,0x87);
  FUN_9d06dca4(0x4d,1);
  FUN_9d06dae4(0x1b,3);
  FUN_9d06db24(0x1b,0);
  FUN_9d06dce0(1,0);
  FUN_9d06e178(1,0);
  FUN_9d06e1a0(1,0);
  UARTSetDataRate(1,80000000,0x1c200);
  FUN_9d06cb10(1,0x87);
  FUN_9d06dca4(0x4e,1);
  FUN_9d06dae4(0x1c,3);
  FUN_9d06db24(0x1c,0);
  FUN_9d06dce0(3,0);
  FUN_9d06e178(3,0);
  FUN_9d06e1a0(3,0);
  UARTSetDataRate(3,80000000,0x1c200);
  FUN_9d06cb10(3,0x87);
  FUN_9d06dca4(0x50,1);
  FUN_9d06dae4(0x1e,3);
  FUN_9d06db24(0x1e,0);
  FUN_9d06dce0(4,0);
  FUN_9d06e178(4,0);
  FUN_9d06e1a0(4,0);
  UARTSetDataRate(4,80000000,0x1c200);
  FUN_9d06cb10(4,0x87);
  FUN_9d06dca4(0x51,1);
  FUN_9d06dae4(0x1f,3);
  FUN_9d06db24(0x1f,0);
  DAT_a0017564 = rb_create(0,0x80,1,0,0);
  DAT_a001756c = rb_create(0,0x400,1,0,0);
  DAT_a0017568 = rb_create(0,0x5dc,1,0,0);
  g_uart_rxbuf = rb_create(0,0x200,1,0,0);
  FUN_9d06dba4(5,8);
  PORTSetBits(5,8);
  return;
}

