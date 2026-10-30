// zones_arm @ 9d004884
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Uzbraja strefy z maski (bity 0–3); źródło 3 = panel z kodem, 5 = szybkie uzbrojenie; tryb
   ≠ 0 = czuwanie nocne. */

undefined4
zones_arm(int param_1,char param_2,char param_3,char param_4,undefined1 param_5,undefined1 param_6)

{
  char local_res4;
  uint local_38;
  undefined1 auStack_34 [36];
  undefined1 local_10;
  undefined1 local_c;
  
  local_10 = param_5;
  local_c = param_6;
  local_res4 = param_2;
  if (((param_1 == 3) || (param_1 == 4)) && (param_2 == '\0')) {
    local_res4 = ' ';
  }
  FUN_9d062ff8(DAT_a0000340);
  FUN_9d05e3e4(g_zones_state,auStack_34,0);
  for (local_38 = 0; (int)local_38 < 4; local_38 = local_38 + 1) {
    if (((int)param_3 >> (local_38 & 0x1f) & 1U) != 0) {
      zone_arm(param_1,local_38,auStack_34 + local_38 * 8,(int)local_res4,(int)param_4,local_c,
               local_10);
    }
  }
  FUN_9d05e5c0(g_zones_state,auStack_34);
  FUN_9d0630a0(DAT_a0000340);
  return 1;
}

