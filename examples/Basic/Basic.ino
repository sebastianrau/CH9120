#include <CH9120.h>

constexpr int CH9120_RES_PIN = 4;
constexpr int CH9120_CFG_PIN = 5;

CH9120 ch9120(Serial1, CH9120_RES_PIN, CH9120_CFG_PIN);

CH9120Config cfg = {
  .mode = MODE_UDP_SERVER,
  .dhcpEnabled = true,
  .localIP = { 192, 168, 0, 7 },
  .subnetMask = { 255, 255, 255, 0 },
  .gateway = { 192, 168, 0, 1 },
  .localPort = 36666,
  .targetIP = { 192, 168, 0, 255 },
  .targetPort = 36666,
  .baudRate = 921600
};

void setup() {
  Serial.begin(115200);
  
  //Set RX TX Pin for CH9120
  Serial2.setTX(UART_TX_PIN1);
  Serial2.setRX(UART_RX_PIN1);
 
  ch9120.Init(config);
  ch9120.PrintConfig(Serial);

  Serial.println("CH9120 initialized");
}

void loop() {
  Stream& eth = ch9120.GetStream();

  while (eth.available()) {
    Serial.write(eth.read());
  }
}
