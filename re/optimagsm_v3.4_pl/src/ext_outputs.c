// ext_outputs @ 9d01730c
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Komenda 0x35: sterowanie wyjściami ❓. */

undefined4 ext_outputs(undefined1 *param_1,undefined1 *param_2)

{
  char cVar1;
  int local_14;
  int local_10;
  
  local_10 = 0;
  *param_2 = *param_1;
  param_2[1] = 0;
  local_14 = 0;
  do {
    if (0x27 < local_14) {
      return 2;
    }
    cVar1 = param_1[local_14 + 1];
    if (local_14 < 0x20) {
      if (((*(uint *)(&DAT_a0002948 + local_14 * 0x10) & 0x7fff) >> 0xe != 0) && (local_14 < 0x20))
      {
        if (cVar1 == '1') {
          *(undefined4 *)(&DAT_a0002950 + local_14 * 0x10) = 0;
          output_force_on(local_14);
        }
        else if (cVar1 == '0') {
          output_force_off2(local_14);
        }
        else if (cVar1 != 'x') {
          return 2;
        }
      }
    }
    else {
      if (cVar1 != 'x') {
        if (local_10 < 4) {
          FUN_9d01f334(local_10 + 1,cVar1 == '1');
        }
        else {
          FUN_9d059008(local_10 + -4,cVar1 == '1');
        }
      }
      local_10 = local_10 + 1;
    }
    local_14 = local_14 + 1;
  } while( true );
}

