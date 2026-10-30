// cfg_load_all @ 9d04c49c
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Rozpakowuje wszystkie sekcje (INPUT, OUT, TEST, COUNT, TIMES, STREFY, COMM, …) do struktur w
   RAM. */

undefined1 cfg_load_all(void)

{
  undefined1 uVar1;
  int iVar2;
  int local_10;
  
  FUN_9d06e628();
  iVar2 = cfg_section_get(s_INPUT_9d06b7c8,&g_cfg_inputs);
  if (((((((iVar2 == 0) || (iVar2 = cfg_section_get(&DAT_9d06b7d0,&g_cfg_outputs), iVar2 == 0)) ||
         (iVar2 = cfg_section_get(&DAT_9d06b7d4,&DAT_a0015a68), iVar2 == 0)) ||
        ((iVar2 = cfg_section_get(&DAT_9d06b7d8,&DAT_a00154f4), iVar2 == 0 ||
         (iVar2 = cfg_section_get(s_COUNT_9d06b7e0,&DAT_a000396c), iVar2 == 0)))) ||
       ((iVar2 = cfg_section_get(s_TIMES_9d06b7e8,&DAT_a0015d14), iVar2 == 0 ||
        ((iVar2 = cfg_section_get(s_STREFY_9d06b7f0,&g_cfg_zones), iVar2 == 0 ||
         (iVar2 = cfg_section_get(&DAT_9d06b7f8,&DAT_a0015c64), iVar2 == 0)))))) ||
      (iVar2 = cfg_section_get(&DAT_9d06b800,&g_cfg_comm), iVar2 == 0)) ||
     (((((iVar2 = cfg_section_get(s_GPRSCFG_9d06b808,&DAT_a00151bc), iVar2 == 0 ||
         (iVar2 = cfg_section_get(&DAT_9d06b810,&DAT_a001526c), iVar2 == 0)) ||
        (iVar2 = cfg_section_get(&DAT_9d06b818,&DAT_a0015d48), iVar2 == 0)) ||
       (((iVar2 = cfg_section_get(s_EMAIL_9d06b81c,&DAT_a0015b6c), iVar2 == 0 ||
         (iVar2 = cfg_section_get(s_ANINP_9d06b824,&DAT_a00037dc), iVar2 == 0)) ||
        ((iVar2 = cfg_section_get(&DAT_9d06b82c,&DAT_a0015520), iVar2 == 0 ||
         ((iVar2 = cfg_section_get(&DAT_9d06b834,&DAT_a0015dd0), iVar2 == 0 ||
          (iVar2 = cfg_section_get(s_M_RF4_9d06b83c,&DAT_a0000f70), iVar2 == 0)))))))) ||
      ((iVar2 = cfg_section_get(&DAT_9d06b844,&g_modules), iVar2 == 0 ||
       (iVar2 = cfg_section_get(s_APCFG_9d06b848,&DAT_a00010c8), iVar2 == 0)))))) {
    uVar1 = 0;
  }
  else {
    uVar1 = 1;
  }
  if (DAT_a0008504 < '\f') {
    memset(&DAT_a0002010,0,0x44);
  }
  else {
    cfg_section_get(&DAT_9d06b850,&DAT_a0002010);
  }
  if (DAT_a0008504 < '\x0f') {
    memset(&DAT_a000218c,0,0x57);
  }
  else {
    cfg_section_get(&DAT_9d06b858,&DAT_a000218c);
  }
  if (DAT_a0008504 < '\x10') {
    memset(&DAT_a0016770,0,0x120);
  }
  else {
    memcpy(&DAT_a0016770,&DAT_a0014b76,0x120);
  }
  for (local_10 = 0; local_10 < 0x12; local_10 = local_10 + 1) {
    if (((&DAT_a00159a1)[local_10 * 0xb] & 1) != 0) {
      (&DAT_a00159a3)[local_10 * 0xb] = 2;
    }
  }
  FUN_9d06e620();
  FUN_9d066fa0();
  return uVar1;
}

