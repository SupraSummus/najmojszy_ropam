// code_lookup @ 9d05f088
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Szuka kodu w tablicy 32 użytkowników (34 B na wpis); zwraca numer, 100 dla kodu serwisowego,
   0xFF gdy brak. */

uint code_lookup(undefined4 param_1,undefined1 *param_2)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  uint local_18;
  
  local_18 = 0;
  do {
    if (0x1f < (int)local_18) {
      iVar2 = FUN_9d06a74c(&g_service_code4);
      if ((iVar2 == 4) && (iVar2 = FUN_9d06b168(&g_service_code4,param_1), iVar2 == 0)) {
        uVar3 = 100;
      }
      else {
        uVar3 = 0xff;
      }
      return uVar3;
    }
    if ((*(char *)((int)&g_users + local_18 * 0x22) != '\0') &&
       (iVar2 = FUN_9d06b168((int)&g_users + local_18 * 0x22,param_1), iVar2 == 0)) {
      iVar2 = FUN_9d06a74c(param_1);
      iVar1 = FUN_9d06a74c((int)&g_users + local_18 * 0x22);
      if (iVar2 == iVar1) {
        *param_2 = (&DAT_a00162f1)[local_18 * 0x22];
        return local_18 & 0xff;
      }
    }
    local_18 = local_18 + 1;
  } while( true );
}

