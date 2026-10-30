// rnet_crc16 @ 9d052d48
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* CRC ramki: start 0xFFFF, wielomian 0xA001, bit testowany po przesunięciu (niestandardowe). */

undefined2 rnet_crc16(undefined1 *param_1)

{
  ushort local_10;
  undefined2 local_e [3];
  
  local_e[0] = 0xffff;
  rnet_crc16_update(*param_1,local_e);
  rnet_crc16_update(param_1[1],local_e);
  rnet_crc16_update(param_1[2],local_e);
  if ((param_1[2] != -0x56) && (param_1[2] != -1)) {
    for (local_10 = 0; local_10 < (byte)param_1[2]; local_10 = local_10 + 1) {
      rnet_crc16_update(param_1[local_10 + 3],local_e);
    }
  }
  return local_e[0];
}

