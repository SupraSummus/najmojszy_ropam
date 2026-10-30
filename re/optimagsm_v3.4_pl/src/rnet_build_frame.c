// rnet_build_frame @ 9d0535dc
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Wypełnia ramkę: adres, komenda, długość, dane. */

void rnet_build_frame(undefined1 *param_1,undefined1 param_2,undefined1 param_3,undefined4 param_4,
                     char param_5)

{
  *param_1 = param_2;
  param_1[1] = param_3;
  param_1[2] = param_5;
  if (param_5 != '\0') {
    memcpy(param_1 + 3,param_4,(int)param_5);
  }
  return;
}

