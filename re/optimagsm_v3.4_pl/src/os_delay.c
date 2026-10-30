// os_delay @ 9d05ecf8
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Uśpienie bieżącego zadania na N tyknięć, FreeRTOS vTaskDelay. ❓ jednostka (ms?) */

void os_delay(int param_1)

{
  int *piVar1;
  int *piVar2;
  int *piVar3;
  
  if (param_1 - 1U < 0x7ffffffd) {
    *DAT_a0000058 = DAT_a0000204;
    piVar1 = DAT_a00003c4;
    piVar2 = (int *)DAT_a00003c4[1];
    piVar3 = (int *)*DAT_a00003c4;
    if ((DAT_a00003c4 != piVar2) && (piVar2 == piVar3)) {
      *(uint *)DAT_a00003c4[0x19] = *(uint *)DAT_a00003c4[0x19] & ~DAT_a00003c4[0x1a];
      piVar3 = piVar2;
    }
    *piVar2 = (int)piVar3;
    piVar3[1] = (int)piVar2;
    *piVar1 = (int)piVar1;
    piVar1[0xc] = (int)(piVar1 + 0xe);
    piVar1[1] = (int)piVar1;
    FUN_9d05eae8(piVar1 + 0xe,param_1 + 1);
    DAT_a00003c8 = 0;
    *DAT_a0000048 = DAT_a00001fc;
    *DAT_a0000054 = DAT_a0000204;
    return;
  }
                    /* WARNING: Subroutine does not return */
  FUN_9d06d108(300);
}

