// input_react @ 9d005394
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Reakcja na zmianę wejścia wg typu (switch 0–10) i stanu stref; zapisuje zdarzenia (1, 5, 6,
   7, 8, 10, 0x0B, 0x0C). */

uint * input_react(uint *param_1,int param_2,char *param_3,uint *param_4,undefined1 *param_5)

{
  char cVar1;
  char cVar2;
  bool bVar3;
  bool bVar4;
  bool bVar5;
  bool bVar6;
  char cVar7;
  int iVar8;
  uint uVar9;
  
  if ((DAT_a0000168 != '\0') && ((DAT_a0015a74 & 0xff) * 1000 <= DAT_a0000324)) {
    DAT_a0000169 = '\0';
    DAT_a0000168 = '\0';
  }
  cVar1 = *param_3;
  cVar2 = param_3[1];
  cVar7 = (&DAT_a0002b6f)[cVar1 * 0x28];
  if ((*(uint *)(&DAT_a0002b68 + cVar1 * 0x28) & 0xff) >> 7 != 0) {
    *param_5 = 0;
    uVar9 = (param_4 + param_2 * 2)[1];
    *param_1 = param_4[param_2 * 2];
    param_1[1] = uVar9;
    return param_1;
  }
  uVar9 = (uint)(byte)(&DAT_a0002b6b)[cVar1 * 0x28] | (uint)(byte)(&DAT_a0002b6c)[cVar1 * 0x28] << 1
          | (uint)(byte)(&DAT_a0002b6d)[cVar1 * 0x28] << 2 |
          (uint)(byte)(&DAT_a0002b6e)[cVar1 * 0x28] << 3;
  bVar3 = (((*param_4 & 0x7ff) >> 10 | ((param_4[2] & 0x7ff) >> 10) << 1 |
            ((param_4[4] & 0x7ff) >> 10) << 2 | ((param_4[6] & 0x7ff) >> 10) << 3) & uVar9) == uVar9
  ;
  bVar4 = (((*param_4 & 0xfff) >> 0xb | ((param_4[2] & 0xfff) >> 0xb) << 1 |
            ((param_4[4] & 0xfff) >> 0xb) << 2 | ((param_4[6] & 0xfff) >> 0xb) << 3) & uVar9) ==
          uVar9;
  bVar5 = bVar3 || bVar4;
  iVar8 = FUN_9d005010(param_2,(int)cVar1);
  if ((iVar8 == 0) ||
     (((cVar7 != '\x01' && (bVar4)) &&
      ((*(uint *)(&DAT_a0002b68 + cVar1 * 0x28) & 0x3fff) >> 0xd == 0)))) {
    bVar6 = false;
  }
  else {
    bVar6 = true;
  }
  if (bVar6) {
    if (cVar2 == '\x04') {
      event_log(0x11,cVar1);
    }
    else {
      if (cVar2 != '\x03') goto code_r0x9d005868;
      FUN_9d01ef08(param_2,cVar1 + 0x40);
      event_log(0x10,cVar1);
      param_4[param_2 * 2] = param_4[param_2 * 2] & 0xfffeffff | 0x10000;
      if (((*(uint *)(&DAT_a0002b68 + cVar1 * 0x28) & 0x7fff) >> 0xe != 0) ||
         ((DAT_a0015a74 & 0xfff) >> 0xb != 0)) {
        if ((param_4[param_2 * 2] & 0xf) == 3) {
          param_4[param_2 * 2] = param_4[param_2 * 2] & 0xfffffdff | 0x200;
        }
        param_4[param_2 * 2] = param_4[param_2 * 2] & 0xfffffff0 | 1;
        param_4[param_2 * 2 + 1] = 1000;
      }
    }
LAB_9d006e80:
    *param_5 = 1;
    uVar9 = (param_4 + param_2 * 2)[1];
    *param_1 = param_4[param_2 * 2];
    param_1[1] = uVar9;
  }
  else {
code_r0x9d005868:
    switch(cVar7) {
    case '\0':
      if ((*(uint *)(&DAT_a0002b68 + cVar1 * 0x28) & 0x7f) >> 6 == 0) {
        *(ushort *)(&DAT_a0002b68 + cVar1 * 0x28) =
             *(ushort *)(&DAT_a0002b68 + cVar1 * 0x28) & 0xffbf | 0x40;
        *(undefined4 *)(&DAT_a0002b7c + cVar1 * 0x28) = 0;
        if ((cVar2 == '\x01') && ((*(uint *)(&DAT_a00039a4 + cVar1 * 0xa8) & 0x200) == 0)) {
          event_log(7,cVar1);
        }
        goto LAB_9d006e80;
      }
      break;
    case '\x01':
      if (((bVar6) && ((*(uint *)(&DAT_a0002b68 + cVar1 * 0x28) & 0x7f) >> 6 == 0)) &&
         ((*(uint *)(&DAT_a0002b68 + cVar1 * 0x28) & 0xfff) >> 0xb == 0)) {
        *(ushort *)(&DAT_a0002b68 + cVar1 * 0x28) =
             *(ushort *)(&DAT_a0002b68 + cVar1 * 0x28) & 0xffbf | 0x40;
        *(undefined4 *)(&DAT_a0002b7c + cVar1 * 0x28) = 0;
        if (cVar2 == '\x01') {
          FUN_9d004f34((int)cVar1,uVar9,param_4);
          event_log(5,cVar1);
        }
        goto LAB_9d006e80;
      }
      break;
    case '\x02':
      if ((((bVar5) && (bVar6)) &&
          ((((param_4[param_2 * 2] & 0xf) == 2 ||
            (((param_4[param_2 * 2] & 0xf) == 6 || ((param_4[param_2 * 2] & 0xf) == 4)))) &&
           ((*(uint *)(&DAT_a0002b68 + cVar1 * 0x28) & 0x7f) >> 6 == 0)))) &&
         ((*(uint *)(&DAT_a0002b68 + cVar1 * 0x28) & 0xfff) >> 0xb == 0)) {
        *(ushort *)(&DAT_a0002b68 + cVar1 * 0x28) =
             *(ushort *)(&DAT_a0002b68 + cVar1 * 0x28) & 0xffbf | 0x40;
        *(undefined4 *)(&DAT_a0002b7c + cVar1 * 0x28) = 0;
        if ((param_4[param_2 * 2] & 0xf) == 4) {
          FUN_9d0030a0(&DAT_a0000314);
        }
        if (cVar2 == '\x01') {
          FUN_9d004f34((int)cVar1,uVar9,param_4);
          event_log(1,cVar1);
        }
        goto LAB_9d006e80;
      }
      break;
    case '\x03':
      if (((((bVar5) && (bVar6)) &&
           (((param_4[param_2 * 2] & 0xf) == 2 ||
            (((param_4[param_2 * 2] & 0xf) == 4 || ((param_4[param_2 * 2] & 0xf) == 6)))))) &&
          ((*(uint *)(&DAT_a0002b68 + cVar1 * 0x28) & 0x7f) >> 6 == 0)) &&
         ((*(uint *)(&DAT_a0002b68 + cVar1 * 0x28) & 0xfff) >> 0xb == 0)) {
        *(ushort *)(&DAT_a0002b68 + cVar1 * 0x28) =
             *(ushort *)(&DAT_a0002b68 + cVar1 * 0x28) & 0xffbf | 0x40;
        *(undefined4 *)(&DAT_a0002b7c + cVar1 * 0x28) = 0;
        if ((param_4[param_2 * 2] & 0xf) == 4) {
          FUN_9d0030a0(&DAT_a0000314);
        }
        if (cVar2 == '\x01') {
          event_log(6,cVar1);
        }
        goto LAB_9d006e80;
      }
      break;
    case '\x04':
      if (bVar6) {
        cVar7 = zone_outputs_off(param_2);
        if (cVar2 == '\x01') {
          param_4[param_2 * 2] = param_4[param_2 * 2] & 0xfffffeff;
          param_4[param_2 * 2] = param_4[param_2 * 2] & 0xfffffdff;
          if ((DAT_a0015a74 & 0x3fff) >> 0xd == 0) {
            if (((param_4[param_2 * 2] & 0xf) == 3) && (cVar7 != '\x01')) {
              FUN_9d00440c(param_2,0);
              FUN_9d0044e8(1,(*(uint *)(&DAT_a0002b68 + cVar1 * 0x28) & 0x3fff) >> 0xd,param_2);
              param_4[param_2 * 2] = param_4[param_2 * 2] & 0xfffffff0 | 5;
              param_4[param_2 * 2 + 1] = *(int *)(&DAT_a00038d8 + param_2 * 0x2c) * 1000;
              DAT_a0000314 = cVar1;
              param_4[param_2 * 2] =
                   param_4[param_2 * 2] & 0xfffff7ff |
                   ((*(uint *)(&DAT_a0002b68 + cVar1 * 0x28) & 0x3fff) >> 0xd) << 0xb;
              FUN_9d05205c(3,1,param_2,(int)cVar1,
                           (*(uint *)(&DAT_a0002b68 + cVar1 * 0x28) & 0x3fff) >> 0xd);
              goto LAB_9d006e80;
            }
          }
          else if ((param_4[param_2 * 2] & 0xf) == 3) {
            if (cVar7 != '\x01') {
              FUN_9d00440c(param_2,0);
              FUN_9d0044e8(1,(*(uint *)(&DAT_a0002b68 + cVar1 * 0x28) & 0x3fff) >> 0xd,param_2);
              param_4[param_2 * 2] = param_4[param_2 * 2] & 0xfffffff0 | 5;
              param_4[param_2 * 2 + 1] = *(int *)(&DAT_a00038d8 + param_2 * 0x2c) * 1000;
              DAT_a0000314 = cVar1;
              param_4[param_2 * 2] =
                   param_4[param_2 * 2] & 0xfffff7ff |
                   ((*(uint *)(&DAT_a0002b68 + cVar1 * 0x28) & 0x3fff) >> 0xd) << 0xb;
              FUN_9d05205c(3,1,param_2,(int)cVar1,
                           (*(uint *)(&DAT_a0002b68 + cVar1 * 0x28) & 0x3fff) >> 0xd);
              goto LAB_9d006e80;
            }
          }
          else if ((((param_4[param_2 * 2] & 0xf) == 2) || ((param_4[param_2 * 2] & 0xf) == 6)) ||
                  (((param_4[param_2 * 2] & 0xf) == 1 ||
                   (((param_4[param_2 * 2] & 0xf) == 5 || ((param_4[param_2 * 2] & 0xf) == 4)))))) {
            FUN_9d003f88(param_2,param_4);
            FUN_9d0041bc(1,param_2,param_4 + param_2 * 2,(int)cVar1,0);
            param_4[param_2 * 2] = param_4[param_2 * 2] & 0xffffefff | 0x1000;
            goto LAB_9d006e80;
          }
        }
        else {
          if ((DAT_a0015a74 & 0x3fff) >> 0xd != 0) {
            uVar9 = (param_4 + param_2 * 2)[1];
            *param_1 = param_4[param_2 * 2];
            param_1[1] = uVar9;
            return param_1;
          }
          if (((((param_4[param_2 * 2] & 0xf) == 2) || ((param_4[param_2 * 2] & 0xf) == 6)) ||
              ((param_4[param_2 * 2] & 0xf) == 1)) ||
             (((param_4[param_2 * 2] & 0xf) == 5 || ((param_4[param_2 * 2] & 0xf) == 4)))) {
            FUN_9d003f88(param_2,param_4);
            FUN_9d0041bc(1,param_2,param_4 + param_2 * 2,(int)cVar1,0);
            param_4[param_2 * 2] = param_4[param_2 * 2] & 0xffffefff | 0x1000;
            goto LAB_9d006e80;
          }
        }
      }
      break;
    case '\x05':
      if ((((bVar5) && (bVar6)) && ((*(uint *)(&DAT_a0002b68 + cVar1 * 0x28) & 0x7f) >> 6 == 0)) &&
         ((*(uint *)(&DAT_a0002b68 + cVar1 * 0x28) & 0xfff) >> 0xb == 0)) {
        if (((param_4[param_2 * 2] & 0xf) == 2) || ((param_4[param_2 * 2] & 0xf) == 6)) {
          if (cVar2 == '\x01') {
            if (((DAT_a0015a74 & 0x7ff) >> 10 != 0) && ((param_4[param_2 * 2] & 0xf) == 6)) {
              FUN_9d004f34((int)cVar1,uVar9,param_4);
              event_log(1,cVar1);
              goto LAB_9d006e80;
            }
            if ((param_4[param_2 * 2] & 0x7fff) >> 0xe != 1) {
              event_log(0xb,cVar1);
              param_4[param_2 * 2] = param_4[param_2 * 2] & 0xfffffff0 | 4;
              param_4[param_2 * 2 + 1] = *(int *)(&DAT_a00038d4 + param_2 * 0x2c) * 1000;
              param_4[param_2 * 2] = param_4[param_2 * 2] & 0xffffbfff | 0x4000;
              DAT_a0000314 = cVar1;
              DAT_a0000315 = cVar2;
              if (*(int *)(&DAT_a0002b80 + cVar1 * 0x28) != 0) {
                *(undefined4 *)(&DAT_a0002b84 + cVar1 * 0x28) =
                     *(undefined4 *)(&DAT_a0002b80 + cVar1 * 0x28);
                *(ushort *)(&DAT_a0002b68 + cVar1 * 0x28) =
                     *(ushort *)(&DAT_a0002b68 + cVar1 * 0x28) & 0xfbff | 0x400;
                param_4[param_2 * 2] = param_4[param_2 * 2] & 0xffffdfff | 0x2000;
              }
            }
          }
          else if ((param_4[param_2 * 2] & 0x7fff) >> 0xe != 0) goto LAB_9d006e80;
        }
        else if ((param_4[param_2 * 2] & 0xf) == 4) {
          *(ushort *)(&DAT_a0002b68 + cVar1 * 0x28) =
               *(ushort *)(&DAT_a0002b68 + cVar1 * 0x28) & 0xffbf | 0x40;
          *(undefined4 *)(&DAT_a0002b7c + cVar1 * 0x28) = 0;
          if (cVar2 == '\x01') {
            event_log(0xb,cVar1);
          }
          else {
            event_log(2,cVar1);
          }
        }
      }
      break;
    case '\x06':
      if (((bVar5) && (bVar6)) &&
         (((*(uint *)(&DAT_a0002b68 + cVar1 * 0x28) & 0x7f) >> 6 == 0 &&
          ((*(uint *)(&DAT_a0002b68 + cVar1 * 0x28) & 0xfff) >> 0xb == 0)))) {
        if (((param_4[param_2 * 2] & 0xf) == 2) || ((param_4[param_2 * 2] & 0xf) == 6)) {
          *(ushort *)(&DAT_a0002b68 + cVar1 * 0x28) =
               *(ushort *)(&DAT_a0002b68 + cVar1 * 0x28) & 0xffbf | 0x40;
          *(undefined4 *)(&DAT_a0002b7c + cVar1 * 0x28) = 0;
          if (cVar2 == '\x01') {
            FUN_9d004f34((int)cVar1,uVar9,param_4);
            event_log(1,cVar1);
          }
          goto LAB_9d006e80;
        }
        if ((param_4[param_2 * 2] & 0xf) == 4) {
          *(ushort *)(&DAT_a0002b68 + cVar1 * 0x28) =
               *(ushort *)(&DAT_a0002b68 + cVar1 * 0x28) & 0xffbf | 0x40;
          *(undefined4 *)(&DAT_a0002b7c + cVar1 * 0x28) = 0;
          if (cVar2 == '\x01') {
            DAT_a0000314 = cVar1;
            DAT_a0000315 = cVar2;
            event_log(0xc,cVar1);
          }
          else {
            event_log(2,cVar1);
          }
        }
      }
      break;
    case '\a':
      if ((((bVar3 || bVar4) && (bVar6)) &&
          (((param_4[param_2 * 2] & 0xf) == 2 || ((param_4[param_2 * 2] & 0xf) == 6)))) &&
         (cVar2 == '\x01')) {
        DAT_a0000169 = DAT_a0000169 + '\x01';
        if ((int)(uint)DAT_a0015a73 <= (int)DAT_a0000169) {
          event_log(8,cVar1);
          DAT_a0000169 = '\0';
          FUN_9d004f34((int)cVar1,uVar9,param_4);
          goto LAB_9d006e80;
        }
        DAT_a0000168 = '\x01';
        DAT_a0000324 = 0;
      }
      break;
    case '\b':
      if (((bVar6) && (cVar2 == '\x01')) && (iVar8 = zone_outputs_off(param_2), iVar8 != 0)) {
        event_log(10,cVar1);
        goto LAB_9d006e80;
      }
      break;
    case '\t':
      if (bVar6) {
        param_4[param_2 * 2] = param_4[param_2 * 2] & 0xfffffeff;
        param_4[param_2 * 2] = param_4[param_2 * 2] & 0xfffffdff;
        if ((cVar2 == '\x01') && ((param_4[param_2 * 2] & 0xf) == 3)) {
          FUN_9d00440c(param_2,0);
          FUN_9d0044e8(1,(*(uint *)(&DAT_a0002b68 + cVar1 * 0x28) & 0x3fff) >> 0xd,param_2);
          param_4[param_2 * 2] = param_4[param_2 * 2] & 0xfffffff0 | 5;
          param_4[param_2 * 2 + 1] = *(int *)(&DAT_a00038d8 + param_2 * 0x2c) * 1000;
          DAT_a0000314 = cVar1;
          if ((*(uint *)(&DAT_a0002b68 + cVar1 * 0x28) & 0x3fff) >> 0xd != 0) {
            param_4[param_2 * 2] = param_4[param_2 * 2] & 0xfffffff0 | 6;
          }
          FUN_9d05205c(3,1,param_2,(int)cVar1,
                       (*(uint *)(&DAT_a0002b68 + cVar1 * 0x28) & 0x3fff) >> 0xd);
          goto LAB_9d006e80;
        }
      }
      break;
    case '\n':
      if (bVar6) {
        param_4[param_2 * 2] = param_4[param_2 * 2] & 0xfffffeff;
        param_4[param_2 * 2] = param_4[param_2 * 2] & 0xfffffdff;
        if ((cVar2 == '\x01') &&
           ((((param_4[param_2 * 2] & 0xf) == 2 || ((param_4[param_2 * 2] & 0xf) == 1)) ||
            (((param_4[param_2 * 2] & 0xf) == 5 || ((param_4[param_2 * 2] & 0xf) == 4)))))) {
          FUN_9d003f88(param_2,param_4);
          FUN_9d0041bc(1,param_2,param_4 + param_2 * 2,(int)cVar1,0);
          param_4[param_2 * 2] = param_4[param_2 * 2] & 0xffffefff | 0x1000;
          goto LAB_9d006e80;
        }
      }
    }
    *param_5 = 0;
    uVar9 = (param_4 + param_2 * 2)[1];
    *param_1 = param_4[param_2 * 2];
    param_1[1] = uVar9;
  }
  return param_1;
}

