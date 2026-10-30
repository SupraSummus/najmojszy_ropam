// bus_poll_round @ 9d0290a0
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* Obieg po 18 gniazdach modułów; numer gniazda wybiera obsługę (1–4 TPR, 9 koncentrator,
   …). */

void bus_poll_round(char param_1)

{
  byte bVar1;
  byte bVar2;
  int iVar3;
  byte local_10;
  byte local_f;
  char local_e;
  
  for (local_10 = 0; local_10 < 0x12; local_10 = local_10 + 1) {
    if ((((&DAT_a00159a1)[(uint)local_10 * 0xb] & 1) != 0) &&
       (*(int *)(&DAT_a0003300 + (uint)local_10 * 0x10) =
             *(int *)(&DAT_a0003300 + (uint)local_10 * 0x10) + -10,
       *(int *)(&DAT_a0003300 + (uint)local_10 * 0x10) < 1)) {
      bVar2 = 10;
      local_e = '\x01';
      switch(local_10) {
      case 1:
      case 2:
      case 3:
      case 4:
        local_e = tpr_service(local_10);
        FUN_9d01bf4c(local_10);
        break;
      case 5:
        iVar3 = FUN_9d05d0a0();
        local_e = iVar3 != 0;
        if (!(bool)local_e) {
          FUN_9d05d04c();
        }
        break;
      case 6:
        local_e = FUN_9d030d60();
        break;
      case 7:
        local_e = FUN_9d06311c();
        break;
      case 8:
        if (DAT_a0000180 != '\x01') {
          local_e = FUN_9d0257ec();
        }
        break;
      case 9:
        local_e = hub_poll();
        bVar2 = 2;
        break;
      case 10:
        local_e = FUN_9d058b88(&DAT_a0000280);
        break;
      case 0xb:
        iVar3 = FUN_9d066c0c(0xb);
        local_e = iVar3 != 0;
        break;
      case 0xc:
        iVar3 = FUN_9d066c0c(0xc);
        local_e = iVar3 != 0;
        break;
      case 0xd:
        iVar3 = FUN_9d0635f0(0xd);
        local_e = iVar3 != 0;
        break;
      case 0xe:
        iVar3 = FUN_9d0635f0(0xe);
        local_e = iVar3 != 0;
        break;
      case 0xf:
        if ((((DAT_a00000e7 == '\0') && ((DAT_a000277e & 0x3f) >> 5 == 0)) &&
            ((_g_sys_flags & 3) >> 1 == 0)) && ((_g_sys_flags & 1) == 0)) {
          local_e = '\0';
        }
        else {
          local_e = '\x01';
        }
        DAT_a00000e7 = '\0';
      }
      *(undefined4 *)(&DAT_a0003300 + (uint)local_10 * 0x10) =
           *(undefined4 *)(&DAT_a00032fc + (uint)local_10 * 0x10);
      if (local_e == '\x01') {
        (&DAT_a00032f8)[(uint)local_10 * 0x10] = 0;
        if ((&DAT_a00159a3)[(uint)local_10 * 0xb] == '\x01') {
          (&DAT_a00159a3)[(uint)local_10 * 0xb] = 2;
          FUN_9d0521c0(0x76,local_10);
          if ((byte)(&DAT_a00159a2)[(uint)local_10 * 0xb] < 4) {
            FUN_9d00703c(0,(&DAT_a00159a2)[(uint)local_10 * 0xb],2,local_10);
          }
        }
        if (((byte)(&DAT_a00159a1)[(uint)local_10 * 0xb] & 0x7f) >> 6 != 0) {
          FUN_9d0521c0(0x82,local_10);
          (&DAT_a00159a1)[(uint)local_10 * 0xb] = (&DAT_a00159a1)[(uint)local_10 * 0xb] & 0xbf;
        }
        if ((((byte)(&DAT_a00159a1)[(uint)local_10 * 0xb] & 0x1f) >> 4 == 0) ||
           ((&DAT_a0017100)[local_10] == '\x01')) {
          if ((((byte)(&DAT_a00159a1)[(uint)local_10 * 0xb] & 0x1f) >> 4 != 1) &&
             ((&DAT_a0017100)[local_10] != '\0')) {
            FUN_9d052120(0x81,0,local_10);
          }
        }
        else {
          FUN_9d052120(0x81,1,local_10);
        }
        if ((((byte)(&DAT_a00159a1)[(uint)local_10 * 0xb] & 0x3f) >> 5 == 0) ||
           ((&DAT_a0017114)[local_10] == '\x01')) {
          if ((((byte)(&DAT_a00159a1)[(uint)local_10 * 0xb] & 0x3f) >> 5 != 1) &&
             ((&DAT_a0017114)[local_10] != '\0')) {
            FUN_9d052120(0x80,0,local_10);
          }
        }
        else {
          FUN_9d052120(0x80,1,local_10);
        }
        (&DAT_a0017114)[local_10] =
             (byte)(((uint)(byte)(&DAT_a00159a1)[(uint)local_10 * 0xb] << 0x1a) >> 0x1f);
        (&DAT_a0017100)[local_10] =
             (byte)(((uint)(byte)(&DAT_a00159a1)[(uint)local_10 * 0xb] << 0x1b) >> 0x1f);
        if ((byte)(&DAT_a00159a2)[(uint)local_10 * 0xb] < 4) {
          if (((byte)(&DAT_a00159a1)[(uint)local_10 * 0xb] & 7) >> 2 == 0) {
            (&DAT_a00032e4)[local_10] = 0;
          }
          else if ((byte)(&DAT_a00032e4)[local_10] < 100) {
            (&DAT_a00032e4)[local_10] = (&DAT_a00032e4)[local_10] + '\x01';
          }
          if (((((byte)(&DAT_a00159a1)[(uint)local_10 * 0xb] & 7) >> 2 == 0) ||
              (((byte)(&DAT_a00159a1)[(uint)local_10 * 0xb] & 0xf) >> 3 == 1)) ||
             ((byte)(&DAT_a00032e4)[local_10] < 5)) {
            if ((((byte)(&DAT_a00159a1)[(uint)local_10 * 0xb] & 7) >> 2 != 1) &&
               (((byte)(&DAT_a00159a1)[(uint)local_10 * 0xb] & 0xf) >> 3 != 0)) {
              FUN_9d00703c(0,(&DAT_a00159a2)[(uint)local_10 * 0xb],4,local_10);
              FUN_9d0521c0(0x78,local_10);
              (&DAT_a00159a1)[(uint)local_10 * 0xb] = (&DAT_a00159a1)[(uint)local_10 * 0xb] & 0xf7;
            }
          }
          else {
            FUN_9d01ef08((&DAT_a00159a2)[(uint)local_10 * 0xb],local_10 + 0x91);
            FUN_9d007300((&DAT_a00159a2)[(uint)local_10 * 0xb],3,local_10);
            FUN_9d0521c0(0x77,local_10);
            (&DAT_a00159a1)[(uint)local_10 * 0xb] = (&DAT_a00159a1)[(uint)local_10 * 0xb] & 0xf7 | 8
            ;
          }
        }
      }
      else {
        *(short *)(&DAT_a0003304 + (uint)local_10 * 0x10) =
             *(short *)(&DAT_a0003304 + (uint)local_10 * 0x10) + 1;
        bVar1 = (&DAT_a00032f8)[(uint)local_10 * 0x10];
        (&DAT_a00032f8)[(uint)local_10 * 0x10] = bVar1 + 1;
        if ((bVar2 < bVar1) &&
           (*(int *)(&DAT_a0003300 + (uint)local_10 * 0x10) =
                 *(int *)(&DAT_a00032fc + (uint)local_10 * 0x10) * 10,
           (&DAT_a00159a3)[(uint)local_10 * 0xb] == '\x02')) {
          if ((local_10 == 0xf) && ((_g_sys_flags & 3) >> 1 != 1)) {
            FUN_9d01b014();
          }
          if ((DAT_a00159d8 & 1) != 0) {
            FUN_9d05d04c();
          }
          if ((DAT_a00159f9 & 1) != 0) {
            FUN_9d02472c();
          }
          if (((byte)(&DAT_a00159a2)[(uint)local_10 * 0xb] < 4) &&
             ((DAT_a0015a74 & 0x1fffff) >> 0x14 != 0)) {
            FUN_9d01ef08((&DAT_a00159a2)[(uint)local_10 * 0xb],local_10 + 0x80);
            FUN_9d007300((&DAT_a00159a2)[(uint)local_10 * 0xb],1,local_10);
          }
          if ((DAT_a00159e3 & 1) != 0) {
            for (local_f = 0; local_f < 8; local_f = local_f + 1) {
              *(undefined2 *)(&DAT_a0001234 + (uint)local_f * 8) = 0xd8fa;
              (&DAT_a0001236)[(uint)local_f * 8] = 0xff;
            }
          }
          FUN_9d0521c0(0x75,local_10);
          (&DAT_a00159a3)[(uint)local_10 * 0xb] = 1;
          if (((&DAT_a00159a4)[(uint)local_10 * 0xb] != '\0') && (param_1 != '\x01')) {
            FUN_9d003d84((&DAT_a00159a5)[(uint)local_10 * 0xb],(&DAT_a00159a4)[(uint)local_10 * 0xb]
                        );
          }
        }
      }
    }
  }
  return;
}

