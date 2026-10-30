// rnet_recv_frame @ 9d0532dc
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Odbiera odpowiedź zaczynającą się od 0xAA, sprawdza CRC i odsyła ACK (3) albo NAK (1). */

undefined4 rnet_recv_frame(char param_1,char *param_2)

{
  char cVar1;
  undefined1 uVar2;
  undefined1 uVar3;
  int iVar4;
  uint uVar5;
  byte bVar6;
  byte local_18;
  byte local_17;
  byte local_16;
  
  local_16 = 0;
  memset(param_2 + 3,0,100);
  local_17 = 0;
  do {
    if (2 < local_17) {
      return 0;
    }
    do {
      do {
        do {
          bVar6 = local_16 + 1;
          if (5 < local_16) {
            return 0;
          }
          if (param_1 == '\0') {
            PORTClearBits(5,8);
            uart_wait_bytes(0,4,100);
          }
          else {
            uart_wait_bytes((int)param_1,6,0x14);
          }
          *param_2 = '\0';
          do {
            cVar1 = rnet_read_byte((int)param_1);
            *param_2 = cVar1;
            if (*param_2 == -0x56) break;
            iVar4 = uart_rx_count((int)param_1);
          } while (iVar4 != 0);
          local_16 = bVar6;
          if (*param_2 != -0x56) goto LAB_9d0535a4;
          cVar1 = rnet_read_byte((int)param_1);
          param_2[1] = cVar1;
          cVar1 = rnet_read_byte((int)param_1);
          param_2[2] = cVar1;
        } while ((g_rnet_timeout != '\0') || (200 < (byte)param_2[2]));
        for (local_18 = 0; local_18 < (byte)param_2[2]; local_18 = local_18 + 1) {
          cVar1 = rnet_read_byte((int)param_1);
          param_2[local_18 + 3] = cVar1;
        }
      } while (g_rnet_timeout != '\0');
      uVar2 = rnet_read_byte((int)param_1);
      uVar3 = rnet_read_byte((int)param_1);
    } while (g_rnet_timeout != '\0');
    *(ushort *)(param_2 + 0xf4) = CONCAT11(uVar2,uVar3);
    if (param_1 == '\0') {
      busy_wait_t1(300);
      PORTSetBits(5,8);
    }
    uVar5 = rnet_crc16(param_2);
    if (uVar5 == *(ushort *)(param_2 + 0xf4)) {
      uart_putc((int)param_1,3);
      if (param_1 == '\0') {
        busy_wait_t1(200);
        PORTClearBits(5,8);
      }
      return 1;
    }
    uart_putc((int)param_1,1);
    g_rnet_cnt_badcrc = g_rnet_cnt_badcrc + 1;
    if (param_1 == '\0') {
      busy_wait_t1(200);
      PORTClearBits(5,8);
    }
LAB_9d0535a4:
    local_17 = local_17 + 1;
  } while( true );
}

