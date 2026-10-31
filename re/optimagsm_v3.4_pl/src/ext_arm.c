// ext_arm @ 9d019034
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Komenda 0x36: uzbrojenie stref w otwartej sesji (strefy ∧ uprawnienia kodu, tryb nocny). */

undefined4 ext_arm(char *param_1,char *param_2)

{
  char cVar1;
  char cVar2;
  char cVar3;
  char cVar4;
  char cVar5;
  byte bVar6;
  byte bVar7;
  int iVar8;
  int iVar9;
  char local_18;
  byte local_c [4];
  
  cVar1 = *param_1;
  iVar8 = (int)cVar1;
  local_18 = '\0';
  iVar9 = FUN_9d0171ec((int)cVar1,param_1 + 6);
  if (iVar9 == 0) {
    local_18 = '\x03';
    (&DAT_a000207a)[iVar8 * 0x27] = (&DAT_a000207a)[iVar8 * 0x27] + '\x01';
    if ('\x03' < (char)(&DAT_a000207a)[iVar8 * 0x27]) {
      local_18 = '\x04';
    }
  }
  else {
    (&DAT_a000207a)[iVar8 * 0x27] = 0;
    bVar7 = param_1[2];
    cVar2 = param_1[3];
    cVar3 = param_1[4];
    cVar4 = param_1[5];
    cVar5 = param_1[1];
    bVar6 = code_lookup(iVar8 * 0x27 + -0x5fffdf8b,local_c);
    bVar7 = local_c[0] & (bVar7 | cVar2 << 1 | cVar3 << 2 | cVar4 << 3);
    iVar8 = FUN_9d00530c((int)(char)bVar7);
    if (iVar8 != 0) {
      if ((cVar5 != '\0') && (iVar9 = FUN_9d01d0f0((int)(char)bVar7), iVar9 == 0)) {
        local_18 = '\x06';
      }
    }
    else {
      local_18 = '\x05';
    }
    if (((bVar6 < 100) && (iVar8 != 0)) && (local_18 == '\0')) {
      zones_arm(4,bVar6,(int)(char)bVar7,(int)cVar5,1,1);
    }
  }
  *param_2 = cVar1;
  param_2[1] = local_18;
  return 2;
}

