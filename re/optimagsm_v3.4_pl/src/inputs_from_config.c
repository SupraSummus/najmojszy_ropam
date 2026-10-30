// inputs_from_config @ 9d001ba0
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Przepisuje rekordy INPUT (168 B) do struktur roboczych wejść (40 B): typ, czasy, strefy, flagi.
    */

void inputs_from_config(void)

{
  ushort uVar1;
  int local_10;
  
  for (local_10 = 0; local_10 < 8; local_10 = local_10 + 1) {
    FUN_9d05c970(&DAT_a0002384);
    FUN_9d001a48(&DAT_a0002384);
  }
  FUN_9d0017a0();
  for (local_10 = 0; local_10 < 0x30; local_10 = local_10 + 1) {
    *(ushort *)((int)&g_inputs_rt + local_10 * 0x28 + 2) =
         *(ushort *)((int)&g_inputs_rt + local_10 * 0x28 + 2) & 0xffe0 |
         (byte)(&DAT_a00039a8)[local_10 * 0xa8] & 0x1f;
    *(undefined4 *)(&DAT_a0002b70 + local_10 * 0x28) =
         *(undefined4 *)(&DAT_a0003998 + local_10 * 0xa8);
    *(uint *)(&DAT_a0002b80 + local_10 * 0x28) =
         (uint)*(ushort *)(&DAT_a00039a2 + local_10 * 0xa8) * 1000;
    *(uint *)(&DAT_a0002b78 + local_10 * 0x28) =
         (uint)*(ushort *)(&DAT_a00039a0 + local_10 * 0xa8) * 60000;
    (&DAT_a0002b6f)[local_10 * 0x28] = (char)*(undefined2 *)(&DAT_a0003996 + local_10 * 0xa8);
    uVar1 = FUN_9d001490(local_10);
    *(ushort *)((int)&g_inputs_rt + local_10 * 0x28 + 2) =
         *(ushort *)((int)&g_inputs_rt + local_10 * 0x28 + 2) & 0xf0ff | (uVar1 & 0xf) << 8;
    *(ushort *)((int)&g_inputs_rt + local_10 * 0x28 + 2) =
         *(ushort *)((int)&g_inputs_rt + local_10 * 0x28 + 2) & 0xfff |
         (ushort)((((&g_inputs_rt)[local_10 * 10] & 0xfffffff) >> 0x18) << 0xc);
    *(ushort *)(&DAT_a0002b68 + local_10 * 0x28) =
         *(ushort *)(&DAT_a0002b68 + local_10 * 0x28) & 0xfffb |
         (ushort)((((&g_inputs_rt)[local_10 * 10] & 0xfffffff) >> 0x18 & 1) << 2);
    (&DAT_a0002b6b)[local_10 * 0x28] = (*(uint *)(&DAT_a00039a4 + local_10 * 0xa8) & 1) != 0;
    (&DAT_a0002b6c)[local_10 * 0x28] = (*(uint *)(&DAT_a00039a4 + local_10 * 0xa8) & 2) != 0;
    (&DAT_a0002b6d)[local_10 * 0x28] = (*(uint *)(&DAT_a00039a4 + local_10 * 0xa8) & 4) != 0;
    (&DAT_a0002b6e)[local_10 * 0x28] = (*(uint *)(&DAT_a00039a4 + local_10 * 0xa8) & 8) != 0;
    *(ushort *)(&DAT_a0002b68 + local_10 * 0x28) =
         *(ushort *)(&DAT_a0002b68 + local_10 * 0x28) & 0xdfff |
         (ushort)((*(uint *)(&DAT_a00039a4 + local_10 * 0xa8) & 0x10) != 0) << 0xd;
    *(ushort *)(&DAT_a0002b68 + local_10 * 0x28) =
         *(ushort *)(&DAT_a0002b68 + local_10 * 0x28) & 0xbfff |
         (ushort)((*(uint *)(&DAT_a00039a4 + local_10 * 0xa8) & 0x20) != 0) << 0xe;
    *(ushort *)(&DAT_a0002b68 + local_10 * 0x28) =
         *(ushort *)(&DAT_a0002b68 + local_10 * 0x28) & 0x7fff |
         (ushort)((*(uint *)(&DAT_a00039a4 + local_10 * 0xa8) & 0x40) != 0) << 0xf;
    (&DAT_a0002b6a)[local_10 * 0x28] =
         (&DAT_a0002b6a)[local_10 * 0x28] & 0xfe |
         (*(uint *)(&DAT_a00039a4 + local_10 * 0xa8) & 0x80) != 0;
    (&DAT_a0002b6a)[local_10 * 0x28] =
         (&DAT_a0002b6a)[local_10 * 0x28] & 0xfd |
         ((*(uint *)(&DAT_a00039a4 + local_10 * 0xa8) & 0x100) != 0) << 1;
    *(ushort *)(&DAT_a0002b68 + local_10 * 0x28) =
         *(ushort *)(&DAT_a0002b68 + local_10 * 0x28) & 0xefff;
    *(ushort *)(&DAT_a0002b68 + local_10 * 0x28) =
         *(ushort *)(&DAT_a0002b68 + local_10 * 0x28) & 0xfffe;
    *(ushort *)(&DAT_a0002b68 + local_10 * 0x28) =
         *(ushort *)(&DAT_a0002b68 + local_10 * 0x28) & 0xffbf;
    *(ushort *)(&DAT_a0002b68 + local_10 * 0x28) =
         *(ushort *)(&DAT_a0002b68 + local_10 * 0x28) & 0xffef;
    *(ushort *)(&DAT_a0002b68 + local_10 * 0x28) =
         *(ushort *)(&DAT_a0002b68 + local_10 * 0x28) & 0xffdf;
    *(ushort *)(&DAT_a0002b68 + local_10 * 0x28) =
         *(ushort *)(&DAT_a0002b68 + local_10 * 0x28) & 0xf7ff;
    *(ushort *)(&DAT_a0002b68 + local_10 * 0x28) =
         *(ushort *)(&DAT_a0002b68 + local_10 * 0x28) & 0xfcff;
    *(undefined4 *)(&DAT_a0002b88 + local_10 * 0x28) = 0;
  }
  return;
}

