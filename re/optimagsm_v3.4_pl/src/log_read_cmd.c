// log_read_cmd @ 9d052500
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Komenda serwisowa 0x04: liczba (4 B), potem paczki; argument 1 = cały dziennik, inny = ostatnie
   1000. */

undefined4 log_read_cmd(char param_1)

{
  undefined2 uVar1;
  char cVar2;
  undefined2 *puVar3;
  int local_20;
  int local_1c;
  int local_10 [2];
  
  local_10[0] = log_count();
  puVar3 = (undefined2 *)shared_lock(DAT_a0000118);
  uVar1 = *puVar3;
  shared_unlock(DAT_a0000118);
  if (param_1 == '\x01') {
    svc_transmit(local_10,4);
    local_20 = 0;
    while ((local_20 <= local_10[0] && (cVar2 = log_send_block(uVar1,local_20,100), cVar2 == '\x01')
           )) {
      local_20 = local_20 + 100;
    }
  }
  else {
    if (local_10[0] < 1000) {
      local_1c = 0;
    }
    else {
      local_1c = local_10[0] + -1000;
      local_10[0] = 1000;
    }
    svc_transmit(local_10,4);
    local_20 = local_1c;
    while ((local_20 <= local_10[0] + local_1c &&
           (cVar2 = log_send_block(uVar1,local_20,100), cVar2 == '\x01'))) {
      local_20 = local_20 + 100;
    }
  }
  return 1;
}

