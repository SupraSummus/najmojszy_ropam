// ext_query_41 @ 9d0199a4
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Komenda 0x41: odpowiada 1 B, 1 gdy bit 1 flag pod 0xA000277C zgaszony ❓. */

undefined4 ext_query_41(undefined4 param_1,undefined1 *param_2)

{
  if (((byte)DAT_a000277c & 3) >> 1 == 0) {
    *param_2 = 1;
  }
  else {
    *param_2 = 0;
  }
  return 1;
}

