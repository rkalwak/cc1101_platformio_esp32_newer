#include <stdint.h>
#include <string>
#include <vector>
#include <aes.hpp>
struct decoded_data
{
    std::string meterId = "unknown";
    std::string rawData;
    std::string usage;
    std::string manufacturer;
    float usageNumber = 0.000001;
};
class apator162DecoderMine
{
    uint8_t keyArray[32] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    struct AES_ctx ctx;
    uint8_t in[96];

public:
    float get_value(uint8_t *telegram);
    void generate_iv(const uint8_t *packet, uint8_t *iv);
    void extract_payload(const uint8_t *packet, uint8_t *extractedPayload, uint8_t payloadIndex = 17);
    decoded_data decrypt(uint8_t *packet, uint8_t length);
};