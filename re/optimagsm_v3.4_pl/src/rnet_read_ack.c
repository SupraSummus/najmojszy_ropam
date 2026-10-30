// rnet_read_ack @ 9d052c18
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Czeka na bajt potwierdzenia; brak = 2. */

undefined1 rnet_read_ack(char param_1)

{
  int iVar1;
  undefined4 local_10;
  undefined1 local_c [4];
  
  local_10 = 0x32;
  if (param_1 == '\x01') {
    local_10 = 0xf;
  }
  iVar1 = uart_read((int)param_1,local_c,1,local_10);
  if (iVar1 == 0) {
    local_c[0] = 2;
  }
  return local_c[0];
}

