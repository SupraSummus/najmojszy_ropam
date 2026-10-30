// inputs_set_lines @ 9d00199c
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Wpisuje wartości linii (u16) kolejnych wejść do g_inputs_state, od wejścia o podanym numerze.
    */

void inputs_set_lines(int param_1,int param_2,int param_3)

{
  int iVar1;
  int local_10;
  undefined2 *local_c;
  
  iVar1 = shared_lock(g_inputs_state);
  local_c = (undefined2 *)(iVar1 + param_1 * 4);
  for (local_10 = 0; local_10 < param_2; local_10 = local_10 + 1) {
    *local_c = *(undefined2 *)(param_3 + local_10 * 2);
    local_c = local_c + 2;
  }
  FUN_9d05e4c4(g_inputs_state);
  return;
}

