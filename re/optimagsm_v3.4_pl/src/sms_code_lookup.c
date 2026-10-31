// sms_code_lookup @ 9d016474
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Kod z pierwszych 4 znaków polecenia SMS (spacja na 5. pozycji). */

int sms_code_lookup(int param_1,undefined1 *param_2)

{
  int iVar1;
  int iVar2;
  undefined1 auStack_14 [4];
  undefined1 local_10;
  
  if (*(char *)(param_1 + 4) == ' ') {
    memcpy(auStack_14,param_1,4);
    local_10 = 0;
    iVar1 = code_lookup(auStack_14,param_2);
    if (iVar1 == 100) {
      *param_2 = 0;
    }
    if ((iVar1 != 0xff) && (iVar2 = FUN_9d06b168(&DAT_9d057764,auStack_14), iVar2 == 0)) {
      DAT_a000014c = 1;
    }
  }
  else {
    iVar1 = 0xff;
  }
  return iVar1;
}

