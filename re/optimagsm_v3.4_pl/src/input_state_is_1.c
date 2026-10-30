// input_state_is_1 @ 9d0018a4
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Czy wejście ma stan 1 (bajt 2 wpisu w tablicy stanów). ❓ naruszenie */

bool input_state_is_1(int param_1)

{
  char cVar1;
  int iVar2;
  
  iVar2 = shared_lock(g_inputs_state);
  cVar1 = *(char *)(iVar2 + param_1 * 4 + 2);
  shared_unlock(g_inputs_state);
  return cVar1 == '\x01';
}

