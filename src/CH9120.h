#pragma once

#include <Arduino.h>
#include <stdlib.h>
#include <stdio.h>


#define MODE_TCP_SERVER 0
#define MODE_TCP_CLIENT 1
#define MODE_UDP_SERVER 2
#define MODE_UDP_CLIENT 3

#define REG_MODE1 0x10                //Port 1: Setup Mode   0x00:TCP Server 0x01:TCP Client 0x02:UDP Server 0x03:UDP Client
#define REG_LOCAL_IP 0x11             //Local IP
#define REG_SUBNET_MASK 0x12          //Subnet Mask
#define REG_GATEWAY 0x13              //Gateway
#define REG_LOCAL_PORT1 0X14          //Port 1:Local Port
#define REG_TARGET_IP1 0x15           //Port 1:Target IP
#define REG_TARGET_PORT1 0x16         //Port 1:Target Port
#define REG_PORT_RANDOM_ENABLE1 0x17  //Port 1:Port Random Enable
#define REG_UART1_BAUD1 0x21          //Port 1:Baud rate of serial port 1
#define REG_DHCP_ENABLE 0x33          // Enable DHCP

#define REG_SAVE_EEPROM       0x0D
#define REG_EXECUTE_RESET     0x0E
#define REG_LEAVE_CONFIG      0x5E

#define REG_READ_MODE         0x60
#define REG_READ_LOCAL_IP     0x61
#define REG_READ_SUBNET_MASK  0x62
#define REG_READ_GATEWAY      0x63
#define REG_READ_LOCAL_PORT   0x64
#define REG_READ_TARGET_IP    0x65
#define REG_READ_TARGET_PORT  0x66
#define REG_READ_BAUD_RATE    0x71

typedef struct
{
  uint8_t mode;
  bool dhcpEnabled;
  uint8_t localIP[4];
  uint8_t subnetMask[4];
  uint8_t gateway[4];
  uint16_t localPort;
  uint8_t targetIP[4];
  uint16_t targetPort;
  uint32_t baudRate;
} CH9120Config;



class CH9120 {

public:

  CH9120(HardwareSerial& serial, int resPin, int cfgPin);

  void Init(const CH9120Config& config);

  void StartConfig();
  void EndConfig(bool reset = true);


  bool GetMode(uint8_t* mode);
  void SetMode(unsigned char Mode);

  void SetDHCP(bool enable);

  bool GetLocalIP(uint8_t ipAdr[4]);
  void SetLocalIP(const unsigned char ipAdr[]);

  bool GetLocalPort(uint16_t* localPort);
  void SetLocalPort(const uint16_t localPort);

  bool GetSubnetMask(uint8_t subnetMask[4]);
  void SetSubnetMask(const unsigned char subnetMask[]);

  bool GetGateway(uint8_t gatewayAdr[4]);
  void SetGateway(const unsigned char gatewayAdr[]);

  bool GetTargetIP(uint8_t targetIpAdr[4]);
  void SetTargetIP(const unsigned char targetIpAdr[]);

  bool GetTargetPort(uint16_t* targetPort);
  void SetTargetPort(const uint16_t targetPort);

  bool GetBaudRate(uint32_t* baud);
  void SetBaudRate(const uint32_t baud);

  bool GetMAC(uint8_t mac[6]);

  Stream& GetStream();

  size_t Write(uint8_t byte);
  size_t Write(const uint8_t* data, size_t size);

  void PrintConfig(Stream& out);


private:
  HardwareSerial& serialPort;
  int resPin;
  int cfgPin;
  uint32_t transportBaud = 9600;
  bool dhcpEnabled = false;

  bool setParameter(uint8_t command);
  bool setParameter(uint8_t command, const uint8_t* data, size_t len);
  bool readParameter(uint8_t command, uint8_t* data, size_t len);
};
