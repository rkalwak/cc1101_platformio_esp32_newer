#include "Arduino.h"
#include "crc.hpp"
#include "mbus_packet.hpp"
#include "aes.hpp"
#include "3outof6.hpp"
#include "mbus_defs.hpp"
#include "tmode_rf_settings.hpp"
#include "rf_mbus.hpp"
#include "SensorInfo.h"
#include "WaterMeter.h"
#include <SuplaDevice.h>
#include <Drivers/drivers.h>
#include <supla/network/esp_wifi.h>
Supla::ESPWifi wifi(wifiSSIDString, wifiPasswordString);
Supla::Sensor::WaterMeter *meter;
void setup()
{
  Serial.begin(115200);
  delay(5000);
  Serial.println("----------------Example sensors decoding-------------");

  Serial.println("1. Apator 16-2 long frame");
  auto meterT = new Supla::Sensor::WaterMeter(0);
  meterT->add_driver(new Apator162());
  // 00000000000000000000000000000000
  std::vector<unsigned char> keyT{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  // 6E4401068798710205070F977AF70060852585B973EF73E80EB54F6FE2A6344641920366F1FA4E23813A35D994D0BE59372C9FCCD117B03200E9B825C56065AC3FD2F648D935EFF04D7E1DFA72C0803AC667FB1F5A5DAB4387643A22E68894C7356D2A9D49A28B1A86235ED4C7E367D2DE9F98BCB7954E10C7F770B8DFC1610000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
  uint8_t apatorminarray[254] = {0x6E, 0x44, 0x01, 0x06, 0x87, 0x98, 0x71, 0x02, 0x05, 0x07, 0x0F, 0x97, 0x7A, 0xF7, 0x00, 0x60, 0x85, 0x25, 0x85, 0xB9, 0x73, 0xEF, 0x73, 0xE8, 0x0E, 0xB5, 0x4F, 0x6F, 0xE2, 0xA6, 0x34, 0x46, 0x41, 0x92, 0x03, 0x66, 0xF1, 0xFA, 0x4E, 0x23, 0x81, 0x3A, 0x35, 0xD9, 0x94, 0xD0, 0xBE, 0x59, 0x37, 0x2C, 0x9F, 0xCC, 0xD1, 0x17, 0xB0, 0x32, 0x00, 0xE9, 0xB8, 0x25, 0xC5, 0x60, 0x65, 0xAC, 0x3F, 0xD2, 0xF6, 0x48, 0xD9, 0x35, 0xEF, 0xF0, 0x4D, 0x7E, 0x1D, 0xFA, 0x72, 0xC0, 0x80, 0x3A, 0xC6, 0x67, 0xFB, 0x1F, 0x5A, 0x5D, 0xAB, 0x43, 0x87, 0x64, 0x3A, 0x22, 0xE6, 0x88, 0x94, 0xC7, 0x35, 0x6D, 0x2A, 0x9D, 0x49, 0xA2, 0x8B, 0x1A, 0x86, 0x23, 0x5E, 0xD4, 0xC7, 0xE3, 0x67, 0xD2, 0xDE, 0x9F, 0x98, 0xBC, 0xB7, 0x95, 0x4E, 0x10, 0xC7, 0xF7, 0x70, 0xB8, 0xDF, 0xC1, 0x61, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  meterT->add_sensor(new Supla::Sensor::SensorBase("02719887", "apator162", "total_water_m3", keyT));
  uint8_t len_without_crc = crcRemove(apatorminarray, packetSize(apatorminarray[0]));
  std::vector<unsigned char> frameApatorMine(apatorminarray, apatorminarray + len_without_crc);
  float readValue = meterT->parse_frame(frameApatorMine);
  Serial.print(readValue);
  Serial.println("m3");

  Serial.println("2. Iperl");
  auto meterI = new Supla::Sensor::WaterMeter(0);
  meterI->add_driver(new Iperl());
  // E6C88800DEB868C0D6A84880CE982840
  std::vector<unsigned char> keyIperl{0xE6, 0xC8, 0x88, 0x00, 0xDE, 0xB8, 0x68, 0xC0, 0xD6, 0xA8, 0x48, 0x80, 0xCE, 0x98, 0x28, 0x40};
  // 1E44AE4C27093321680749A17AC400100561EBBE1A7896F68423B8FE0E88410406A25BE22A0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
  uint8_t iperlarray[254] = {0x1E, 0x44, 0xAE, 0x4C, 0x27, 0x09, 0x33, 0x21, 0x68, 0x07, 0x49, 0xA1, 0x7A, 0xC4, 0x00, 0x10, 0x05, 0x61, 0xEB, 0xBE, 0x1A, 0x78, 0x96, 0xF6, 0x84, 0x23, 0xB8, 0xFE, 0x0E, 0x88, 0x41, 0x04, 0x06, 0xA2, 0x5B, 0xE2, 0x2A,0x00,0x00,0x00,0x00};
  meterI->add_sensor(new Supla::Sensor::SensorBase("21330927", "iperl", "total_water_m3", keyIperl));
  len_without_crc = crcRemove(iperlarray, packetSize(iperlarray[0]));
  Serial.println("Iperl length:");
  Serial.println(len_without_crc);
  std::vector<unsigned char> frameIperl(iperlarray, iperlarray + len_without_crc);
  readValue = meterI->parse_frame(frameIperl);
  Serial.print(readValue);
  Serial.println("m3");

  Serial.println("3. Apator 16-2 short frame");
  auto meterA = new Supla::Sensor::WaterMeter(0);
  meterA->add_driver(new Apator162());
  // 3E44010614217103050787F47A86003085437BF01862DB9B8F85A4B456FB513D34A9747BAAA9AA99 EAF2D34CAAD1BB25FEFB1906E8CC2972508F811D619F1518A3C8E36F3F53236E8A00000000000000 00000000000000000000000000000000000000000000000000000000000000000000000000000000 00000000000000000000000000000000000000000000000000000000000000000000000000000000 0000000000000000000000000000000000000000000000000000000000000000
  uint8_t apatorminarray2[254] = {0x3e, 0x44, 0x01, 0x06, 0x14, 0x21, 0x71, 0x03, 0x05, 0x07, 0x87, 0xf4, 0x7a, 0x86, 0x00, 0x30, 0x85, 0x43, 0x7B, 0xf0, 0x18, 0x62, 0xdB, 0x9B, 0x8f, 0x85, 0xa4, 0xB4, 0x56, 0xfB, 0x51, 0x3d, 0x34, 0xa9, 0x74, 0x7B, 0xaa, 0xa9, 0xaa, 0x99, 0xea, 0xf2, 0xd3, 0x4c, 0xaa, 0xd1, 0xBB, 0x25, 0xfe, 0xfB, 0x19, 0x06, 0xe8, 0xcc, 0x29, 0x72, 0x50, 0x8f, 0x81, 0x1d, 0x61, 0x9f, 0x15, 0x18, 0xa3, 0xc8, 0xe3, 0x6f, 0x3f, 0x53, 0x23, 0x6e, 0x8a};

  meterA->add_sensor(new Supla::Sensor::SensorBase("03712114", "apator162", "total_water_m3", keyT));
  len_without_crc = crcRemove(apatorminarray2, packetSize(apatorminarray2[0]));
  Serial.println("Apator 16-2 2 length:");
  Serial.println(len_without_crc);
  std::vector<unsigned char> frameApatorShort(apatorminarray2, apatorminarray2 + len_without_crc);
  readValue = meterA->parse_frame(frameApatorShort);
  Serial.print(readValue);
  Serial.println("m3");

  Serial.println("4. Apator 16-2 long frame 2");
  auto meterA2 = new Supla::Sensor::WaterMeter(0);
  meterA2->add_driver(new Apator162());
  // 6E44010602934101050761ED7AB5006085926B8B7384B92BFDFA150C39FE800068055F46ECD610487583781090BB2648FD2B614212B0D3FAAB97B83401E4E996944BD651CCFB410D5833E3EC813EC7162D58A49953011E1B6EE916E2AB5255D68EBFDF31C11948FC9B5EB6E53D1505CF4EEE4F8E5FA92149CED8AE2511AEC0
  uint8_t apatorminarray3[254] = {0x6E,0x44,0x01,0x06,0x02,0x93,0x41,0x01,0x05,0x07,0x61,0xED,0x7A,0xB5,0x00,0x60,0x85,0x92,0x6B,0x8B,0x73,0x84,0xB9,0x2B,0xFD,0xFA,0x15,0x0C,0x39,0xFE,0x80,0x00,0x68,0x05,0x5F,0x46,0xEC,0xD6,0x10,0x48,0x75,0x83,0x78,0x10,0x90,0xBB,0x26,0x48,0xFD,0x2B,0x61,0x42,0x12,0xB0,0xD3,0xFA,0xAB,0x97,0xB8,0x34,0x01,0xE4,0xE9,0x96,0x94,0x4B,0xD6,0x51,0xCC,0xFB,0x41,0x0D,0x58,0x33,0xE3,0xEC,0x81,0x3E,0xC7,0x16,0x2D,0x58,0xA4,0x99,0x53,0x01,0x1E,0x1B,0x6E,0xE9,0x16,0xE2,0xAB,0x52,0x55,0xD6,0x8E,0xBF,0xDF,0x31,0xC1,0x19,0x48,0xFC,0x9B,0x5E,0xB6,0xE5,0x3D,0x15,0x05,0xCF,0x4E,0xEE,0x4F,0x8E,0x5F,0xA9,0x21,0x49,0xCE,0xD8,0xAE,0x25,0x11,0xAE,0xC0};

  meterA2->add_sensor(new Supla::Sensor::SensorBase("01419302", "apator162", "total_water_m3", keyT));
  len_without_crc = crcRemove(apatorminarray3, packetSize(apatorminarray3[0]));
  Serial.println("Apator 16-2 2 length:");
  Serial.println(len_without_crc);
  std::vector<unsigned char> frameApatorLong2(apatorminarray3, apatorminarray3 + len_without_crc);
  readValue = meterA2->parse_frame(frameApatorLong2);
  Serial.print(readValue);
  Serial.println("m3");
  Serial.println("---------------End of example sensors decoding------------------");

      // Replace the following GUID with value that you can retrieve from https://www.supla.org/arduino/get-guid
      char GUID[SUPLA_GUID_SIZE] = {0x01, 0x59, 0x3E, 0x4F, 0xB1, 0x7D, 0x33, 0x87, 0xA4, 0x22, 0xF5, 0xF3, 0x43, 0xC0, 0xDD, 0x9E};

  // Replace the following AUTHKEY with value that you can retrieve from: https://www.supla.org/arduino/get-authkey
  char AUTHKEY[SUPLA_AUTHKEY_SIZE] = {0x69, 0x06, 0x6D, 0x40, 0x48, 0x57, 0x5D, 0xFC, 0x79, 0xD7, 0x2C, 0xB2, 0x19, 0x75, 0x33, 0x28};
  // put your setup code here, to run once:

  SuplaDevice.begin(GUID,           // Global Unique Identifier
                    serverVariable, // SUPLA server address
                    emailVariable,  // Email address used to login to Supla Cloud
                    AUTHKEY);       // Authorization key
  std::vector<unsigned char> key{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  meter = new Supla::Sensor::WaterMeter(23, 19, 18, 5, 4, 2);
  meter->add_sensor(new Supla::Sensor::SensorInfo("87987102", "apator162", "total_water_m3", key));

  meter->add_driver(new Amiplus());
  meter->add_driver(new Apator08());
  meter->add_driver(new Apator162());
  meter->add_driver(new ApatorEITN());
  meter->add_driver(new Bmeters());
  meter->add_driver(new C5isf());
  meter->add_driver(new Compact5());
  meter->add_driver(new Dme07());
  meter->add_driver(new Elf());
  meter->add_driver(new Evo868());
  meter->add_driver(new FhkvdataIII());
  meter->add_driver(new Hydrocalm3());
  meter->add_driver(new Hydrus());
  meter->add_driver(new Iperl());
  meter->add_driver(new Itron());
  meter->add_driver(new Izar());
  meter->add_driver(new Mkradio3());
  meter->add_driver(new Mkradio4());
  meter->add_driver(new Qheat());
  meter->add_driver(new Qwater());
  meter->add_driver(new Sharky774());
  meter->add_driver(new TopasESKR());
  meter->add_driver(new Ultrimis());
  meter->add_driver(new Unismart());
  meter->add_driver(new Vario451());
}

void loop()
{
  SuplaDevice.iterate();
  meter->iterateAlways();
}
