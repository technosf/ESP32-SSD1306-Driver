/*
 ESP32-SSD1306-Driver Library SSD1306 Driver

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

#ifndef SSD1306_H_
#define SSD1306_H_

#define SSD1306_CHARGEPUMP				0x8d
#define SSD1306_COLUMNADDR 				0x21
#define SSD1306_COMSCANDEC				0xc8
#define SSD1306_DEACTIVATE_SCROLL		0x2e
#define SSD1306_DISPLAYALLON_RESUME		0xa4
#define SSD1306_DISPLAYOFF				0xae
#define SSD1306_DISPLAYON 				0xaf
#define SSD1306_INVERTDISPLAY 			0xa7
#define SSD1306_MEMORYMODE				0x20
#define SSD1306_NORMALDISPLAY 			0xa6
#define SSD1306_PAGEADDR 				0x22
#define SSD1306_SETCOMPINS				0xda
#define SSD1306_SETCONTRAST				0x81
#define SSD1306_SETDISPLAYCLOCKDIV		0xd5
#define SSD1306_SETDISPLAYOFFSET		0xd3
#define SSD1306_SETDISPLAYSTARTLINE		0x40
#define SSD1306_SETMULTIPLEX			0xa8
#define SSD1306_SETPRECHARGE			0xd9
#define SSD1306_SETSEGREMAP_0			0xa0
#define SSD1306_SETSEGREMAP_127			0xa1
#define SSD1306_SETVCOMDETECT			0xdb

#include <string.h>
#include <stdint.h>
#include <algorithm>

#include "PIF.h"

enum color_t
{
    TRANSPARENT = -1,    //!< Transparent (not drawing)
    BLACK = 0,    //!< Black (pixel off)
    WHITE = 1,    //!< White (or blue, yellow, pixel on)
    INVERT = 2,    //!< Invert pixel (XOR)
};

/**
 * @brief Panel type
 * Panel type and dimensions, with the enumerator indicating the number of memory pages
 */
enum panel_type_t
{
    SSD1306_128x64 = 8,    //!< 128x32 panel, 8 pages of memory
    SSD1306_128x32 = 4    //!< 128x64 panel, 4 pages of memory
};

/**
 * \class SSD1306
 *
 */
class SSD1306
{
        static const uint8_t COLUMNS = 128;    /// < SSD1306 is a 128 column driver chip

    public:

        /**
         * @brief   Constructor
         * @param   pif     Wire protocol interface adaptor
         * @param   scl_pin  SCL Pin
         * @param   sda_pin  SDA Pin
         */
        SSD1306( PIF* pif, panel_type_t type );

        virtual ~SSD1306()
        {
        }

        /**
         * @brief   Initialize OLED panel
         * @return  true if successful
         * @remark  Possible reasons for failure include non-configured panel type, out of memory or I2C not responding
         */
        bool init();

        /**
         * @brief   De-initialize OLED panel, turn off power and free memory
         * @return  true if successful
         * @remark  Possible reasons for failure include non-configured panel type, out of memory or I2C not responding
         */
        void term();

        /**
         * @brief   Return OLED panel height
         * @return  Panel height, or return 0 if failed (panel not initialized)
         */
        uint8_t width();

        /**
         * @brief   Return OLED panel height
         * @return  Panel height, or return 0 if failed (panel not initialized)
         */
        uint8_t height();

        /**
         * @brief   Clear display buffer (fill with black)
         */
        void clear();

        /**
         * @brief   Refresh display (send display buffer to the panel)
         */
        void refresh( bool force );

        /**
         * @brief	Set the color of pixels in a segment
         * @param   page   the page coord of the column
         * @param   column   the column # in the page
         * @param   bits   the segment bits to set
         * @param   color   the color to set the segment bits
         * @param   count   the number of consecutive segments in the page to draw
         * @return  True if segment was drawn
         */
        bool segment( uint8_t page, uint8_t column, uint8_t bits, color_t color, uint8_t count = 1 );

        /**
         * @brief	Set the color of a single pixel
         * @param   x   the x coord of the pixel
         * @param   y   the y coord of the pixel
         * @param   color   the desired color of the pixel
         * @param	count	number of times to draw the bits
         * @return  True if pixel was drawn
         */
        bool pixel( uint8_t x, uint8_t y, color_t color );

        /**
         * @brief	Draw a filled box of the given color
         * @param   x   the starting x coord of the box
         * @param   y   the starting y coord of the box
         * @param   color   the color to set the box
         * @param   w   the width of the box in pixels to draw
         * @param   h   the height of the box in pixels
         * @return  True if line was drawn
         */
        bool box( uint8_t x, uint8_t y, color_t color, uint8_t w, uint8_t h );

        /**
         * @brief	Draw a horizontal line of the given color
         * @param   x   the starting x coord of the line
         * @param   y   the starting y coord of the line
         * @param   color   the color to set the segment bits
         * @param   w   the width of the line in pixels to draw
         * @param   h   the height of the line in pixels, defaults to 1
         * @return  True if line was drawn
         */
        bool horizontal( uint8_t x, uint8_t y, color_t color, uint8_t w, uint8_t h = 1 );

        /**
         * @brief	Draw a vertical line of the given color
         * @param   x   the starting x coord of the line
         * @param   y   the starting y coord of the line
         * @param   color   the color to set the segment bits
         * @param   h   the height of the line in pixels to draw
         * @param   w   the width of the line in pixels, defaults to 1
         * @return  True if line was drawn
         */
        bool vertical( uint8_t x, uint8_t y, color_t color, uint8_t h, uint8_t w = 1 );

        /**
         * @brief   Set normal or inverted display
         * @param   invert      Invert display?
         */
        void invert_display( bool invert );

        /**
         * @brief   Direct update display buffer
         * @param   data        Data to fill display buffer, no length check is performed!
         * @param   length      Length of data
         */
        void update_buffer( uint8_t* data, uint16_t length );

    private:
        const uint8_t BITS [ 8 ] = { 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF };    /// < Segment bit mask

        bool m_init { false };
        PIF * m_pif;	///< Wire protocol adapter
        panel_type_t m_type;
        uint8_t (*m_buffer) [ COLUMNS ];    /// Display buffer - Page by Column
        uint16_t m_buffer_size;
        uint8_t m_width { COLUMNS };          			/// panel width (128)
        uint8_t m_height;          			/// panel height (32 or 64)
        uint16_t m_pixels;          			/// panel pixel count
        uint8_t m_refresh_top { 0 };    /// "Dirty" window
        uint8_t m_refresh_left { 0 };
        uint8_t m_refresh_right { 0 };
        uint8_t m_refresh_bottom { 0 };

        uint8_t initcmds32 [ 25 ] =    /// < initiate 32 line display
                { SSD1306_DISPLAYOFF,
                SSD1306_SETDISPLAYCLOCKDIV, 0x80,    // Suggested value 0x80
                        SSD1306_SETMULTIPLEX, 0x1f,			// 1/32
                        SSD1306_SETDISPLAYOFFSET, 0x00,			// 0 no offset
                        SSD1306_SETDISPLAYSTARTLINE + 0,    // line #0
                        SSD1306_MEMORYMODE, 0x00,    // 0x0 act like ks0108
                        SSD1306_SETSEGREMAP_127,
                        SSD1306_COMSCANDEC,
                        SSD1306_SETCOMPINS, 0x02,    //
                        SSD1306_SETCONTRAST, 0x2f,    //
                        SSD1306_SETPRECHARGE, 0xf1,    //
                        SSD1306_SETVCOMDETECT, 0x40,    //
                        SSD1306_DEACTIVATE_SCROLL,
                        SSD1306_CHARGEPUMP, 0x14,    // Charge pump on
                        SSD1306_DISPLAYALLON_RESUME,
                        SSD1306_NORMALDISPLAY };

        uint8_t initcmds64 [ 25 ] =    /// < Initiate 64 line display
                { SSD1306_DISPLAYOFF,
                SSD1306_SETDISPLAYCLOCKDIV, 0x80,    // Suggested value 0x80
                        SSD1306_SETMULTIPLEX, 0x3f,    // 1/64
                        SSD1306_SETDISPLAYOFFSET, 0x00,    // 0 no offset
                        SSD1306_SETDISPLAYSTARTLINE + 0,    // line #0
                        SSD1306_MEMORYMODE, 0x00,    // 0x0 act like ks0108
                        SSD1306_SETSEGREMAP_127,
                        SSD1306_COMSCANDEC,
                        SSD1306_SETCOMPINS, 0x12,    //
                        SSD1306_SETCONTRAST, 0xcf,    //
                        SSD1306_SETPRECHARGE, 0xf1,    //
                        SSD1306_SETVCOMDETECT, 0x30,    //
                        SSD1306_DEACTIVATE_SCROLL,
                        SSD1306_CHARGEPUMP, 0x14,    // Charge pump on
                        SSD1306_DISPLAYALLON_RESUME,
                        SSD1306_NORMALDISPLAY };

        uint8_t termcmds [ 3 ] { SSD1306_DISPLAYOFF, SSD1306_CHARGEPUMP, 0x10 };    /// < Charge pump off
};

#endif  /* SSD1306_H */

