#pragma once

#include <vector>
#include <string>
#include <stdint.h>
struct SensorInfo
{
    virtual ~SensorInfo() = default;

public:
    SensorInfo(std::string meter_id, std::string type, std::string property_to_send, std::vector<unsigned char> key)
        : _meter_id(meter_id), _type(type), _property_to_send(property_to_send), _key(key){};
    std::string get_type() { return this->_type; };
    std::string get_meter_id() { return this->_meter_id; };
    std::string get_property_to_send() { return this->_property_to_send; };
    std::vector<unsigned char> get_key() { return this->_key; };

private:
    SensorInfo();
    std::vector<unsigned char> _key;
    std::string _type;
    std::string _meter_id;
    std::string _property_to_send;
};