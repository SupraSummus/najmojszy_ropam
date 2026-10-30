// Most Wi-Fi <-> port serwisowy (RJ12) centrali, Seeed XIAO ESP32-C6.
//
// Przezroczysty: bajty z połączenia TCP idą na UART i odwrotnie,
// więc tools/ropam_serwis.py działa przez sieć tak samo jak przez kabel:
//   python3 tools/ropam_serwis.py socket://ADRES:2323 id
//
// Połączenia (docs: esp32/README.md):
//   TX centrali -> D7 (RX), przez dzielnik, jeśli linia ma 5 V
//   RX centrali <- D6 (TX), przez rezystor 1 kΩ
//   GND -> GND
//
// Dane sieci Wi-Fi w sekrety.h (wzór: sekrety.h.przyklad, poza gitem).

#include <WiFi.h>

#include "sekrety.h"

const unsigned long PANEL_BAUD = 115200;
const uint16_t TCP_PORT = 2323;

WiFiServer server(TCP_PORT);
WiFiClient client;

void setup() {
  Serial.begin(115200);
  Serial1.setRxBufferSize(4096);
  Serial1.begin(PANEL_BAUD, SERIAL_8N1, D7, D6);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_HASLO);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print('.');
  }
  Serial.printf("\nadres: %s, port %u\n",
                WiFi.localIP().toString().c_str(), TCP_PORT);
  server.begin();
  server.setNoDelay(true);
}

void loop() {
  if (!client || !client.connected()) {
    client = server.accept();
    if (client) {
      client.setNoDelay(true);
      Serial.println("klient połączony");
    }
  }

  uint8_t buf[256];
  size_t n = Serial1.available();
  if (n) {
    n = Serial1.readBytes(buf, min(n, sizeof(buf)));
    // Bez klienta bajty (np. 'C' co 2 s) po prostu przepadają.
    if (client && client.connected()) {
      client.write(buf, n);
    }
  }
  if (client && client.connected()) {
    int m = client.available();
    if (m > 0) {
      m = client.read(buf, min((size_t)m, sizeof(buf)));
      Serial1.write(buf, m);
    }
  }
}
