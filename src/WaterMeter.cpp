#include "WaterMeter.h"
namespace Supla
{
  namespace Sensor
  {
    WaterMeter::WaterMeter(std::vector<unsigned char> key, std::string type, uint8_t mosi, uint8_t miso, uint8_t clk, uint8_t cs, uint8_t gdo0, uint8_t gdo2) : lastReadTime(0)
    {
      bool isInitialized = receiver.init(mosi, miso, clk, cs, gdo0, gdo2);
      if (isInitialized)
      {
        Serial.println("Receiver started.");
      }
      channel.setType(SUPLA_CHANNELTYPE_IMPULSE_COUNTER);
      channel.setDefault(SUPLA_CHANNELFNC_IC_WATER_METER);
      _type = type;
      _key = key;
    };

    void WaterMeter::add_driver(Driver *driver)
    {
      this->drivers_[driver->get_name()] = driver;
    }

    bool WaterMeter::decrypt_telegram(std::vector<unsigned char> &telegram, std::vector<unsigned char> key)
    {
      bool ret_val = false;
      std::vector<unsigned char>::iterator pos;
      // CI
      pos = telegram.begin() + 10;
      // data offset
      int offset{0};

      unsigned char iv[16];
      int i = 0;

      if ((*pos == 0x67) || (*pos == 0x6E) || (*pos == 0x74) ||
          (*pos == 0x7A) || (*pos == 0x7D) || (*pos == 0x7F) || (*pos == 0x9E))
      {
        offset = 15;

        // dll-mfct + dll-id + dll-version + dll-type
        for (int j = 0; j < 8; ++j)
        {
          iv[i++] = telegram[2 + j];
        }
        // tpl-acc
        for (int j = 0; j < 8; ++j)
        {
          iv[i++] = telegram[11];
        }
      }
      else if ((*pos == 0x68) || (*pos == 0x6F) || (*pos == 0x72) ||
               (*pos == 0x75) || (*pos == 0x7C) || (*pos == 0x7E) || (*pos == 0x9F))
      {
        offset = 23;

        // tpl-mfct
        for (int j = 0; j < 2; ++j)
        {
          iv[i++] = telegram[15 + j];
        }
        // tpl-id
        for (int j = 0; j < 4; ++j)
        {
          iv[i++] = telegram[11 + j];
        }
        // tpl-version + tpl-type
        for (int j = 0; j < 2; ++j)
        {
          iv[i++] = telegram[17 + j];
        }
        // tpl-acc
        for (int j = 0; j < 8; ++j)
        {
          iv[i++] = telegram[19];
        }
      }
      else
      {
        Serial.println("CI unknown");
      }

      pos = telegram.begin() + offset;
      int num_encrypted_bytes = 0;
      int num_not_encrypted_at_end = 0;

      if (decrypt_TPL_AES_CBC_IV(telegram, pos, key, iv,
                                 &num_encrypted_bytes, &num_not_encrypted_at_end))
      {
        uint32_t decrypt_check = 0x2F2F;
        uint32_t dc = (((uint32_t)telegram[offset] << 8) | ((uint32_t)telegram[offset + 1]));
        if (dc == decrypt_check)
        {
          ret_val = true;
        }
      }
      return ret_val;
    }


    void WaterMeter::iterateAlways()
    {
      if (receiver.task())
      {
        Serial.println("Found.");
        dumpHex(receiver.MBpacket, packetLength);

        Serial.println("........................................");
        WMbusFrame mbus_data = receiver.get_frame();
        std::vector<unsigned char> frame = mbus_data.frame;
        std::string telegram = format_hex_pretty(frame);
        telegram.erase(std::remove(telegram.begin(), telegram.end(), '.'), telegram.end());
        uint32_t meter_id = ((uint32_t)frame[7] << 24) | ((uint32_t)frame[6] << 16) |
                            ((uint32_t)frame[5] << 8) | ((uint32_t)frame[4]);
        std::string meterIdString = telegram.substr(8, 8);
        // need to decrypt first
        bool isOk = true;
        if (_key.size())
        {
          if (!decrypt_telegram(frame, _key))
          {
            isOk = false;
          }
        }
        if (isOk)
        {
          auto driver = this->drivers_[_type];
          auto mapValues = driver->get_values(frame);
          auto readValue = mapValues["total_water_m3"];
          Serial.print("Meter id as number: ");
          Serial.println(meter_id);
          Serial.print("Meter id as string: ");
          Serial.println(meterIdString.c_str());
          Serial.print(readValue);
          Serial.println("m3");
          channel.setNewValue((int)(readValue * 1000000));
        }
      }
    }


  };
};