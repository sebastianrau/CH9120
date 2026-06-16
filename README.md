# CH9120 Arduino Library

Arduino library for configuring and using the WCH CH9120 Ethernet-to-Serial module.

The library configures the CH9120 over UART, saves the module configuration to EEPROM, resets the module, and then exposes the configured transport UART stream for UDP/TCP data.

## Features

- Configure CH9120 operating mode: TCP server, TCP client, UDP server, or UDP client
- Enable or disable DHCP
- Set local IP address, subnet mask, and gateway for static IP use
- Set target IP address
- Set local and target UDP/TCP ports
- Set UART transport baud rate
- Read back common CH9120 settings
- Write bytes or buffers to the transport stream
- Print the active module configuration to any `Stream`
- Access the configured UART stream with `GetStream()`


## Modes

```cpp
MODE_TCP_SERVER
MODE_TCP_CLIENT
MODE_UDP_SERVER
MODE_UDP_CLIENT
```

## Configuration

Create a `CH9120Config` object and pass it to `Init()`.

```cpp

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

```

This example configuration is suitable for UDP broadcast communication, for example Art-Net on port `6454`.

When `dhcpEnabled` is `true`, `Init()` skips writing the static local IP, subnet mask, and gateway fields.

## Basic Example

```cpp
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
```

## API

### Constructor

```cpp
CH9120(HardwareSerial& serial, int resPin, int cfgPin);
```

Creates a CH9120 driver using the selected hardware serial port, reset pin, and configuration pin.

### `Init(config)`

```cpp
void Init(const CH9120Config& config);
```

Starts configuration mode at `9600` baud, writes the supplied settings, saves the configuration to EEPROM, resets the module, and then switches the serial port to `config.baudRate`.

### Configuration mode

```cpp
void StartConfig();
void EndConfig(bool reset = true);
```

Manually enter or leave CH9120 configuration mode.

### Mode

```cpp
bool GetMode(uint8_t* mode);
void SetMode(unsigned char mode);
```

Read or set the CH9120 operating mode.

### DHCP

```cpp
void SetDHCP(bool enable);
```

Enable or disable DHCP.

### Local network settings

```cpp
bool GetLocalIP(uint8_t ipAdr[4]);
void SetLocalIP(const unsigned char ipAdr[]);

bool GetSubnetMask(uint8_t subnetMask[4]);
void SetSubnetMask(const unsigned char subnetMask[]);

bool GetGateway(uint8_t gatewayAdr[4]);
void SetGateway(const unsigned char gatewayAdr[]);
```

Read or set local static network settings.

### Ports and target

```cpp
bool GetLocalPort(uint16_t* localPort);
void SetLocalPort(const uint16_t localPort);

bool GetTargetIP(uint8_t targetIpAdr[4]);
void SetTargetIP(const unsigned char targetIpAdr[]);

bool GetTargetPort(uint16_t* targetPort);
void SetTargetPort(const uint16_t targetPort);
```

Read or set local and target transport settings.

### Baud rate

```cpp
bool GetBaudRate(uint32_t* baud);
void SetBaudRate(const uint32_t baud);
```

Read or set the CH9120 UART transport baud rate.

### Stream and write helpers

```cpp
Stream& GetStream();

size_t Write(uint8_t byte);
size_t Write(const uint8_t* data, size_t size);
```

Access the configured transport stream directly or write bytes through the library helper methods.

### Configuration printout

```cpp
void PrintConfig(Stream& out);
```

Reads and prints the active module configuration to the supplied stream.

### Full configuration readback

```cpp
bool GetConfig(CH9120Config& config);
```

Reads the active module configuration into a `CH9120Config` structure.

## Notes

- Configuration mode uses `9600` baud.
- After configuration, the library switches the serial port to the configured transport baud rate.
- `PrintConfig()` temporarily enters configuration mode and then returns to the transport baud rate without resetting the module.
- Use a board with a hardware UART for best reliability at high baud rates such as `921600`.