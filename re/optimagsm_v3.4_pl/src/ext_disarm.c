// ext_disarm @ 9d0192fc
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Komenda 0x37: rozbrojenie stref w sesji ❓. */

undefined4 ext_disarm(char *param_1,char *param_2)

{
  char cVar1;
  byte bVar2;
  char cVar3;
  char cVar4;
  char cVar5;
  byte bVar6;
  int iVar7;
  int iVar8;
  char local_18;
  byte local_e [6];
  
  local_18 = '\0';
  cVar1 = *param_1;
  iVar7 = (int)cVar1;
  iVar8 = FUN_9d0171ec((int)cVar1,param_1 + 5);
  if (iVar8 == 0) {
    local_18 = '\x03';
    (&DAT_a000207a)[iVar7 * 0x27] = (&DAT_a000207a)[iVar7 * 0x27] + '\x01';
    if ('\x03' < (char)(&DAT_a000207a)[iVar7 * 0x27]) {
      local_18 = '\x04';
    }
  }
  else {
    (&DAT_a000207a)[iVar7 * 0x27] = 0;
    bVar2 = param_1[1];
    cVar3 = param_1[2];
    cVar4 = param_1[3];
    cVar5 = param_1[4];
    bVar6 = code_lookup(iVar7 * 0x27 + -0x5fffdf8b,local_e);
    if (bVar6 < 100) {
      FUN_9d0049c4(4,bVar6,(int)(char)(local_e[0] & (bVar2 | cVar3 << 1 | cVar4 << 2 | cVar5 << 3)),
                   1);
    }
    else {
      local_18 = '\x03';
    }
  }
  *param_2 = cVar1;
  param_2[1] = local_18;
  return 2;
}

