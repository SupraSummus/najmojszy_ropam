// rnet_send_frame @ 9d052f50
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Wysyła ramkę [adres][cmd][len][dane][CRC_hi][CRC_lo], czeka na bajt ACK (3 = OK, 1 = zły CRC,
   brak = 2), do 3 prób. */

undefined4 rnet_send_frame(char param_1,char *param_2)

{
  undefined2 uVar1;
  byte local_10;
  char local_f;
  char local_e;
  
  local_e = '\0';
  uVar1 = rnet_crc16(param_2);
  *(undefined2 *)(param_2 + 0xf4) = uVar1;
  uart_rx_flush((int)param_1);
  do {
    if (param_1 == '\0') {
      PORTSetBits(5,8);
    }
    uart_putc((int)param_1,(int)*param_2);
    uart_putc((int)param_1,(int)param_2[1]);
    uart_putc((int)param_1,(int)param_2[2]);
    if ((param_2[2] != -0x56) && (param_2[2] != -1)) {
      for (local_10 = 0; local_10 < (byte)param_2[2]; local_10 = local_10 + 1) {
        uart_putc((int)param_1,(int)param_2[local_10 + 3]);
      }
    }
    uart_putc((int)param_1,(int)(char)((ushort)*(undefined2 *)(param_2 + 0xf4) >> 8));
    uart_putc((int)param_1,(int)(char)*(undefined2 *)(param_2 + 0xf4));
    if (param_1 == '\0') {
      PORTClearBits(5,8);
    }
    uart_rx_flush((int)param_1);
    if (*param_2 == -2) {
      return 1;
    }
    local_f = rnet_read_ack((int)param_1);
    if ((local_f == '\x02') && (*param_2 == -0x20)) {
      local_f = rnet_read_ack((int)param_1);
    }
    if (local_f == '\x02') {
      g_rnet_cnt_noack = g_rnet_cnt_noack + 1;
      if (2 < (byte)(local_e + 1U)) {
        return 0;
      }
    }
    else {
      if (local_f == '\x03') {
        g_rnet_cnt_ok = g_rnet_cnt_ok + 1;
        return 1;
      }
      if (local_f != '\x01') {
        return 0;
      }
      g_rnet_cnt_badcrc = g_rnet_cnt_badcrc + 1;
      if (2 < (byte)(local_e + 1U)) {
        return 0;
      }
    }
    local_e = local_e + '\x01';
  } while( true );
}

