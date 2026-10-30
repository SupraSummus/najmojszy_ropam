// strcpy @ 9d069f8c
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

uint * strcpy(uint *param_1,uint *param_2)

{
  uint *puVar1;
  uint uVar2;
  char cVar3;
  uint uVar4;
  uint uVar5;
  
  puVar1 = param_1;
  if ((((uint)param_1 | (uint)param_2) & 3) == 0) {
    uVar4 = *param_2;
    while( true ) {
      uVar2 = uVar4 + 0xfefefeff & 0x80808080;
      if ((uVar2 != 0) && ((uVar2 & uVar4) != uVar2)) {
        cVar3 = (char)*param_2;
        goto LAB_9d06a080;
      }
      uVar2 = param_2[1];
      *puVar1 = uVar4;
      uVar4 = uVar2 + 0xfefefeff & 0x80808080;
      if ((uVar4 != 0) && ((uVar4 & uVar2) != uVar4)) {
        param_2 = param_2 + 1;
        cVar3 = (char)*param_2;
        puVar1 = puVar1 + 1;
        goto LAB_9d06a080;
      }
      uVar5 = param_2[2];
      puVar1[1] = uVar2;
      uVar4 = uVar5 + 0xfefefeff & 0x80808080;
      if ((uVar4 != 0) && ((uVar4 & uVar5) != uVar4)) {
        param_2 = param_2 + 2;
        cVar3 = (char)*param_2;
        puVar1 = puVar1 + 2;
        goto LAB_9d06a080;
      }
      uVar2 = param_2[3];
      puVar1[2] = uVar5;
      uVar4 = uVar2 + 0xfefefeff & 0x80808080;
      if ((uVar4 != 0) && ((uVar4 & uVar2) != uVar4)) break;
      uVar4 = param_2[4];
      puVar1[3] = uVar2;
      puVar1 = puVar1 + 4;
      param_2 = param_2 + 4;
    }
    param_2 = param_2 + 3;
    cVar3 = (char)*param_2;
    puVar1 = puVar1 + 3;
  }
  else {
    cVar3 = (char)*param_2;
  }
LAB_9d06a080:
  while( true ) {
    *(char *)puVar1 = cVar3;
    if (cVar3 == '\0') {
      return param_1;
    }
    cVar3 = *(char *)((int)param_2 + 1);
    *(char *)((int)puVar1 + 1) = cVar3;
    if (cVar3 == '\0') {
      return param_1;
    }
    cVar3 = *(char *)((int)param_2 + 2);
    *(char *)((int)puVar1 + 2) = cVar3;
    if (cVar3 == '\0') {
      return param_1;
    }
    cVar3 = *(char *)((int)param_2 + 3);
    *(char *)((int)puVar1 + 3) = cVar3;
    if (cVar3 == '\0') {
      return param_1;
    }
    uVar4 = param_2[1];
    *(char *)(puVar1 + 1) = (char)uVar4;
    if ((char)uVar4 == '\0') {
      return param_1;
    }
    cVar3 = *(char *)((int)param_2 + 5);
    *(char *)((int)puVar1 + 5) = cVar3;
    if (cVar3 == '\0') {
      return param_1;
    }
    cVar3 = *(char *)((int)param_2 + 6);
    *(char *)((int)puVar1 + 6) = cVar3;
    if (cVar3 == '\0') {
      return param_1;
    }
    cVar3 = *(char *)((int)param_2 + 7);
    *(char *)((int)puVar1 + 7) = cVar3;
    if (cVar3 == '\0') break;
    cVar3 = (char)param_2[2];
    puVar1 = puVar1 + 2;
    param_2 = param_2 + 2;
  }
  return param_1;
}

