// rnet_crc16_update @ 9d052c94
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Krok CRC dla jednego bajtu. */

void rnet_crc16_update(byte param_1,ushort *param_2)

{
  undefined1 local_10;
  
  *param_2 = *param_2 ^ (ushort)param_1;
  for (local_10 = 0; local_10 < 8; local_10 = local_10 + 1) {
    *param_2 = *param_2 >> 1;
    if ((*param_2 & 1) != 0) {
      *param_2 = *param_2 ^ 0xa001;
    }
  }
  return;
}

