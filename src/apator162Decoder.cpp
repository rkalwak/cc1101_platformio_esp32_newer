#include "apator162Decoder.h"
#include "Arduino.h"
#include "utils.hpp"

float apator162Decoder::get_total_water_m3(std::vector<unsigned char> &telegram)
{
  float ret_val{};
  uint32_t usage = 0;
  size_t i = 25;
  if (telegram[10] == 0xb6)
  {
    // Oups really old style telegram that we cannot decode.
  }
  else
  {
    while (i < telegram.size())
    {
      int c = telegram[i];
      int size = this->registerSize(c);
      if (c == 0xff)
        break; // An FF signals end of telegram padded to encryption boundary,
      // FFFFFFF623A where 4 last are perhaps crc or counter?
      i++;
      if (size == -1 || i + size >= telegram.size())
      {
        break;
      }
      if (c == 0x10 && size == 4 && i + size < telegram.size())
      {
        // We found the register representing the total
        usage = ((uint32_t)telegram[i + 3] << 24) | ((uint32_t)telegram[i + 2] << 16) |
                ((uint32_t)telegram[i + 1] << 8) | ((uint32_t)telegram[i + 0]);
        ret_val = usage / 1000.0;
        break;
      }
      i += size;
    }
  }
  return ret_val;
};

int apator162Decoder::registerSize(int c)
{
  switch (c)
  {
    // case 0x0f: return 3; // Payload often starts with 0x0f,
    // which  also means dif = manufacturer data follows.
    // After 0x0F there is always:
    // next 4B : Date - In default frame
    // next 3B : Faults - In default frame

  case 0x00:
    return 4; // Date
  case 0x01:
    return 3; // Faults - In default frame f.ex. 0F 09 4D A1 97 18 02 00 -> 18 02 00 -> 00 02 18 -> 0x0218

  case 0xA1:
  case 0x10:
    return 4; // Total volume - In default frame

  case 0x11:
    return 2; // Flow

  case 0x40:
    return 6; // Detectors
  case 0x41:
    return 2; // Voltage
  case 0x42:
    return 4; // Energy
  case 0x43:
    return 2; // Life days - In default frame f.ex. 43 6E 0A -> 2670 days from first run

  case 0x44:
    return 3;

  case 0x71:
    return 1 + 2 * 4; // ?
  case 0x72:
    return 1 + 3 * 4; // ?
  case 0x73:
    return 1 + 4 * 4; // Historical data
  case 0x75:
    return 1 + 6 * 4; // Historical data
  case 0x7B:
    return 1 + 12 * 4; // Historical data

  case 0x80:
  case 0x81:
  case 0x82:
  case 0x83:
  case 0x84:
  case 0x86:
  case 0x87:
    return 10; // Events

  case 0x85:
  case 0x88:
  case 0x8F:
    return 11; // Events

  case 0x8A:
    return 9; // Events

  case 0x8B:
  case 0x8C:
    return 6; // Events

  case 0x8E:
    return 7; // Events

  case 0xA0:
    return 4;

  case 0xA2:
    return 1;

  case 0xA3:
    return 7;

  case 0xA4:
    return 4;

  case 0xA5:
  case 0xA9:
  case 0xAF:
    return 1;

  case 0xA6:
    return 3;

  case 0xA7:
  case 0xA8:
  case 0xAA:
  case 0xAB:
  case 0xAC:
  case 0xAD:
    return 2;

  case 0xB0:
    return 5;
  case 0xB1:
    return 8;
  case 0xB2:
    return 16;
  case 0xB3:
    return 8;
  case 0xB4:
    return 2;
  case 0xB5:
    return 16;

  // Unknown
  case 0xB6:
    return 3;
  case 0xB7:
    return 3;
  case 0xB8:
    return 3;
  case 0xB9:
    return 3;
  case 0xBA:
    return 3;
  case 0xBB:
    return 3;
  case 0xBC:
    return 3;
  case 0xBD:
    return 3;
  case 0xBE:
    return 3;
  case 0xBF:
    return 3;

  case 0xC0:
    return 3;
  case 0xC1:
    return 3;
  case 0xC2:
    return 3;
  case 0xC3:
    return 3;
  case 0xC4:
    return 3;
  case 0xC5:
    return 3;
  case 0xC6:
    return 3;
  case 0xC7:
    return 3;

  case 0xD0:
    return 3;
  case 0xD3:
    return 3;

  case 0xF0:
    return 4;
  }
  return -1;
}

bool apator162Decoder::decrypt_telegram(std::vector<unsigned char> &telegram)
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
  std::vector<unsigned char> key{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
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

float apator162Decoder::extractValue(std::vector<unsigned char> frame)
{
  std::string decrypted_telegram = format_hex_pretty(frame);
  decrypted_telegram.erase(std::remove(decrypted_telegram.begin(), decrypted_telegram.end(), '.'),
                           decrypted_telegram.end());
  Serial.printf("Decrypted T : %s\n", decrypted_telegram.c_str());
  std::string val = decrypted_telegram.substr(80, 8);
  Serial.printf("Raw value: %s\n", val.c_str());
  Serial.println(".");

  return get_total_water_m3(frame);
}