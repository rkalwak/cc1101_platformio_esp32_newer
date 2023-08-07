#include "Arduino.h"
#include "crc.hpp"
#include "mbus_packet.hpp"
#include "wmbus_utils.hpp"
#include "utils.hpp"
#include "aes.hpp"
#include "3outof6.hpp"
#include "mbus_defs.hpp"
#include "tmode_rf_settings.hpp"
#include "rf_mbus.hpp"
#include "apator162decoder.h"
#include <SuplaDevice.h>
#include <supla/channel_element.h>

#include <supla/network/esp_wifi.h>
Supla::ESPWifi wifi("Dom44A", "najciemniejpodlatarnia1234");
namespace Supla
{
  namespace Sensor
  { 
    class WaterMeter : public ChannelElement
    {
    private:
      float readValue = 0.0;
      int packetLength = 192;
      rf_mbus receiver;
      apator162Decoder myDecoder;

    public:
      WaterMeter(uint8_t mosi = 23, uint8_t miso = 19, uint8_t clk = 18, uint8_t cs = 5, uint8_t gdo0 = 4, uint8_t gdo2 = 2) : lastReadTime(0)
      {
        bool isInitialized = receiver.init(mosi, miso, clk, cs, gdo0, gdo2);
        if (isInitialized)
        {
          Serial.println("Receiver started.");
        }
        channel.setType(SUPLA_CHANNELTYPE_IMPULSE_COUNTER);
        channel.setDefault(SUPLA_CHANNELFNC_IC_WATER_METER);
      };

      void iterateAlways()
      {
        if (receiver.task())
        {
          Serial.println("Found.");
          dumpHex(receiver.MBpacket, packetLength);
          /*
          Serial.println("Decrypting.");
          decoded_data data = decrypt(receiver.MBpacket, packetLength);
          Serial.println("Decrypted. Rawdata:");
          Serial.println(data.rawData);
          Serial.print(data.usageNumber);
          Serial.println("m3");
          */
          Serial.println("........................................");
          WMbusFrame mbus_data = receiver.get_frame();
          std::vector<unsigned char> frame = mbus_data.frame;
          std::string telegram = format_hex_pretty(frame);
          telegram.erase(std::remove(telegram.begin(), telegram.end(), '.'), telegram.end());

          if (myDecoder.decrypt_telegram(frame))
          {
            readValue = myDecoder.extractValue(frame);
            Serial.print(readValue);
            Serial.println("m3");
            channel.setNewValue((int)(readValue*1000000));
          }
        }

        /*
        if (((lastReadTime + 10000 )< millis()))
        {
          lastReadTime = millis();
          //Serial.println("Sending value.");
          //Serial.printf("Sending value: %s m3.\n", readValue);
          counter+=10;
          channel.setNewValue(readValue);
        }
        */
      }

    protected:
      uint64_t lastReadTime;
    };

  };
};

void setup()
{
  Serial.begin(115200);
  // Replace the following GUID with value that you can retrieve from https://www.supla.org/arduino/get-guid
  char GUID[SUPLA_GUID_SIZE] = {0x01, 0x59, 0x3E, 0x4F, 0xB1, 0x7D, 0x33, 0x87, 0xA4, 0x22, 0xF5, 0xF3, 0x43, 0xC0, 0xDD, 0x9E};

  // Replace the following AUTHKEY with value that you can retrieve from: https://www.supla.org/arduino/get-authkey
  char AUTHKEY[SUPLA_AUTHKEY_SIZE] = {0x69, 0x06, 0x6D, 0x40, 0x48, 0x57, 0x5D, 0xFC, 0x79, 0xD7, 0x2C, 0xB2, 0x19, 0x75, 0x33, 0x28};
  // put your setup code here, to run once:

  SuplaDevice.begin(GUID,                  // Global Unique Identifier
                    serverVariable,     // SUPLA server address
                    emailVariable, // Email address used to login to Supla Cloud
                    AUTHKEY);              // Authorization key
  new Supla::Sensor::WaterMeter();
}

void loop()
{
  SuplaDevice.iterate();
}
