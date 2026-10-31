// log_append @ 9d051a94
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Dopisuje rekord do pierścienia 10 000 rekordów we flashu. */

void log_append(undefined4 param_1)

{
  ushort uVar1;
  ushort *puVar2;
  undefined1 auStack_210 [512];
  undefined4 local_10;
  int local_c;
  
  puVar2 = (ushort *)shared_lock(DAT_a0000118);
  uVar1 = *puVar2;
  FUN_9d051a48((uint)uVar1,&local_10,&local_c);
  if (local_c == 0) {
    FUN_9d056698(local_10,param_1);
  }
  else {
    FUN_9d051a48((uint)uVar1 % 10000,&local_10,&local_c);
    FUN_9d055f1c(local_10,auStack_210);
    memcpy(auStack_210 + local_c * 0x10,param_1,0x10);
    FUN_9d056698(local_10,auStack_210);
  }
  if (*puVar2 < 9999) {
    *puVar2 = *puVar2 + 1;
  }
  else {
    *puVar2 = 0;
    *(undefined1 *)(puVar2 + 1) = 1;
  }
  shared_unlock(DAT_a0000118);
  return;
}

