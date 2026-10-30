// cfg_store_all @ 9d04c878
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Zapisuje struktury z RAM do sekcji i przelicza CRC konfiguracji. */

undefined4 cfg_store_all(void)

{
  bool bVar1;
  int iVar2;
  undefined4 uVar3;
  
  DAT_a0008504 = DAT_9d06d94d;
  iVar2 = cfg_section_put(s_INPUT_9d06b7c8,&DAT_a0003980);
  if (((((((iVar2 == 0) || (iVar2 = cfg_section_put(&DAT_9d06b7d0,&DAT_a0006900), iVar2 == 0)) ||
         (iVar2 = cfg_section_put(&DAT_9d06b7d8,&DAT_a00154f4), iVar2 == 0)) ||
        ((iVar2 = cfg_section_put(s_COUNT_9d06b7e0,&DAT_a000396c), iVar2 == 0 ||
         (iVar2 = cfg_section_put(s_TIMES_9d06b7e8,&DAT_a0015d14), iVar2 == 0)))) ||
       ((iVar2 = cfg_section_put(s_STREFY_9d06b7f0,&DAT_a00038bc), iVar2 == 0 ||
        ((iVar2 = cfg_section_put(&DAT_9d06b7f8,&DAT_a0015c64), iVar2 == 0 ||
         (iVar2 = cfg_section_put(&DAT_9d06b800,&g_cfg_comm), iVar2 == 0)))))) ||
      (iVar2 = cfg_section_put(&DAT_9d06b7d4,&DAT_a0015a68), iVar2 == 0)) ||
     ((((iVar2 = cfg_section_put(s_GPRSCFG_9d06b808,&DAT_a00151bc), iVar2 == 0 ||
        (iVar2 = cfg_section_put(&DAT_9d06b810,&DAT_a001526c), iVar2 == 0)) ||
       (iVar2 = cfg_section_put(&DAT_9d06b818,&DAT_a0015d48), iVar2 == 0)) ||
      (((iVar2 = cfg_section_put(s_EMAIL_9d06b81c,&DAT_a0015b6c), iVar2 == 0 ||
        (iVar2 = cfg_section_put(s_ANINP_9d06b824,&DAT_a00037dc), iVar2 == 0)) ||
       ((iVar2 = cfg_section_put(&DAT_9d06b82c,&DAT_a0015520), iVar2 == 0 ||
        ((iVar2 = cfg_section_put(s_LOGTXT_9d06b860,&DAT_a0005900), iVar2 == 0 ||
         (iVar2 = cfg_section_put(&DAT_9d06b834,&DAT_a0015dd0), iVar2 == 0)))))))))) {
    bVar1 = false;
  }
  else {
    bVar1 = true;
  }
  cfg_section_put(s_APCFG_9d06b848,&DAT_a00010c8);
  cfg_section_put(&DAT_9d06b858,&DAT_a000218c);
  cfg_section_put(&DAT_9d06b844,&g_modules);
  if (bVar1) {
    g_cfg_crc = crc16_xmodem(&g_config,g_config + -4);
    uVar3 = 0xccbc;
  }
  else {
    uVar3 = 0;
  }
  return uVar3;
}

