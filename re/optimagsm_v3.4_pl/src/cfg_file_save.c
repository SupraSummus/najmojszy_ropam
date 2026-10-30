// cfg_file_save @ 9d04be4c
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Zapisuje g_config do pliku (FatFs FA_CREATE_ALWAYS | FA_WRITE). */

undefined1 cfg_file_save(undefined4 param_1)

{
  int local_238;
  undefined1 local_234;
  undefined1 auStack_230 [548];
  int local_c;
  
  local_234 = 0;
  local_238 = FUN_9d00d464(auStack_230,param_1,10);
  if (local_238 == 0) {
    local_238 = FUN_9d00de2c(auStack_230,&g_config,0xccbc,&local_c);
  }
  FUN_9d00e714(auStack_230);
  if ((local_238 == 0) && (local_c == 0xccbc)) {
    local_234 = 1;
  }
  return local_234;
}

