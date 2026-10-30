// Bierny podsłuch magistrali RopamNET na Seeed XIAO ESP32-C6.
//
// Transceiver RS-485 (3,3 V, np. MAX3485) tylko na odbiór:
//   RO -> D7 (RX), DE i /RE na stałe do GND, A/B/GND do magistrali.
// Bajty z magistrali idą bez zmian na USB (Serial),
// skąd zapisuje je np. `cat /dev/ttyACM0 > zapis.bin`.
// Analiza: tools/ropamnet.py analizuj zapis.bin

const unsigned long BUS_BAUD = 115200;

void setup() {
  Serial.begin(115200);  // USB CDC; prędkość nie ma tu znaczenia
  Serial1.setRxBufferSize(4096);
  Serial1.begin(BUS_BAUD, SERIAL_8N1, D7, D6);
}

void loop() {
  uint8_t buf[256];
  size_t n = Serial1.available();
  if (n) {
    n = Serial1.readBytes(buf, min(n, sizeof(buf)));
    Serial.write(buf, n);
  }
}
