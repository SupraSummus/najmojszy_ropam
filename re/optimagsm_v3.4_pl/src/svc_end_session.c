// svc_end_session @ 9d05f714
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Zakończenie sesji: używane przez komendy 0x26/0x2F, kończy się SoftReset. */

void svc_end_session(void)

{
  FUN_9d062ff8();
  SoftReset();
  svc_end_session();
  return;
}

