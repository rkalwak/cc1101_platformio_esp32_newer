#include "apator162mine.h"
#include "Arduino.h"
#include "utils.hpp"

float apator162DecoderMine::get_value(uint8_t *telegram)
{
  uint32_t usage = 0;
  size_t i = 0;
  float water_usage = 0;
  usage = ((uint32_t)telegram[i + 3] << 24) | ((uint32_t)telegram[i + 2] << 16) |
          ((uint32_t)telegram[i + 1] << 8) | ((uint32_t)telegram[i + 0]);
  water_usage = (usage) / 1000.0;
  return water_usage;
}

void apator162DecoderMine::generate_iv(const uint8_t *packet, uint8_t *iv)
{
  int i = 0;
  uint8_t tpl_acc = packet[13];
  uint8_t dll_mfct_b[2] = {0x01, 0x06};
  // id + version + type
  uint8_t dll_a[6] = {0x87, 0x98, 0x71, 0x02, 0x05, 0x07};
  // mine is here
  //  M-field
  iv[i++] = dll_mfct_b[0];
  iv[i++] = dll_mfct_b[1];

  // A-field
  for (int j = 0; j < 6; ++j)
  {
    iv[i++] = dll_a[j];
  }
  for (int j = 0; j < 8; ++j)
  {
    iv[i++] = tpl_acc;
  }
}

void apator162DecoderMine::extract_payload(const uint8_t *packet, uint8_t *extractedPayload, uint8_t payloadIndex)
{
  int j = 0;
  for (int i = 0; i < 96; i++)
  {
    extractedPayload[j] = packet[i + payloadIndex];
    j++;
  }
}

decoded_data apator162DecoderMine::decrypt(uint8_t *packet, uint8_t length)
{

  Serial.println("Payload:");
  extract_payload(packet, in);
  printHexString(in, 96);
  Serial.println(".");
  uint8_t iv[16];
  generate_iv(packet, iv);
  Serial.println("IV:");
  printHexString(iv);
  AES_init_ctx_iv(&ctx, keyArray, iv);
  AES_CBC_decrypt_buffer(&ctx, in, 96);

  Serial.println("Decoded payload:");
  printHexString(iv, 96);
  Serial.println(".");

  Serial.println("Decoded value:");
  uint8_t valueOnly[] = {0, 0, 0, 0, 0, 0, 0, 0};
  for (int i = 46; i < 54; i++)
  {
    valueOnly[i - 46] = in[i];
  }
  printHexString(in, 54, 46);
  Serial.println(".");
  decoded_data data;
  data.usageNumber = get_value(valueOnly);
  return data;
}
