// log_clear @ 9d0519c4
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Kasuje dziennik (komenda 0x05). */

void log_clear(void)

{
  undefined2 *puVar1;
  uint local_10;
  
  for (local_10 = 0; local_10 < 0x139; local_10 = local_10 + 1) {
    FUN_9d055e74(local_10 + 0xdac);
  }
  puVar1 = (undefined2 *)shared_lock(DAT_a0000118);
  *puVar1 = 0;
  *(undefined1 *)(puVar1 + 1) = 0;
  shared_unlock(DAT_a0000118);
  return;
}

