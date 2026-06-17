#include <Arduino.h>
#include <CH9120.h>

constexpr int CH9120_CFG_PIN = 18;
constexpr int CH9120_RES_PIN = 19;
constexpr int UART_TX_PIN1 = 20;
constexpr int UART_RX_PIN1 = 21;

CH9120 ch9120(Serial2, CH9120_RES_PIN, CH9120_CFG_PIN);

CH9120Config cfg = {
  .mode = MODE_UDP_SERVER,
  .dhcpEnabled = true,
  .localIP = { 192, 168, 192, 7 },
  .subnetMask = { 255, 255, 255, 0 },
  .gateway = { 192, 168, 192, 1 },
  .localPort = 6454,
  .targetIP = { 192, 168, 192, 255 },
  .targetPort = 6454,
  .baudRate = 921600
};

void setup() {
  Serial.begin(921600);

  //Set RX TX Pin for CH9120
  Serial2.setTX(UART_TX_PIN1);
  Serial2.setRX(UART_RX_PIN1);

  ch9120.Init(cfg);
  delay(1000);
  ch9120.PrintConfig(Serial);
  Serial.println("CH9120 initialized");
}


void loop() {
  while (ch9120.GetStream().available()) {
    Serial.println("rx");
    Serial.write(ch9120.GetStream().read());
  }
}
