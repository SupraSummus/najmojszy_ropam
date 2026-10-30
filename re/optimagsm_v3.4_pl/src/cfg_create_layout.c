// cfg_create_layout @ 9d04cc98
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Buduje katalog sekcji (INPUT, OUT, …) dla nowej konfiguracji. ❓ */

void cfg_create_layout(void)

{
  g_cfg_nsections = 0;
  cfg_section_add(s_INPUT_9d06b7c8,0x1f80);
  cfg_section_add(&DAT_9d06b7d0,0x1c00);
  cfg_section_add(&DAT_9d06b7d8,0x2c);
  cfg_section_add(s_TIMES_9d06b7e8,0xe);
  cfg_section_add(s_STREFY_9d06b7f0,0x20);
  cfg_section_add(s_COUNT_9d06b7e0,0x12);
  cfg_section_add(&DAT_9d06b7f8,0xae);
  cfg_section_add(&DAT_9d06b800,0x21);
  cfg_section_add(&DAT_9d06b7d4,0x104);
  cfg_section_add(s_GPRSCFG_9d06b808,0xb0);
  cfg_section_add(&DAT_9d06b810,0x288);
  cfg_section_add(&DAT_9d06b818,0x88);
  cfg_section_add(s_EMAIL_9d06b81c,0xf8);
  cfg_section_add(s_NAPIS_9d06b868,0x64c);
  cfg_section_add(&DAT_9d06b844,0xc6);
  cfg_section_add(s_ANINP_9d06b824,0xe0);
  cfg_section_add(&DAT_9d06b82c,0x480);
  cfg_section_add(s_LOGBIN_9d06b870,0x1000);
  cfg_section_add(s_LOGTXT_9d06b860,0x1000);
  cfg_section_add(s_M_TPR1_9d06b878,0xb68);
  cfg_section_add(s_M_TPR2_9d06b880,0xb68);
  cfg_section_add(s_M_TPR3_9d06b888,0xb68);
  cfg_section_add(s_M_TPR4_9d06b890,0xb68);
  cfg_section_add(s_M_RF4_9d06b83c,0x12a);
  cfg_section_add(&DAT_9d06b834,0x500);
  cfg_section_add(s_APCFG_9d06b848,0x16a);
  cfg_section_add(&DAT_9d06b850,0x44);
  cfg_section_add(&DAT_9d06b858,0x57);
  g_config = 0xccbc;
  DAT_a0008509 = FW_VERSION;
  DAT_a0008505 = (char)g_boot_version;
  return;
}

