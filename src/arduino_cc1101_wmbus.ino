#include "Arduino.h"
#include "crc.hpp"
#include "mbus_packet.hpp"
#include "aes.hpp"
#include "3outof6.hpp"
#include "mbus_defs.hpp"
#include "tmode_rf_settings.hpp"
#include "rf_mbus.hpp"
#include "WaterMeter.h"
#include <SuplaDevice.h>
#include <Drivers/drivers.h>
#include <supla/network/esp_wifi.h>
Supla::ESPWifi wifi(wifiSSIDString, wifiPasswordString);

void setup()
{
  Serial.begin(115200);
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
  auto meter = new Supla::Sensor::WaterMeter(23, 19, 18, 5, 4, 2);
  meter->add_sensor(new SensorInfo("87987102", "apator162", "total_water_m3", key));

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
}
