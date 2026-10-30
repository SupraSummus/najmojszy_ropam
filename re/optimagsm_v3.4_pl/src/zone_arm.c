// zone_arm @ 9d0046e8
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Uzbrojenie jednej strefy: stan 3 → 5 (odliczanie), bit 11 = noc, drugie słowo = czas na
   wyjście w ms. */

char zone_arm(undefined4 param_1,int param_2,uint *param_3,char param_4,char param_5,
             undefined1 param_6,char param_7)

{
  char cVar1;
  
  cVar1 = FUN_9d005230(param_2);
  FUN_9d003ccc();
  *param_3 = *param_3 & 0xfffffeff;
  *param_3 = *param_3 & 0xfffffdff;
  if ((cVar1 != '\0') &&
     (*param_3 = *param_3 & 0xfffff7ff | (uint)(param_5 != '\0') << 0xb, (*param_3 & 0xf) == 3)) {
    *param_3 = *param_3 & 0xfffffff0 | 5;
    if (param_7 == '\0') {
      param_3[1] = 0;
    }
    else {
      param_3[1] = *(int *)(&DAT_a00038d8 + param_2 * 0x2c) * 1000;
    }
    DAT_a0000314 = param_4;
    FUN_9d00440c(param_2,param_6);
    FUN_9d05205c(3,param_1,param_2,(int)param_4,(int)param_5);
    FUN_9d0044e8(1,param_5 != '\0',param_2);
  }
  return cVar1;
}

