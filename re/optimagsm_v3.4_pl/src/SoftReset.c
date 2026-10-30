// SoftReset @ 9d06cc10
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* PLIB: wstrzymanie DMA, odblokowanie SYSKEY, zapis RSWRST, restart procesora. */

void SoftReset(void)

{
  undefined4 uVar1;
  int iVar2;
  uint uVar3;
  
  FUN_9d06e628();
  iVar2 = DAT_bf883000;
  if (-1 < iVar2 << 0x13) {
    DAT_bf883008 = 0x1000;
    do {
      uVar3 = DAT_bf883000;
    } while ((uVar3 & 0x800) != 0);
  }
  DAT_bf80f230 = 0;
  DAT_bf80f230 = 0xaa996655;
  DAT_bf80f230 = 0x556699aa;
  DAT_bf80f618 = 1;
  uVar1 = DAT_bf80f610;
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}

