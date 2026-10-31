// log_send_block @ 9d051dc8
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Wysyła XMODEM-em paczkę 100 rekordów po 16 B. */

bool log_send_block(int param_1,uint param_2,int param_3)

{
  char cVar1;
  int iVar2;
  int local_20;
  uint local_1c;
  undefined4 local_10;
  int local_c;
  
  local_20 = 0;
  iVar2 = shared_lock(DAT_a0000118);
  cVar1 = *(char *)(iVar2 + 2);
  shared_unlock(DAT_a0000118);
  for (local_1c = param_2; local_1c < param_2 + param_3; local_1c = local_1c + 1) {
    if (cVar1 == '\0') {
      FUN_9d051a48(local_1c,&local_10,&local_c);
    }
    else {
      FUN_9d051a48((local_1c + param_1) % 10000,&local_10,&local_c);
    }
    FUN_9d056570(local_10,local_c << 4,&g_svc_buf + local_20 * 0x10,0x10);
    local_20 = local_20 + 1;
  }
  iVar2 = svc_transmit(&g_svc_buf,param_3 << 4);
  return -1 < iVar2;
}

