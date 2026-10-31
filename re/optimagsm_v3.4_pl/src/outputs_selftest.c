// outputs_selftest @ 9d03da1c
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Test wyjść i sygnalizacji po skasowaniu dziennika (komenda 0x05). */

void outputs_selftest(void)

{
  int local_10;
  int local_c;
  
  for (local_c = 0; local_c < 3; local_c = local_c + 1) {
    for (local_10 = 0; local_10 < 8; local_10 = local_10 + 1) {
      output_force_off(local_10);
    }
    os_delay(5000);
    for (local_10 = 0; local_10 < 8; local_10 = local_10 + 1) {
      output_force_on(local_10);
      *(undefined4 *)(&DAT_a0002950 + local_10 * 0x10) = 0;
      os_delay(5000);
    }
    FUN_9d054dac(4);
    FUN_9d054dac(2);
    FUN_9d054dac(3);
    FUN_9d054dac(0);
    FUN_9d054dac(1);
    FUN_9d054dac(5);
    FUN_9d054dac(1);
    os_delay(5000);
    FUN_9d054d48(4);
    os_delay(2000);
    FUN_9d054d48(2);
    os_delay(2000);
    FUN_9d054d48(5);
    os_delay(2000);
    FUN_9d054d48(3);
    os_delay(2000);
    FUN_9d054d48(0);
    os_delay(2000);
    FUN_9d054d48(1);
    os_delay(2000);
    FUN_9d054d48(5);
    os_delay(2000);
    FUN_9d054d48(1);
  }
  FUN_9d03aac0(0x31);
  return;
}

