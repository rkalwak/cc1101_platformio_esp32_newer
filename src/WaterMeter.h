#ifndef _WaterMeter_h
#define _WaterMeter_h
#include "Arduino.h"
#include "rf_mbus.hpp"
#include "Drivers/driver.h"
#include <supla/channel_element.h>
#include "wmbus_utils.hpp"
#include "utils.hpp"
#include <stdint.h>
#include <string>
#include <vector>
#include "sensor.h"
namespace Supla
{
	namespace Sensor
	{
		class WaterMeter : public ChannelElement
		{
		public:
			WaterMeter(uint8_t mosi = 23, uint8_t miso = 19, uint8_t clk = 18, uint8_t cs = 5, uint8_t gdo0 = 4, uint8_t gdo2 = 2);

			void iterateAlways();
			//void onInit();
			std::map<std::string, Driver *> drivers_{};
			std::map<std::string, SensorInfo *> sensors_{};
			void add_driver(Driver *driver);
			void add_sensor(SensorInfo *sensor);
			bool decrypt_telegram(std::vector<unsigned char> &telegram, std::vector<unsigned char> key);

		protected:
			uint64_t lastReadTime;


		private:
			float readValue = 0.0;
			int packetLength = 192;
			rf_mbus receiver;
		};
	};
};
#endif