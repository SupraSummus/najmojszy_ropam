// sys_restart @ 9d05f714
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Restart centrali (FUN_9d062ff8, potem SoftReset); wołany przez komendy 0x1F i 0x2F oraz po
   bezczynności sesji zdalnej. */

void sys_restart(void)

{
  FUN_9d062ff8();
  SoftReset();
  sys_restart();
  return;
}

