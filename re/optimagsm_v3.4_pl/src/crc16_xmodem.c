// crc16_xmodem @ 9d04e318
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* CRC-16/XMODEM tablicowo (wielomian 0x1021, start 0). */

uint crc16_xmodem(char *param_1,int param_2)

{
  uint uVar1;
  int iVar2;
  char *local_res0;
  
  uVar1 = 0;
  local_res0 = param_1;
  for (iVar2 = 0; iVar2 < param_2; iVar2 = iVar2 + 1) {
    uVar1 = (uint)(ushort)((ushort)(uVar1 << 8) ^
                          *(ushort *)
                           (&crc16_xmodem_table + ((uVar1 >> 8 ^ (int)*local_res0) & 0xff) * 2));
    local_res0 = local_res0 + 1;
  }
  return uVar1;
}

