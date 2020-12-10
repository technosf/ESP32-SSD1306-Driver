/*
 ESP32-SSD1306-Driver Library Display interface

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

#ifndef SSD1306_DISPLAY_H_
#define SSD1306_DISPLAY_H_

#include <string>
#include <stdint.h>

#include "SSD1306.h"

/**
 * @brief Display interface 
 * 
 * Abstract Display definition, specifies display related calls to be implemented
 */
class Display
{

public:
        /**
         * @brief   Return display width in pixels
         * 
         * @return  Panel width, or return 0 if failed (panel not initialized)
         */
        virtual uint8_t width() = 0;

        /**
         * @brief   Return display height in pixels
         * 
         * @return  Panel height, or return 0 if failed (panel not initialized)
         */
        virtual uint8_t height() = 0;

        /**
         * @brief   Clear display buffer (fill with black)
         * 
         * @param   limit Cleared area is limited to the last refreshed area
         * @return  Display& - Fluent
         */
        virtual Display &clear(bool limit = false) = 0;

        /**
         * @brief   Refresh display (send display buffer to the panel)
         * 
         * @param   force   The program automatically tracks "dirty" region to minimize refresh area. Set #force to true
         *                  ignores the dirty region and refresh the whole screen.
         * @return  Display& - Fluent
         */
        virtual Display &refresh(bool force = false) = 0;

        /**
         * @brief   Set normal or inverted display
         * 
         * @param   invert      Invert display true/false?
         * @return  Display& - Fluent
         */
        virtual Display &invert(bool invert) = 0;

        /**
         * @brief   Draw one pixel
         * 
         * @param   x       X coordinate
         * @param   y       Y coordinate
         * @param   color   Color of the pixel
         * @return  Display - Fluent
         */
        virtual Display &draw_pixel(uint8_t x, uint8_t y, color_t color) = 0;

        /**
         * @brief   Draw horizontal line
         * 
         * @param   x       X coordinate or starting (left) point
         * @param   y       Y coordinate or starting (left) point
         * @param   w       Line width
         * @param   color   Color of the line
         * @return  Display& - Fluent
         */
        virtual Display &draw_hline(uint8_t x, uint8_t y, uint8_t w, color_t color) = 0;

        /**
         * @brief   Draw vertical line
         * 
         * @param   x       X coordinate or starting (top) point
         * @param   y       Y coordinate or starting (top) point
         * @param   h       Line height
         * @param   color   Color of the line
         * @return  Display& - Fluent
         */
        virtual Display &draw_vline(uint8_t x, uint8_t y, uint8_t h, color_t color) = 0;

        /**
         * @brief   Draw an aliased line
         * 
         * @param   x       X coordinate or starting (top) point
         * @param   y       Y coordinate or starting (top) point
         * @param   xx      X coordinate or ending (bottom) point
         * @param   yy      Y coordinate or ending (bottom) point
         * @param   color   Color of the line
         * @return  Display& - Fluent
         */
        virtual Display &draw_line(uint8_t x, uint8_t y, uint8_t xx, uint8_t yy, color_t color) = 0;

        /**
         * @brief   Draw a rectangle
         * 
         * @param   x       X coordinate or starting (top left) point
         * @param   y       Y coordinate or starting (top left) point
         * @param   w       Rectangle width
         * @param   h       Rectangle height
         * @param   color   Color of the rectangle border
         * @return  Display& - Fluent
         */
        virtual Display &draw_rectangle(uint8_t x, uint8_t y, uint8_t w, uint8_t h, color_t color) = 0;

        /**
         * @brief   Draw a filled rectangle
         * 
         * @param   x       X coordinate or starting (top left) point
         * @param   y       Y coordinate or starting (top left) point
         * @param   w       Rectangle width
         * @param   h       Rectangle height
         * @param   color   Color of the rectangle
         * @return  Display& - Fluent
         */
        virtual Display &fill_rectangle(uint8_t x, uint8_t y, uint8_t w, uint8_t h, color_t color) = 0;

        /**
         * @brief   Draw a filled circle
         * 
         * @param   x      X coordinate or center
         * @param   y      Y coordinate or center
         * @param   r       Radius
         * @param   color   Color of the circle
         * @return  Display& - Fluent
         */
        virtual Display &draw_circle(uint8_t x, uint8_t y, uint8_t r, color_t color) = 0;

        /**
         * @brief   Draw a filled circle
         * 
         * @param   x      X coordinate or center
         * @param   y      Y coordinate or center
         * @param   r       Radius
         * @param   color   Color of the circle
         * @return  Display& - Fluent
         */
        virtual Display &fill_circle(uint8_t x, uint8_t y, uint8_t r, color_t color) = 0;

        /**
         * @brief   Draw one character using currently selected font
         * 
         * @param   x           X position of character (top-left corner)
         * @param   y           Y position of character (top-left corner)
         * @param   c           The character to draw
         * @param   foreground  Character color
         * @param   background  Background color
         * @param   outwidth  Width of the character
         * @return  Display& - Fluent
         */
        virtual Display &draw_char(uint8_t x, uint8_t y, unsigned char c, color_t foreground, color_t background,
                                   uint8_t *outwidth = nullptr) = 0;

        /**
         * @brief   Draw string using currently selected font
         * 
         * @param   x           X position of string (top-left corner)
         * @param   y           Y position of string (top-left corner)
         * @param   str         The string to draw
         * @param   foreground  Character color
         * @param   background  Background color
         * @param   outwidth  Width of the string (out-of-display pixels also included)
         * @return  Display& - Fluent
         */
        virtual Display &draw_string(uint8_t x, uint8_t y, std::string str, color_t foreground, color_t background,
                                     uint8_t *outwidth = nullptr) = 0;

        /**
         * @brief   Measure width of string with current selected font
         * 
         * @param   str String to measure
         * @return  Width of the string
         */
        virtual uint8_t measure_string(std::string str) = 0;

        /**
         * @brief   Get the height of current selected font
         * 
         * @return  Height of the font (in pixels) or 0 if none font selected
         */
        virtual uint8_t font_height() = 0;

        /**
         * @brief   Get the "C" value (space between adjacent characters)
         * 
         * @return  "C" value
         */
        virtual uint8_t font_c() = 0;

        /**
         * @brief   Select font for drawing
         * 
         * @param  idx     Font index, see fonts.c
         * @return Display&  - Fluent
         */
        virtual Display &select_font(uint8_t idx) = 0;
};

#endif /* SSD1306_DISPLAY_H_ */
