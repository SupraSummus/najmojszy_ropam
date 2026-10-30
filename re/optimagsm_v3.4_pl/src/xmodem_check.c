// xmodem_check @ 9d04e640
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Sprawdza CRC-16 albo sumę 8-bitową bloku. */

undefined4 xmodem_check(int param_1,int param_2,int param_3)

{
  short sVar1;
  int local_18;
  char local_14;
  
  if (param_1 == 0) {
    local_14 = '\0';
    for (local_18 = 0; local_18 < param_3; local_18 = local_18 + 1) {
      local_14 = *(char *)(param_2 + local_18) + local_14;
    }
    if (local_14 == *(char *)(param_2 + param_3)) {
      return 1;
    }
  }
  else {
    sVar1 = crc16_xmodem(param_2,param_3);
    if (sVar1 == (ushort)((ushort)*(byte *)(param_2 + param_3) * 0x100 +
                         (ushort)*(byte *)(param_2 + param_3 + 1))) {
      return 1;
    }
  }
  return 0;
}

