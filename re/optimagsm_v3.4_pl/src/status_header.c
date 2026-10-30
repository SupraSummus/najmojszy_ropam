// status_header @ 9d01c1e4
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Nagłówek stanu na żywo (220 B): maski wejść, wyjścia, strefy, flagi, czas; część
   odpowiedzi na 0x24. */

undefined1 status_header(undefined1 param_1,int param_2)

{
  bool bVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  uint local_a8;
  undefined4 local_90;
  undefined4 local_8c;
  uint local_88;
  uint local_84;
  uint local_80;
  uint local_7c;
  uint local_78;
  uint local_74;
  uint local_70;
  uint local_6c;
  undefined2 local_68 [2];
  undefined4 local_64;
  undefined4 local_60;
  undefined1 auStack_5c [12];
  uint local_50;
  uint local_4c;
  uint local_48;
  uint local_44;
  uint local_40;
  undefined4 local_3c;
  uint local_38;
  uint local_34;
  uint local_30;
  uint local_2c;
  
  local_90 = 0;
  local_88 = 0;
  local_84 = 0;
  local_80 = 0;
  local_7c = 0;
  local_78 = 0;
  local_74 = 0;
  local_70 = 0;
  local_6c = 0;
  local_a8 = 0;
  bVar1 = false;
  while ((!bVar1 && (local_a8 < 0x30))) {
    uVar2 = input_state_is_1(local_a8);
    uVar3 = uVar2 << (local_a8 & 0x1f);
    uVar4 = uVar3;
    uVar2 = (uVar2 >> 1) >> ((local_a8 ^ 0xffffffff) & 0x1f) | 0 << (local_a8 & 0x1f);
    if ((local_a8 & 0x20) != 0) {
      uVar4 = 0;
      uVar2 = uVar3;
    }
    local_88 = uVar4 | local_88;
    local_84 = uVar2 | local_84;
    local_50 = local_88;
    local_4c = local_84;
    uVar2 = input_state_is_3(local_a8);
    uVar3 = uVar2 << (local_a8 & 0x1f);
    uVar4 = uVar3;
    uVar2 = (uVar2 >> 1) >> ((local_a8 ^ 0xffffffff) & 0x1f) | 0 << (local_a8 & 0x1f);
    if ((local_a8 & 0x20) != 0) {
      uVar4 = 0;
      uVar2 = uVar3;
    }
    local_80 = uVar4 | local_80;
    local_7c = uVar2 | local_7c;
    local_40 = (*(uint *)(&DAT_a0002b68 + local_a8 * 0x28) & 0x7f) >> 6 |
               (*(uint *)(&DAT_a0002b68 + local_a8 * 0x28) & 0xfff) >> 0xb |
               (*(uint *)(&DAT_a0002b68 + local_a8 * 0x28) & 0xff) >> 7;
    local_3c = 0;
    uVar3 = local_40 << (local_a8 & 0x1f);
    uVar4 = uVar3;
    uVar2 = 0U >> ((local_a8 ^ 0xffffffff) & 0x1f) | 0 << (local_a8 & 0x1f);
    if ((local_a8 & 0x20) != 0) {
      uVar4 = 0;
      uVar2 = uVar3;
    }
    local_78 = uVar4 | local_78;
    local_74 = uVar2 | local_74;
    uVar3 = ((*(uint *)(&DAT_a0002b68 + local_a8 * 0x28) & 0x1fff) >> 0xc) << (local_a8 & 0x1f);
    uVar4 = uVar3;
    uVar2 = 0U >> ((local_a8 ^ 0xffffffff) & 0x1f) | 0 << (local_a8 & 0x1f);
    if ((local_a8 & 0x20) != 0) {
      uVar4 = 0;
      uVar2 = uVar3;
    }
    local_70 = uVar4 | local_70;
    local_6c = uVar2 | local_6c;
    bVar1 = local_a8 + 1 < local_a8;
    local_a8 = local_a8 + 1;
    local_48 = local_80;
    local_44 = local_7c;
    local_38 = local_78;
    local_34 = local_74;
    local_30 = local_70;
    local_2c = local_6c;
  }
  memcpy(param_2,&local_88,8);
  memcpy(param_2 + 8,&local_80,8);
  memcpy(param_2 + 0x10,&local_78,8);
  memcpy(param_2 + 0x18,&local_70,8);
  memcpy(param_2 + 0x20,&DAT_a00010a0,8);
  memcpy(param_2 + 0x28,&DAT_a00010a8,8);
  local_90 = outputs_mask();
  memcpy(param_2 + 0x30,&local_90,4);
  memcpy(param_2 + 0x34,&DAT_a0000094,4);
  local_64 = FUN_9d02b82c();
  memcpy(param_2 + 0x38,&local_64,4);
  local_60 = FUN_9d02b978();
  memcpy(param_2 + 0x3c,&local_60,4);
  local_68[0] = FUN_9d03265c();
  memcpy(param_2 + 0x40,local_68,2);
  *(undefined1 *)(param_2 + 0x42) = DAT_a0016725;
  *(undefined1 *)(param_2 + 0x43) = DAT_a0016754;
  FUN_9d05e3e4(DAT_a00001b0,param_2 + 0x44,0);
  FUN_9d05e3e4(DAT_a0000160,param_2 + 0x4c,0);
  FUN_9d05e3e4(DAT_a00001a4,param_2 + 0x6c,0);
  FUN_9d05e3e4(DAT_a00000c0,param_2 + 0x78,0);
  memcpy(param_2 + 0xb8,&DAT_a000277c,0x10);
  memcpy(param_2 + 200,&DAT_a0000154,8);
  FUN_9d041fd0(auStack_5c);
  memcpy(param_2 + 0xd0,auStack_5c,8);
  local_8c = FUN_9d01efac(param_1);
  memcpy(param_2 + 0xd8,&local_8c,4);
  return 0xdc;
}

