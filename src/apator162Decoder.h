#include <stdint.h>
#include <string>
#include <vector>

class apator162Decoder
{

public:
    bool decrypt_telegram(std::vector<unsigned char> &telegram);
    float extractValue(std::vector<unsigned char> frame);

private:
    float get_total_water_m3(std::vector<unsigned char> &telegram);
    int registerSize(int c);
};