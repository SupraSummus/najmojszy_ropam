// event_log_ex @ 9d052120
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Zapis zdarzenia: czas (6 B z zegara), kod, argument, drugi argument; rekord 16 B. */

void event_log_ex(undefined1 param_1,undefined1 param_2,undefined1 param_3)

{
  undefined1 local_20;
  undefined1 local_1f;
  undefined1 local_1e;
  undefined1 local_1d;
  undefined1 local_1c;
  undefined1 local_1b;
  undefined1 local_1a;
  undefined1 local_18;
  undefined1 local_17;
  undefined1 local_10;
  undefined1 local_f;
  undefined1 local_e;
  undefined1 local_d;
  undefined1 local_c;
  undefined1 local_b;
  
  rtc_get(&local_10);
  local_20 = local_10;
  local_1f = local_f;
  local_1e = local_e;
  local_1c = local_c;
  local_1b = local_b;
  local_1d = local_d;
  local_1a = param_1;
  local_18 = param_2;
  local_17 = param_3;
  log_append(&local_20);
  return;
}

