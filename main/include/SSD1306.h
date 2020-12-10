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

#ifndef SSD1306_SSD1306_H_
#define SSD1306_SSD1306_H_

#define CMD_CHARGEPUMP 0x8d
#define CMD_COLUMNADDR 0x21
#define CMD_COMSCANDEC 0xc8
#define CMD_DEACTIVATE_SCROLL 0x2e
#define CMD_DISPLAYALLON_RESUME 0xa4
#define CMD_DISPLAYOFF 0xae
#define CMD_DISPLAYON 0xaf
#define CMD_INVERTDISPLAY 0xa7
#define CMD_MEMORYMODE 0x20
#define CMD_NORMALDISPLAY 0xa6
#define CMD_PAGEADDR 0x22
#define CMD_SETCOMPINS 0xda
#define CMD_SETCONTRAST 0x81
#define CMD_SETDISPLAYCLOCKDIV 0xd5
#define CMD_SETDISPLAYOFFSET 0xd3
#define CMD_SETDISPLAYSTARTLINE 0x40
#define CMD_SETMULTIPLEX 0xa8
#define CMD_SETPRECHARGE 0xd9
#define CMD_SETSEGREMAP_0 0xa0
#define CMD_SETSEGREMAP_127 0xa1
#define CMD_SETVCOMDETECT 0xdb

#include <esp_log.h>
#include <stdio.h>

#include <algorithm>
#include <string.h>
#include <stdint.h>
#include <algorithm>

#include "PIF.h"

/**
 * @brief Color options supported by the SSD1306
 * 
 */
enum color_t
{
    TRANSPARENT = -1, ///< Transparent (not drawing)
    BLACK = 0,        ///< Black (pixel off)
    WHITE = 1,        ///< White (or blue, yellow, pixel on)
    INVERT = 2,       ///< Invert pixel (XOR)
};

/**
 * @brief Panel type
 * 
 * Panel type and dimensions, with the enumerator indicating the number of memory pages
 */
enum panel_type_t
{
    SSD1306_128x64 = 8, ///< 128x32 panel, 8 pages of memory
    SSD1306_128x32 = 4  ///< 128x64 panel, 4 pages of memory
};

/**
 * @brief SSD1306 chip driver, commands and controls
 * 
 */
class SSD1306
{
    static const constexpr char *TAG = "SSD1306";

public:
    SSD1306(PIF *pif, panel_type_t type);

    virtual ~SSD1306()
    {
    }

    bool init();
    void powerdown();
    uint8_t width();
    uint8_t height();
    void clear(bool limit = false);
    void refresh(bool force);
    bool segment(uint8_t page, uint8_t column, uint8_t bits, color_t color, uint8_t count = 1);
    bool pixel(uint8_t x, uint8_t y, color_t color);
    bool box(uint8_t x, uint8_t y, color_t color, uint8_t w, uint8_t h);
    bool horizontal(uint8_t x, uint8_t y, color_t color, uint8_t w, uint8_t h = 1);
    bool vertical(uint8_t x, uint8_t y, color_t color, uint8_t h, uint8_t w = 1);
    void line(uint8_t x, uint8_t y, color_t color, uint8_t xx, uint8_t yy);
    void invert_display(bool invert);
    void update_buffer(uint8_t *data, uint16_t length);

private:
    static const constexpr uint8_t COLUMNS = 128; ///< SSD1306 is a 128 column driver chip

    const uint8_t BITS[8] = {0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF}; ///< Segment bit mask

    bool m_init{false};
    PIF *m_pif;                   ///< Wire protocol adapter
    panel_type_t m_type;          ///< panel type
    uint8_t (*m_buffer)[COLUMNS]; ///< Display buffer - Page by Column
    uint16_t m_buffer_bytes;       ///< buffer size in bytes
    uint8_t m_width{COLUMNS};     ///< panel width (128)
    uint8_t m_height;             ///< panel height (32 or 64)
    uint16_t m_pixels;            ///< panel pixel count

    struct dirtywindow ///< "Dirty" window
    {
        bool isdirty{false};
        uint8_t toppage{255};
        uint8_t leftcol{255};
        uint8_t rightcol{0};
        uint8_t bottompage{0};

        void clear() ///< Clear the dirty window
        {
            toppage = 255;
            leftcol = 255;
            rightcol = 0;
            bottompage = 0;
            isdirty = false;
        }

        void touch(uint8_t page, uint8_t colstart, uint8_t colend = 0) ///< Touch part of the window
        {
            toppage = std::min(toppage, page);
            bottompage = std::max(bottompage, page);
            leftcol = std::min(leftcol, colstart);
            rightcol = std::max(rightcol, colend);
            isdirty = true;
        }

        void touch() ///< Touch the entire window
        {
            toppage = 0;
            bottompage = 255;
            leftcol = 0;
            rightcol = COLUMNS - 1;
            isdirty = true;
        }
    } m_dirtywindow;

    dirtywindow m_previous_dirtywindow;

    uint8_t initcmds32[25] = ///< initiate 32 line display
        {CMD_DISPLAYOFF,
         CMD_SETDISPLAYCLOCKDIV, 0x80, ///< Suggested value 0x80
         CMD_SETMULTIPLEX, 0x1f,       ///< 1/32
         CMD_SETDISPLAYOFFSET, 0x00,   ///< 0 no offset
         CMD_SETDISPLAYSTARTLINE + 0,  ///< line #0
         CMD_MEMORYMODE, 0x00,         ///< 0x0 act like ks0108
         CMD_SETSEGREMAP_127,
         CMD_COMSCANDEC,
         CMD_SETCOMPINS, 0x02,
         CMD_SETCONTRAST, 0x2f,
         CMD_SETPRECHARGE, 0xf1,
         CMD_SETVCOMDETECT, 0x40,
         CMD_DEACTIVATE_SCROLL,
         CMD_CHARGEPUMP, 0x14, ///< Charge pump on
         CMD_DISPLAYALLON_RESUME,
         CMD_NORMALDISPLAY};

    uint8_t initcmds64[25] = ///< Initiate 64 line display
        {CMD_DISPLAYOFF,
         CMD_SETDISPLAYCLOCKDIV, 0x80, ///< Suggested value 0x80
         CMD_SETMULTIPLEX, 0x3f,       ///< 1/64
         CMD_SETDISPLAYOFFSET, 0x00,   ///< 0 no offset
         CMD_SETDISPLAYSTARTLINE + 0,  ///< line #0
         CMD_MEMORYMODE, 0x00,         ///< 0x0 act like ks0108
         CMD_SETSEGREMAP_127,
         CMD_COMSCANDEC,
         CMD_SETCOMPINS, 0x12,
         CMD_SETCONTRAST, 0xcf,
         CMD_SETPRECHARGE, 0xf1,
         CMD_SETVCOMDETECT, 0x30,
         CMD_DEACTIVATE_SCROLL,
         CMD_CHARGEPUMP, 0x14, ///< Charge pump on
         CMD_DISPLAYALLON_RESUME,
         CMD_NORMALDISPLAY};

    uint8_t pwrdwncmds[3]{CMD_DISPLAYOFF, CMD_CHARGEPUMP, 0x10}; ///< Charge pump off
};

#endif /* SSD1306_SSD1306_H */
