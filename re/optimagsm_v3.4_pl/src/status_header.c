// status_header @ 9d011470
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Składa nagłówek statusu: '*', '4', wersja firmware'u, wersja z g_boot_version, maska
   modułów. ❓ */

void status_header(undefined1 *param_1)

{
  undefined4 uVar1;
  uint local_18;
  uint local_14;
  
  local_14 = 0;
  *param_1 = 0x2a;
  param_1[1] = 0x34;
  sprintf(param_1 + 2,&DAT_9d057174,(int)FW_VERSION);
  sprintf(param_1 + 4,&DAT_9d057174,g_boot_version);
  for (local_18 = 0; local_18 < 0x12; local_18 = local_18 + 1) {
    if (((&DAT_a00159a1)[local_18 * 0xb] & 1) == 0) {
      local_14 = local_14 & ~(1 << (local_18 & 0x1f));
    }
    else {
      local_14 = local_14 | ((byte)(&DAT_a00159a1)[local_18 * 0xb] & 1) << (local_18 & 0x1f);
    }
  }
  sprintf(param_1 + 6,&DAT_9d05717c,local_14);
  sprintf(param_1 + 0xe,&DAT_9d05717c,DAT_a0015538);
  sprintf(param_1 + 0x16,&DAT_9d05717c,DAT_a001553c);
  sprintf(param_1 + 0x1e,&DAT_9d05717c,DAT_a0015658);
  sprintf(param_1 + 0x26,&DAT_9d05717c,DAT_a001565c);
  sprintf(param_1 + 0x2e,&DAT_9d05717c,DAT_a0015778);
  sprintf(param_1 + 0x36,&DAT_9d05717c,DAT_a001577c);
  sprintf(param_1 + 0x3e,&DAT_9d05717c,DAT_a0015898);
  sprintf(param_1 + 0x46,&DAT_9d05717c,DAT_a001589c);
  sprintf(param_1 + 0x4e,&DAT_9d057184,DAT_a00037f0);
  sprintf(param_1 + 0x52,&DAT_9d057184,DAT_a00037f2);
  for (local_18 = 0x56; local_18 < 0x9e; local_18 = local_18 + 1) {
    param_1[local_18] = 0x2d;
  }
  uVar1 = FUN_9d0278d4(param_1,0x9e);
  sprintf(param_1 + 0x9e,&DAT_9d05718c,uVar1);
  param_1[0xa0] = 0;
  return;
}

