// output_is_on @ 9d008a78
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Stan wyjścia: bit 3 = włączone, bit 2 = wyłączone, inaczej bit 0 (logika). */

bool output_is_on(int param_1)

{
  int iVar1;
  byte *pbVar2;
  undefined1 local_10;
  
  iVar1 = shared_lock(DAT_a0000164);
  pbVar2 = (byte *)(iVar1 + param_1);
  if ((*pbVar2 & 8) == 0) {
    if ((*pbVar2 & 4) == 0) {
      local_10 = (*pbVar2 & 1) != 0;
    }
    else {
      local_10 = false;
    }
  }
  else {
    local_10 = true;
  }
  shared_unlock(DAT_a0000164);
  return local_10;
}

