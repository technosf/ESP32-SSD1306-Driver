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

#include <oled_fonts.h>

#include "Display.h"
#include "SSD1306.h"

/**
 * \class OLED
 *
 */
class OLED : public Display
{
    private:
        SSD1306& m_ssd1306;    /// < SSD1306 driving this Display

    public:

        /**
         * @brief OLED Display driven by a SSD1306 chip
         * @param ssd1306
         */
        OLED( SSD1306& ssd1306 );

        virtual ~OLED()
        {
        }

        /**
         * @brief   Return OLED panel height
         * @return  Panel height, or return 0 if failed (panel not initialized)
         */
        virtual uint8_t width();

        /**
         * @brief   Return OLED panel height
         * @return  Panel height, or return 0 if failed (panel not initialized)
         */
        virtual uint8_t height();

        /**
         * @brief   Clear display buffer (fill with black)
         */
        virtual Display &clear();

        /**
         * @brief   Refresh display (send display buffer to the panel)
         * @param   force   The program automatically tracks "dirty" region to minimize refresh area. Set #force to true
         *                  ignores the dirty region and refresh the whole screen.
         */
        virtual Display &refresh( bool force = false );

        /**
         * @brief   Set normal or inverted display
         * @param   invert      Invert display?
         */
        virtual Display &invert( bool invert );

        /**
         * @brief   Draw one pixel
         * @param   x       X coordinate
         * @param   y       Y coordinate
         * @param   color   Color of the pixel
         */
        virtual Display &draw_pixel( uint8_t x, uint8_t y, color_t color );

        /**
         * @brief   Draw horizontal line
         * @param   x       X coordinate or starting (left) point
         * @param   y       Y coordinate or starting (left) point
         * @param   w       Line width
         * @param   color   Color of the line
         */
        virtual Display &draw_hline( uint8_t x, uint8_t y, uint8_t w, color_t color );

        /**
         * @brief   Draw vertical line
         * @param   x       X coordinate or starting (top) point
         * @param   y       Y coordinate or starting (top) point
         * @param   h       Line height
         * @param   color   Color of the line
         */
        virtual Display &draw_vline( uint8_t x, uint8_t y, uint8_t h, color_t color );

        /**
         * @brief   Draw a rectangle
         * @param   x       X coordinate or starting (top left) point
         * @param   y       Y coordinate or starting (top left) point
         * @param   w       Rectangle width
         * @param   h       Rectangle height
         * @param   color   Color of the rectangle border
         */
        virtual Display &draw_rectangle( uint8_t x, uint8_t y, uint8_t w, uint8_t h, color_t color );

        /**
         * @brief   Draw a filled rectangle
         * @param   x       X coordinate or starting (top left) point
         * @param   y       Y coordinate or starting (top left) point
         * @param   w       Rectangle width
         * @param   h       Rectangle height
         * @param   color   Color of the rectangle
         */
        virtual Display &fill_rectangle( uint8_t x, uint8_t y, uint8_t w, uint8_t h, color_t color );

        /**
         * @brief   Draw a filled circle
         * @param   x0      X coordinate or center
         * @param   y0      Y coordinate or center
         * @param   r       Radius
         * @param   color   Color of the circle
         */
        virtual Display &draw_circle( uint8_t x0, uint8_t y0, uint8_t r, color_t color );

        /**
         * @brief   Draw a filled circle
         * @param   x0      X coordinate or center
         * @param   y0      Y coordinate or center
         * @param   r       Radius
         * @param   color   Color of the circle
         */
        virtual Display &fill_circle( uint8_t x0, uint8_t y0, uint8_t r, color_t color );

        /**
         * @brief   Draw one character using currently selected font
         * @param   x           X position of character (top-left corner)
         * @param   y           Y position of character (top-left corner)
         * @param   c           The character to draw
         * @param   foreground  Character color
         * @param   background  Background color
         * @return  Width of the character
         */
        virtual Display &draw_char( uint8_t x, uint8_t y, unsigned char c, color_t foreground, color_t background,
                uint8_t* outwidth = nullptr );

};

#endif  /* SSD1306_OLED_H_ */

