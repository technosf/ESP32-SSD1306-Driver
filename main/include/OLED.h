/*
 ESP32-SSD1306-Driver Library OLED Display

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

#ifndef SSD1306_OLED_H_
#define SSD1306_OLED_H_

#include <string>
#include <stdint.h>
#include <sdkconfig.h>
#include <esp_log.h>

#include <algorithm>
#include <vector>

#include <Font_Manager.h>
#include "Display.h"
#include "SSD1306.h"

/**
 * @brief 
 * 
 */
class OLED : public Display
{
        static const constexpr char *TAG = "OLED";

private:
        Font_Manager *m_font_manager{nullptr}; ///< The current font
        SSD1306 &m_ssd1306;                    ///< SSD1306 driving this Display

public:
        /**
         * @brief OLED Display driven by a SSD1306 chip
         * @param ssd1306
         */
        OLED(SSD1306 &ssd1306);

        virtual ~OLED()
        {
        }

        virtual uint8_t width();
        virtual uint8_t height();
        virtual Display &clear(bool limit = false);
        virtual Display &refresh(bool force = false);
        virtual Display &invert(bool invert);
        virtual Display &draw_pixel(uint8_t x, uint8_t y, color_t color);
        virtual Display &draw_hline(uint8_t x, uint8_t y, uint8_t w, color_t color);
        virtual Display &draw_vline(uint8_t x, uint8_t y, uint8_t h, color_t color);
        virtual Display &draw_line(uint8_t x, uint8_t y, uint8_t xx, uint8_t yy, color_t color);
        virtual Display &draw_rectangle(uint8_t x, uint8_t y, uint8_t w, uint8_t h, color_t color);
        virtual Display &fill_rectangle(uint8_t x, uint8_t y, uint8_t w, uint8_t h, color_t color);
        virtual Display &draw_circle(uint8_t x0, uint8_t y0, uint8_t r, color_t color);
        virtual Display &fill_circle(uint8_t x0, uint8_t y0, uint8_t r, color_t color);
        virtual Display &draw_char(uint8_t x, uint8_t y, unsigned char c, color_t foreground, color_t background,
                                   uint8_t *outwidth = nullptr);
        virtual Display &draw_string(uint8_t x, uint8_t y, std::string str, color_t foreground, color_t background,
                                     uint8_t *outwidth = nullptr);
        virtual uint8_t measure_string(std::string str);
        virtual uint8_t font_height();
        virtual uint8_t font_c();
        const virtual char *font_name();
        virtual Display &select_font(uint8_t idx);
};

#endif /* SSD1306_OLED_H_ */
