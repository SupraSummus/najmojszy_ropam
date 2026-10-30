// task_service @ 9d02b640
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* Zadanie RTOS: po starcie czeka, potem w pętli woła svc_dispatch. */

void task_service(void)

{
  int iVar1;
  undefined4 uStack_10;
  undefined1 uStack_c;
  undefined1 uStack_b;
  
  uStack_10 = 0;
  uStack_c = 0;
  uStack_b = 0;
  FUN_9d0168d4();
  os_delay(80000);
  svc_flush();
  do {
    iVar1 = svc_dispatch();
    if ((uStack_b != 1) && ((DAT_a000277e & 0x3f) >> 5 != 0)) {
      uStack_10 = 0;
    }
    if ((uStack_c != 1) && ((DAT_a000277c._1_1_ & 0x7f) >> 6 != 0)) {
      uStack_10 = 0;
    }
    if (iVar1 == 0) {
      if ((_DAT_a000015a & 3) >> 1 != 0) {
        FUN_9d054dac(1);
      }
      uStack_10 = uStack_10 + 1;
      if (((DAT_a000277c._1_1_ & 0x7f) >> 6 == 0) || (uStack_10 < 9)) {
        if (((DAT_a000277c._1_1_ & 0x7f) >> 6 != 1) &&
           ((2 < uStack_10 &&
            (_DAT_a000015a = _DAT_a000015a & 0xfffd, (DAT_a000277e & 0x3f) >> 5 != 0)))) {
          FUN_9d05f714();
        }
      }
      else {
        if ((DAT_a000277c._1_1_ & 0x7f) >> 6 != 0) {
          FUN_9d02fe3c();
        }
        DAT_a000277c._1_1_ = DAT_a000277c._1_1_ & 0xbf;
        _DAT_a000015a = _DAT_a000015a & 0xfffd;
      }
    }
    else {
      FUN_9d054e80(1);
      _DAT_a000015a = _DAT_a000015a & 0xfffd | 2;
      uStack_10 = 0;
    }
    uStack_c = (byte)(((uint)DAT_a000277c._1_1_ << 0x19) >> 0x1f);
    uStack_b = (byte)(((uint)DAT_a000277e << 0x1a) >> 0x1f);
  } while( true );
}

