/*
 ESP32-SSD1306-Driver Library Protocol Interface

 v0.1.0

 Copyright 2019 technosf [https://github.com/technosf]

 Licensed under the GNU LESSER GENERAL PUBLIC LICENSE, Version 3.0 or greater (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 https://www.gnu.org/licenses/lgpl-3.0.en.html
 Unless required by applicable law or agreed to in writing,
 software distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and limitations under the License.
 */

#ifndef SSD1306_PIF_H_
#define SSD1306_PIF_H_

#include <stdint.h>

/**
 * @brief Protocol Interface
 *
 * Abstract SSD1306-to-wire-protocol interface specification to be implemented by specific protocol drivers
 *
 */
class PIF
{
    public:

        /*
         *
         */
        virtual ~PIF()
        {
        }

        /**
         * @brief Prints out information on the particular connected protocol bus
         */
        virtual void info() = 0;

        /**
         * @brief Sends a SSD1306 command over the wire protocol to the SSD1306 from the ESP32
         * 
         * @param cmd the command
         */
        virtual void command(const uint8_t cmd ) = 0;

        /**
         * @brief Sends SSD1306 commands over the wire protocol to the SSD1306 from the ESP32
         * 
         * @param cmd the command
         * @param size size of command in bytes
         */
        virtual void command(const uint8_t* cmd, uint8_t size ) = 0;

        /**
         * @brief Sends SSD1306 data over the wire protocol to the SSD1306 from the ESP32
         * 
         * @param data the data
         * @param size size of data in bytes
         */
        virtual void data( uint8_t* data, uint8_t size ) = 0;
};

#endif // SSD1306_PIF_H_
