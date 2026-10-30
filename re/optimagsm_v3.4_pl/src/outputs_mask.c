// outputs_mask @ 9d008b28
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Maska włączonych wyjść 1–32 (bit 0 każdego bajtu tablicy wyjść). */

uint outputs_mask(void)

{
  byte bVar1;
  uint local_18;
  uint local_14;
  byte *local_10;
  
  local_14 = 0;
  local_10 = (byte *)shared_lock(DAT_a0000164);
  for (local_18 = 0; local_18 < 0x20; local_18 = local_18 + 1) {
    bVar1 = *local_10;
    local_10 = local_10 + 1;
    local_14 = local_14 | (bVar1 & 1) << (local_18 & 0x1f);
  }
  shared_unlock(DAT_a0000164);
  return local_14;
}

