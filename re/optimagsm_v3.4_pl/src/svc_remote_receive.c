// svc_remote_receive @ 9d04f1a4
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Odbiór przez modem: 'S', długość (2 B), dane, suma. ❓ szyfrowanie */

uint svc_remote_receive(int param_1,int param_2)

{
  char cVar1;
  undefined1 uVar2;
  undefined1 uVar3;
  short sVar4;
  uint uVar5;
  int local_28;
  int local_24;
  undefined2 local_1f;
  undefined2 local_18;
  undefined4 local_14;
  undefined4 local_10;
  
  local_24 = 0;
  local_14 = DAT_9d069180;
  local_10 = DAT_9d069184;
  svc_flush();
  local_28 = 0;
  while( true ) {
    if (3 < local_28) {
      return 0xffffffff;
    }
    FUN_9d04f104();
    cVar1 = svc_getc(2000);
    if ((DAT_a0000078 == 0) && (cVar1 == 'S')) break;
    local_28 = local_28 + 1;
  }
  uVar2 = svc_getc(2000);
  uVar3 = svc_getc(2000);
  local_1f = CONCAT11(uVar3,uVar2);
  uVar5 = (uint)local_1f;
  if ((DAT_a0000078 == 1) || (param_2 < (int)uVar5)) {
    return 0xffffffff;
  }
  for (local_28 = 0; local_28 < (int)uVar5; local_28 = local_28 + 1) {
    uVar2 = svc_getc(2000);
    if (DAT_a0000078 == 0) {
      *(undefined1 *)(param_1 + local_28) = uVar2;
    }
    else {
      local_24 = local_24 + 1;
      if (10 < local_24) break;
      local_28 = local_28 + -1;
    }
  }
  if (local_24 < 0xb) {
    uVar2 = svc_getc(2000);
    uVar3 = svc_getc(2000);
    local_18 = CONCAT11(uVar3,uVar2);
    sVar4 = crc16_xmodem(param_1,uVar5);
    if (sVar4 == local_18) {
      FUN_9d062524(param_1,uVar5,&DAT_a0015d35,&local_14);
      return uVar5;
    }
  }
  return 0xffffffff;
}

