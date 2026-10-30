// uart_putc @ 9d050870
// Wygenerowane przez tools/ghidra/analyze.sh; nie edytować ręcznie.

/* Wysyłka bajtu na port (aktywne czekanie na TRMT). */

void uart_putc(int param_1,byte param_2)

{
  char cVar1;
  
  cVar1 = (char)*(undefined4 *)(&DAT_9d06e088 + param_1 * 4);
  do {
  } while ((*(uint *)((&uart_base_table)[cVar1] + 0x10) & 0x200) != 0);
  *(uint *)((&uart_base_table)[cVar1] + 0x20) = (uint)param_2;
  do {
  } while ((*(uint *)((&uart_base_table)[cVar1] + 0x10) & 0x100) == 0);
  return;
}

