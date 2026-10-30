// cfg_file_load @ 9d04bd5c
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Wczytuje plik (np. 0:config.rpm) do g_config tylko do odczytu (FatFs FA_READ) i sprawdza CRC. */

undefined4
cfg_file_load(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  short sVar1;
  int iVar2;
  undefined4 uVar3;
  undefined1 auStack_230 [548];
  int local_c;
  
  iVar2 = FUN_9d00d464(auStack_230,param_1,1,param_4,0);
  if (iVar2 == 0) {
    iVar2 = FUN_9d00d894(auStack_230,&g_config,0xccbc,&local_c);
    FUN_9d00e714(auStack_230);
    if ((iVar2 == 0) && (local_c == 0xccbc)) {
      sVar1 = crc16_xmodem(&g_config,g_config + -4);
      if (sVar1 == g_cfg_crc) {
        uVar3 = 1;
      }
      else {
        uVar3 = 0;
      }
    }
    else {
      uVar3 = 0;
    }
  }
  else {
    uVar3 = 0;
  }
  return uVar3;
}

