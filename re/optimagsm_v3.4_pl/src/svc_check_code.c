// svc_check_code @ 9d03c374
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Porównuje 6 bajtów z hasłem komunikacji z konfiguracji. */

undefined4 svc_check_code(int param_1)

{
  int local_10;
  
  local_10 = 0;
  while( true ) {
    if (5 < local_10) {
      return 1;
    }
    if (*(char *)(param_1 + local_10) != (&g_service_code)[local_10]) break;
    local_10 = local_10 + 1;
  }
  return 0;
}

