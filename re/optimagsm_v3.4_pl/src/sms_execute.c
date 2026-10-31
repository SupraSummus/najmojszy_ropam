// sms_execute @ 9d016544
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Wykonuje polecenie SMS „KKKK polecenie” (komenda serwisowa 0x19); tablica 52 poleceń pod
   0x9D05759C. */

bool sms_execute(int param_1)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  byte local_c8;
  bool local_c6;
  int local_c4;
  int local_c0;
  byte local_b4 [4];
  undefined1 auStack_b0 [168];
  
  local_c4 = 0x32;
  local_b4[0] = 0;
  iVar2 = sms_code_lookup(param_1,local_b4);
  bVar1 = iVar2 != 0xff;
  iVar3 = FUN_9d01600c(param_1,bVar1);
  if (iVar3 == 0) {
    if (bVar1) {
      if (((iVar2 < 1) || (99 < iVar2)) || ((local_b4[0] & 0x10) != 0)) {
        strcpy(&DAT_a00026d4,param_1);
        FUN_9d025fb4(param_1);
        uVar4 = FUN_9d06a74c(param_1);
        if (4 < uVar4) {
          local_c4 = 0xd;
        }
        iVar3 = FUN_9d06c96c(param_1,s_android_9d05776c);
        if (iVar3 == 0) {
          local_c0 = 5;
        }
        else {
          local_c0 = 0xc;
        }
        local_c6 = iVar3 != 0;
        uVar4 = FUN_9d06a74c(param_1);
        if (6 < uVar4) {
          for (local_c8 = 0; local_c8 < 0x34; local_c8 = local_c8 + 1) {
            iVar3 = FUN_9d06c96c(param_1 + local_c0,(&PTR_DAT_9d05759c)[(uint)local_c8 * 2]);
            if ((iVar3 != 0) && (iVar3 == param_1 + local_c0)) {
              local_c4 = (*(code *)(&PTR_FUN_9d0575a0)[(uint)local_c8 * 2])
                                   (param_1 + local_c0,iVar2,(int)(char)local_b4[0],local_c6);
              break;
            }
          }
        }
        if (local_c4 == 0x2f) {
          FUN_9d0117c8(auStack_b0,iVar2,(int)(char)local_b4[0]);
          FUN_9d01f6a0(0xa00026bf,auStack_b0);
        }
        else if (local_c4 == 0x30) {
          status_header(auStack_b0);
          FUN_9d01f6a0(0xa00026bf,auStack_b0);
        }
        else if (local_c4 == 0x33) {
          FUN_9d0117c8(auStack_b0,iVar2,(int)(char)local_b4[0]);
          FUN_9d020c58(0xa00026bf,auStack_b0,0);
          FUN_9d015e50(auStack_b0);
          FUN_9d020c58(0xa00026bf,auStack_b0,0);
        }
        if (((local_c4 != 0x32) && (local_c4 != 0x2f)) && ((local_c4 != 0x30 && (local_c4 != 0x33)))
           ) {
          FUN_9d01f6a0(0xa00026bf,(&PTR_s_Wyjscie_zalaczone_9d056fb0)[local_c4]);
        }
        if ((DAT_a000014c != '\0') && (DAT_a0000135 != '\x01')) {
          FUN_9d01f6a0(0xa00026bf,s_Uzywasz_hasla_fabrycznego__zmien_9d0570e4);
        }
        DAT_a000014c = '\0';
      }
      else {
        bVar1 = false;
      }
    }
  }
  else {
    bVar1 = true;
  }
  return bVar1;
}

