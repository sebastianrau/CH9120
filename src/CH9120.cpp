#include "CH9120.h"
#include "Arduino.h"

CH9120::CH9120(HardwareSerial& serial, int resPin, int cfgPin)
  : serialPort(serial),
    resPin(resPin),
    cfgPin(cfgPin) {
}


bool CH9120::setParameter(uint8_t command) {

  uint8_t buffer[3];

  buffer[0] = 0x57;
  buffer[1] = 0xAB;
  buffer[2] = command;

  size_t written = serialPort.write(buffer, 3);
  serialPort.flush();
  delay(100);

  return written == 3;
}

bool CH9120::setParameter(uint8_t command, const uint8_t* data, size_t len) {
  uint8_t buffer[8];

  buffer[0] = 0x57;
  buffer[1] = 0xAB;
  buffer[2] = command;

  if (len > 0 && data == nullptr) return false;
  if (len + 3 > sizeof(buffer)) return false;

  memcpy(&buffer[3], data, len);

  size_t written = serialPort.write(buffer, len + 3);
  serialPort.flush();
  delay(100);

  return written == len + 3;
}

bool CH9120::readParameter(uint8_t command, uint8_t* data, size_t len) {
  if (!data || len == 0) return false;

  uint8_t cmd[3] = { 0x57, 0xAB, command };

  while (serialPort.available()) {
    serialPort.read();
  }

  serialPort.write(cmd, sizeof(cmd));
  serialPort.flush();

  return serialPort.readBytes(data, len) == len;
}


void CH9120::StartConfig() {
  serialPort.flush();
  serialPort.end();
  delay(20);
  serialPort.begin(9600);  // must be 9600
  serialPort.setTimeout(200);

  digitalWrite(cfgPin, LOW);
  digitalWrite(resPin, HIGH);
  delay(100);

  while (serialPort.available()) {
    serialPort.read();
  }
}
void CH9120::EndConfig(bool reset)
 {
    // Save to EEPROM
    setParameter(REG_SAVE_EEPROM);
    delay(200);

    if (reset)
    {
        setParameter(REG_EXECUTE_RESET);
        delay(500);
    }

    digitalWrite(cfgPin, HIGH);

    serialPort.end();
    delay(20);
    serialPort.begin(transportBaud);

}

void CH9120::Init(const CH9120Config& cfg) {
  transportBaud = cfg.baudRate;
  serialPort.begin(9600);  //fix baud rate

  pinMode(cfgPin, OUTPUT);
  pinMode(resPin, OUTPUT);

  StartConfig();
  delay(250);
  SetMode(cfg.mode);  //Mode
  SetDHCP(cfg.dhcpEnabled);
  if (!cfg.dhcpEnabled) {
    SetLocalIP((uint8_t*)cfg.localIP);        //LOCALIP
    SetSubnetMask((uint8_t*)cfg.subnetMask);  //SUBNET MASK
    SetGateway((uint8_t*)cfg.gateway);        //GATEWAY
  }
  SetTargetIP((uint8_t*)cfg.targetIP);  //TARGET IP
  SetLocalPort(cfg.localPort);          //Local port
  SetTargetPort(cfg.targetPort);        //Target port
  SetBaudRate(cfg.baudRate);            //Port 1 baud rate
  EndConfig(true);

  serialPort.begin(cfg.baudRate);
  while (serialPort.available()) {
    serialPort.read();
  }
}


bool CH9120::GetMode(uint8_t* mode) {
  return readParameter(REG_READ_MODE, mode, 1);
}
void CH9120::SetMode(unsigned char mode) {
  setParameter(REG_MODE1, &mode, 1);
}

void CH9120::SetDHCP(bool enable) {
  uint8_t value = enable ? 0x01 : 0x00;
  setParameter(REG_DHCP_ENABLE, &value, 1);
}

bool CH9120::GetLocalIP(uint8_t ipAdr[4]) {
  return readParameter(0x61, ipAdr, 4);
}
void CH9120::SetLocalIP(const unsigned char ipAdr[]) {
  setParameter(REG_LOCAL_IP, ipAdr, 4);
}

bool CH9120::GetSubnetMask(uint8_t subnetMask[4]) {
  return readParameter(0x62, subnetMask, 4);
}
void CH9120::SetSubnetMask(const unsigned char subnetMask[]) {
  setParameter(REG_SUBNET_MASK, subnetMask, 4);
}

bool CH9120::GetGateway(uint8_t gatewayAdr[4]) {
  return readParameter(0x63, gatewayAdr, 4);
}
void CH9120::SetGateway(const unsigned char gatewayAdr[]) {
  setParameter(REG_GATEWAY, gatewayAdr, 4);
}


bool CH9120::GetTargetIP(uint8_t targetIpAdr[4]) {
  return readParameter(0x65, targetIpAdr, 4);
}
void CH9120::SetTargetIP(const unsigned char targetIpAdr[]) {
  setParameter(REG_TARGET_IP1, targetIpAdr, 4);
}

bool CH9120::GetLocalPort(uint16_t* localPort) {
  uint8_t data[2];

  if (!readParameter(0x64, data, 2)) return false;

  *localPort = data[0] | (data[1] << 8);
  return true;
}
void CH9120::SetLocalPort(const uint16_t localPort) {
  uint8_t data[2] = {
    (uint8_t)(localPort & 0xFF),
    (uint8_t)(localPort >> 8)
  };

  setParameter(REG_LOCAL_PORT1, data, 2);
}

bool CH9120::GetTargetPort(uint16_t* targetPort) {
  uint8_t data[2];

  if (!targetPort) return false;
  if (!readParameter(0x66, data, 2)) return false;

  *targetPort = (uint16_t)data[0] | ((uint16_t)data[1] << 8);
  return true;
}
void CH9120::SetTargetPort(const uint16_t targetPort) {
  uint8_t data[2] = {
    (uint8_t)(targetPort & 0xFF),
    (uint8_t)(targetPort >> 8)
  };

  setParameter(REG_TARGET_PORT1, data, 2);
}

bool CH9120::GetBaudRate(uint32_t* baud) {
  uint8_t data[4];

  if (!readParameter(0x71, data, 4)) return false;

  *baud = (uint32_t)data[0]
          | ((uint32_t)data[1] << 8)
          | ((uint32_t)data[2] << 16)
          | ((uint32_t)data[3] << 24);

  return true;
}
void CH9120::SetBaudRate(const uint32_t baud) {
  uint8_t data[4] = {
    (uint8_t)(baud & 0xFF),
    (uint8_t)((baud >> 8) & 0xFF),
    (uint8_t)((baud >> 16) & 0xFF),
    (uint8_t)((baud >> 24) & 0xFF)
  };

  setParameter(REG_UART1_BAUD1, data, 4);
}

bool CH9120::GetMAC(uint8_t mac[6])
{
    return readParameter(0x81, mac, 6);
}




Stream& CH9120::GetStream() {
  return serialPort;
}

size_t CH9120::Write(uint8_t byte) {
  return serialPort.write(byte);
}


size_t CH9120::Write(const uint8_t* data, size_t size) {
  if (!data || size == 0) return 0;

  size_t total = 0;

  while (total < size) {
    int space = serialPort.availableForWrite();

    if (space > 0) {
      size_t chunk = size - total;
      if (chunk > (size_t)space) {
        chunk = space;
      }

      total += serialPort.write(data + total, chunk);
    }
  }

  serialPort.flush();
  return total;
}


void CH9120::PrintConfig(Stream& out) {
  bool dhcp;
  uint8_t mode;
  uint8_t ip[4];
  uint8_t mask[4];
  uint8_t gateway[4];
  uint8_t targetIp[4];

  uint16_t localPort;
  uint16_t targetPort;

  uint32_t baud;
  uint8_t mac[6];


  StartConfig();

  out.println(F("=== CH9120 Configuration ==="));

  if (GetMode(&mode)) {
    out.print(F("Mode: "));
    switch (mode) {
      case MODE_TCP_SERVER: out.println(F("TCP Server")); break;
      case MODE_TCP_CLIENT: out.println(F("TCP Client")); break;
      case MODE_UDP_SERVER: out.println(F("UDP Server")); break;
      case MODE_UDP_CLIENT: out.println(F("UDP Client")); break;
      default: out.println(mode); break;
    }
  }

  if (GetLocalIP(ip)) {
    out.print(F("Local IP: "));
    out.printf("%u.%u.%u.%u\n", ip[0], ip[1], ip[2], ip[3]);
  }

  if (GetSubnetMask(mask)) {
    out.print(F("Subnet: "));
    out.printf("%u.%u.%u.%u\n", mask[0], mask[1], mask[2], mask[3]);
  }

  if (GetGateway(gateway)) {
    out.print(F("Gateway: "));
    out.printf("%u.%u.%u.%u\n", gateway[0], gateway[1], gateway[2], gateway[3]);
  }

  if (GetLocalPort(&localPort)) {
    out.print(F("Local Port: "));
    out.println(localPort);
  }

  if (GetTargetIP(targetIp)) {
    out.print(F("Target IP: "));
    out.printf("%u.%u.%u.%u\n", targetIp[0], targetIp[1], targetIp[2], targetIp[3]);
  }

  if (GetTargetPort(&targetPort)) {
    out.print(F("Target Port: "));
    out.println(targetPort);
  }

  if (GetBaudRate(&baud)) {
    out.print(F("Baud Rate: "));
    out.println(baud);
  }

  if (GetMAC(mac))
{
  out.print(F("MAC: "));
  out.printf(
        "%02X:%02X:%02X:%02X:%02X:%02X\n",
        mac[0], mac[1], mac[2],
        mac[3], mac[4], mac[5]);
}


  EndConfig(false);

  out.println(F("============================"));
}
