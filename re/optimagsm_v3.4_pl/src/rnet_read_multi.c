// rnet_read_multi @ 9d05393c
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Odczyt bloku w wielu ramkach (kontynuacja: komenda '!'). */

undefined1 rnet_read_multi(char param_1,int param_2,int param_3)

{
  undefined1 uVar1;
  byte bVar2;
  bool bVar3;
  int iVar4;
  byte local_14;
  byte local_13;
  
  local_14 = 0;
  local_13 = 0;
  while( true ) {
    while (iVar4 = rnet_recv_frame((int)param_1,param_3), iVar4 == 0) {
      bVar3 = 3 < local_13;
      local_13 = local_13 + 1;
      if (bVar3) {
        return 0;
      }
    }
    local_14 = local_14 + 1;
    local_13 = 0;
    uVar1 = *(undefined1 *)(param_3 + 3);
    bVar2 = *(byte *)(param_3 + 6);
    memcpy(param_2 + (uint)*(ushort *)(param_3 + 4),param_3 + 7,bVar2);
    if (100 < local_14) {
      return uVar1;
    }
    if (200 < bVar2) break;
    if (bVar2 < 0xb4) {
      return uVar1;
    }
    if (*(char *)(param_3 + 1) != '!') {
      return uVar1;
    }
  }
  return uVar1;
}

