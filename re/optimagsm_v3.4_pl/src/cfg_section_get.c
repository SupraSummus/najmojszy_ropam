// cfg_section_get @ 9d04c218
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Kopiuje sekcję konfiguracji o podanej nazwie do struktury w RAM. */

undefined4 cfg_section_get(undefined4 param_1,undefined4 param_2)

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
  memcpy(param_2,&DAT_a0008696 + *(ushort *)(&DAT_a0008516 + local_10 * 0xe),
         *(undefined2 *)(&DAT_a0008518 + local_10 * 0xe));
  return 1;
}

