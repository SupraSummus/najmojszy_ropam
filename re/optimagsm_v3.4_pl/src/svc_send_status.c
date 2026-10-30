// svc_send_status @ 9d03c2dc
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Odsyła 4-bajtowy status (little-endian). */

void svc_send_status(undefined4 param_1)

{
  undefined4 local_res0 [4];
  undefined1 auStack_48 [64];
  
  local_res0[0] = param_1;
  memcpy(auStack_48,local_res0,4);
  svc_transmit(auStack_48,4);
  return;
}

