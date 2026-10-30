// cfg_section_put @ 9d04c030
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Kopiuje strukturę z RAM do sekcji konfiguracji (limit 52 000 B danych). */

undefined4 cfg_section_put(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  int local_10;
  
  local_10 = 0;
  while( true ) {
    if (g_cfg_nsections <= local_10) {
      return 0;
    }
    iVar1 = FUN_9d06b168(param_1,local_10 * 0xe + -0x5fff7af2);
    if (iVar1 == 0) break;
    local_10 = local_10 + 1;
  }
  if ((uint)*(ushort *)(&DAT_a0008516 + local_10 * 0xe) +
      (uint)*(ushort *)(&DAT_a0008518 + local_10 * 0xe) < 52000) {
    memcpy(&DAT_a0008696 + *(ushort *)(&DAT_a0008516 + local_10 * 0xe),param_2,
           *(undefined2 *)(&DAT_a0008518 + local_10 * 0xe));
    return 1;
  }
  return 0;
}

