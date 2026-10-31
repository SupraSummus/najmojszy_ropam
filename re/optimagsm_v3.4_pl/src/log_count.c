// log_count @ 9d052490
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Liczba rekordów (10 000, gdy pierścień się zawinął). */

undefined2 log_count(void)

{
  char cVar1;
  undefined2 uVar2;
  undefined2 *puVar3;
  
  puVar3 = (undefined2 *)shared_lock(DAT_a0000118);
  uVar2 = *puVar3;
  cVar1 = *(char *)(puVar3 + 1);
  shared_unlock(DAT_a0000118);
  if (cVar1 != '\0') {
    uVar2 = 10000;
  }
  return uVar2;
}

