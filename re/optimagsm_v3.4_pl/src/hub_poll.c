// hub_poll @ 9d04fabc
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* Cykliczna wymiana stanu 8 modułów IO/IOE z koncentratorem. */

char hub_poll(void)

{
  uint uVar1;
  undefined1 local_148 [232];
  int local_60;
  int local_5c;
  int local_58;
  char local_54;
  byte local_53;
  int local_50;
  uint auStack_4c [17];
  
  local_60 = 0;
  local_5c = 0;
  local_53 = 0;
  if (DAT_a0000003 != '\0') {
    DAT_a0000003 = DAT_a0000003 + -1;
  }
  if (((_DAT_a000015a & 3) >> 1 == 0) && (DAT_a00000b8 = DAT_a00000b8 + 1, (DAT_a00000b8 & 3) != 0))
  {
    if (DAT_a0000003 == '\0') {
      for (local_60 = 0; local_60 < 8; local_60 = local_60 + 1) {
        uVar1 = FUN_9d008a78((uint)DAT_a0015a0a + local_60);
        auStack_4c[local_60] = uVar1;
        if ((auStack_4c[local_60] == (uint)(byte)(&DAT_a0001fc2)[local_60 * 10]) ||
           ((&DAT_a0001fc2)[local_60 * 10] == -1)) {
          (&DAT_a00000b0)[local_60] = 0;
        }
        else {
          if (auStack_4c[local_60] == 0) {
            (&DAT_a00000b0)[local_60] = 1;
          }
          else {
            (&DAT_a00000b0)[local_60] = 2;
          }
          DAT_a0000003 = '\0';
        }
      }
    }
    local_54 = hub_cmd_f1(&DAT_a00000b0,&DAT_a0001fbc);
    if (local_54 != '\0') {
      DAT_a0015a04 = DAT_a0015a04 & 0xfb | ((DAT_a0001fbe & 1) != 0) << 2;
      if ((DAT_a0015a04 & 7) >> 2 != 0) {
        for (uVar1 = 0; uVar1 < 0xe6; uVar1 = uVar1 + 1) {
          local_148[uVar1] = (&DAT_a0017018)[uVar1];
        }
        rnet_debug_dump(g_rnet_rx,DAT_a001700c,DAT_a0017010,DAT_a0017014);
      }
      if (DAT_a0000003 != '\0') {
        for (local_5c = 0; local_5c < 8; local_5c = local_5c + 1) {
          (&DAT_a00000b0)[local_5c] = 0;
        }
      }
      for (local_60 = 0; local_60 < 8; local_60 = local_60 + 1) {
        if (((&DAT_a0001fc2)[local_60 * 10] == '\x01') && (auStack_4c[local_60] == 0)) {
          FUN_9d0087c4((uint)DAT_a0015a0a + local_60);
        }
        else if (((&DAT_a0001fc2)[local_60 * 10] == '\0') && (auStack_4c[local_60] == 1)) {
          FUN_9d008838((uint)DAT_a0015a0a + local_60);
        }
        if (((&DAT_a0001fc0)[local_60 * 10] != -1) && (((&DAT_a0001fc1)[local_60 * 10] & 4) == 0)) {
          local_53 = 1;
        }
        if (((&DAT_a0001fc0)[local_60 * 10] == -1) ||
           (((&DAT_a0001fc2)[local_60 * 10] != -1 && (((&DAT_a0001fc1)[local_60 * 10] & 4) != 0))))
        {
          local_58 = 0;
        }
        else {
          local_58 = 1;
        }
        if (local_58 == 0) {
          DAT_a0000094 = ~(1 << ((uint)DAT_a0015a0a + local_60 & 0x1f)) & DAT_a0000094;
        }
        else {
          DAT_a0000094 = local_58 << ((uint)DAT_a0015a0a + local_60 & 0x1f) | DAT_a0000094;
        }
      }
      for (local_60 = 0; local_60 < 8; local_60 = local_60 + 1) {
        if ((&DAT_a0001fc5)[local_60 * 10] == -0x5f) {
          FUN_9d037010(&DAT_9d06e600,local_60,2);
        }
        else if ((&DAT_a0001fc5)[local_60 * 10] == -0x60) {
          FUN_9d037010(&DAT_9d06e600,local_60,1);
        }
        else {
          FUN_9d037010(&DAT_9d06e600,local_60,0);
        }
        FUN_9d037010(&DAT_9d06e604,local_60,*(undefined2 *)(&DAT_a0001fc6 + local_60 * 10));
        FUN_9d037010(&DAT_9d06e608,local_60,((&DAT_a0001fc1)[local_60 * 10] & 0x10) != 0);
      }
    }
  }
  else {
    for (local_60 = 0; local_60 < 8; local_60 = local_60 + 1) {
      uVar1 = FUN_9d008a78((uint)DAT_a0015a0a + local_60);
      auStack_4c[local_60] = uVar1;
      if ((auStack_4c[local_60] == (uint)(byte)(&DAT_a0001f05)[local_60 * 0x14]) ||
         ((&DAT_a0001f05)[local_60 * 0x14] == -1)) {
        (&DAT_a00000b0)[local_60] = 0;
      }
      else {
        if (auStack_4c[local_60] == 0) {
          (&DAT_a00000b0)[local_60] = 1;
        }
        else {
          (&DAT_a00000b0)[local_60] = 2;
        }
        DAT_a0000003 = '\0';
      }
    }
    local_54 = hub_cmd_f0(&DAT_a00000b0,&DAT_a0001ef8);
    if (local_54 != '\0') {
      DAT_a0015a04 = DAT_a0015a04 & 0xfb | ((DAT_a0001efa & 1) != 0) << 2;
      for (local_60 = 0; local_60 < 8; local_60 = local_60 + 1) {
        if (((&DAT_a0001f05)[local_60 * 0x14] == '\x01') && (auStack_4c[local_60] == 0)) {
          FUN_9d0087c4((uint)DAT_a0015a0a + local_60);
        }
        else if (((&DAT_a0001f05)[local_60 * 0x14] == '\0') && (auStack_4c[local_60] == 1)) {
          FUN_9d008838((uint)DAT_a0015a0a + local_60);
        }
        if ((&DAT_a0001f0a)[local_60 * 0x14] == -0x5f) {
          FUN_9d037010(&DAT_9d06e600,local_60,2);
        }
        else if ((&DAT_a0001f0a)[local_60 * 0x14] == -0x60) {
          FUN_9d037010(&DAT_9d06e600,local_60,1);
        }
        else {
          FUN_9d037010(&DAT_9d06e600,local_60,0);
        }
        if (((&DAT_a0001efd)[local_60 * 0x14] != -1) &&
           (((&DAT_a0001efe)[local_60 * 0x14] & 4) == 0)) {
          local_53 = 1;
        }
        if (((&DAT_a0001efd)[local_60 * 0x14] == -1) ||
           (((&DAT_a0001f05)[local_60 * 0x14] != -1 && (((&DAT_a0001efe)[local_60 * 0x14] & 4) != 0)
            ))) {
          local_58 = 0;
        }
        else {
          local_58 = 1;
        }
        if (local_58 == 0) {
          DAT_a0000094 = ~(1 << ((uint)DAT_a0015a0a + local_60 & 0x1f)) & DAT_a0000094;
        }
        else {
          DAT_a0000094 = local_58 << ((uint)DAT_a0015a0a + local_60 & 0x1f) | DAT_a0000094;
        }
        FUN_9d037010(&DAT_9d06e604,local_60,*(undefined2 *)(&DAT_a0001f08 + local_60 * 0x14));
        FUN_9d037010(&DAT_9d06e608,local_60,((&DAT_a0001efe)[local_60 * 0x14] & 0x10) != 0);
      }
    }
  }
  if (local_54 == '\0') {
    for (local_60 = 0; local_60 < 8; local_60 = local_60 + 1) {
      DAT_a0000094 = 1 << ((uint)DAT_a0015a0a + local_60 & 0x1f) | DAT_a0000094;
    }
  }
  else {
    local_50 = FUN_9d05e388(DAT_a00001b0);
    if (DAT_a0015a74 < 0) {
      *(byte *)(local_50 + 1) = *(byte *)(local_50 + 1) & 0xbf;
    }
    else {
      *(byte *)(local_50 + 1) = *(byte *)(local_50 + 1) & 0xbf | (local_53 & 1) << 6;
    }
    FUN_9d05e3b8(DAT_a00001b0);
  }
  return local_54;
}

