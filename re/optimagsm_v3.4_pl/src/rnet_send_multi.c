// rnet_send_multi @ 9d0536d0
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Wysyła dłuższy blok w wielu ramkach. */

bool rnet_send_multi(char param_1,undefined1 param_2,undefined1 param_3,int param_4,ushort param_5)

{
  char cVar1;
  undefined1 uVar2;
  int iVar3;
  byte local_20;
  ushort local_1e;
  ushort local_1c;
  
  local_20 = 0;
  DAT_a0016f11 = 0x21;
  cVar1 = (char)param_5 + ((byte)((ulonglong)(param_5 >> 2) * 0x16c16c17 >> 0x20) & 0xfc) * -0x2d;
  local_1c = 0;
  local_1e = 0;
  g_rnet_tx = param_2;
  DAT_a0016f13 = param_3;
  while( true ) {
    uVar2 = (undefined1)(local_1c >> 8);
    if ((param_5 / 0xb4 <= local_1e) || (2 < local_20)) break;
    DAT_a0016f16 = -0x4c;
    DAT_a0016f12 = -0x48;
    DAT_a0016f14 = (char)local_1c;
    DAT_a0016f15 = uVar2;
    memcpy(&DAT_a0016f17,param_4 + (uint)local_1c,0xb4);
    iVar3 = rnet_send_frame((int)param_1,&g_rnet_tx);
    if (iVar3 == 0) {
      local_20 = local_20 + 1;
    }
    else {
      local_1c = local_1c + 0xb4;
      local_20 = 0;
    }
    local_1e = local_1e + 1;
  }
  if ((cVar1 != '\0') && (local_20 < 3)) {
    DAT_a0016f12 = cVar1 + '\x04';
    DAT_a0016f14 = (char)local_1c;
    DAT_a0016f15 = uVar2;
    DAT_a0016f16 = cVar1;
    memcpy(&DAT_a0016f17,param_4 + (uint)local_1c,cVar1);
    iVar3 = rnet_send_frame((int)param_1,&g_rnet_tx);
    if (iVar3 != 0) {
      local_20 = 0;
    }
  }
  return local_20 == 0;
}

