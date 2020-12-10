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

#include "SSD1306.h"

using std::min;

/**
 * @brief Construct a new SSD1306::SSD1306 object
 * 
 * @param pif Wire protocol interface adaptor
 * @param type The SSD1306 panel type
 */
SSD1306::SSD1306(PIF *pif, panel_type_t type)
{
    m_pif = pif;
    m_type = type;
    m_buffer = new uint8_t[m_type][COLUMNS]; // Display buffer - Page by Column
    m_buffer_bytes = m_type * COLUMNS;
    ESP_LOGI(TAG, "SSD1306 type:%d buffer size: %d buffer at %p\n", m_type, m_buffer_bytes, m_buffer);
    m_height = m_type * 8;         // panel height (32 or 64)
    m_pixels = m_width * m_height; // panel pixel count
}

/**
 * @brief   Initialize OLED panel
 * @return  true if successful
 * @remark  Possible reasons for failure include non-configured panel type, out of memory or I2C not responding
 */
bool SSD1306::init()
{
    ESP_LOGI(TAG, "init - Start");
    if (m_init)
        return false;
    m_init = true;
    powerdown();

    // Now we assume all sending will be successful
    if (m_type == SSD1306_128x64)
    {
        ESP_LOGD(TAG, "\tcmd: init 64");
        m_pif->command(initcmds64, sizeof(initcmds64));
    }
    else if (m_type == SSD1306_128x32)
    {
        ESP_LOGD(TAG, "\tcmd: init 32");
        m_pif->command(initcmds32, sizeof(initcmds32));
    }
    else
    {
        ESP_LOGE(TAG, "init - Unknown display type");
        return false;
    }

    clear();
    refresh(true);

    ESP_LOGD(TAG, "\tcmd: ON");
    m_pif->command(CMD_DISPLAYON);

    ESP_LOGI(TAG, "init - Complete");
    return true;
}

/**
 * @brief   Turn off display and power, free memory
 * @return  true if successful
 * @remark  Possible reasons for failure include non-configured panel type, out of memory or I2C not responding
 */
void SSD1306::powerdown()
{
    ESP_LOGI(TAG, "powerdown");

    m_pif->command(pwrdwncmds, sizeof(pwrdwncmds));
    memset(m_buffer, 0, m_height / 8);
}

/**
 * @brief   Return OLED panel height
 * @return  Panel height, or return 0 if failed (panel not initialized)
 */
uint8_t SSD1306::width()
{
    return m_width;
}

/**
 * @brief   Return OLED panel height
 * @return  Panel height, or return 0 if failed (panel not initialized)
 */
uint8_t SSD1306::height()
{
    return m_height;
}

/**
 * @brief   Clear display buffer (fill with black)
 * @param   limit Cleared area is limited to the last refreshed area
 */
void SSD1306::clear(bool limit)
{
    ESP_LOGD(TAG, "clear - limit:%d", limit);

    if (!limit)
    {
        memset(m_buffer, 0, m_buffer_bytes);
        m_dirtywindow.touch();
        return;
    }

    // Clear the dirty window only

    uint8_t segments = 1 + m_previous_dirtywindow.rightcol - m_previous_dirtywindow.leftcol;
    for (int page = m_previous_dirtywindow.toppage; page <= m_previous_dirtywindow.bottompage; page++)
    {
        memset(m_buffer[page] + m_previous_dirtywindow.leftcol, 0, segments);
        m_dirtywindow.touch(page, m_previous_dirtywindow.leftcol, m_previous_dirtywindow.rightcol);
    }
}

/**
 * @brief   Refresh display (send display buffer to the panel)
 */
void SSD1306::refresh(bool force)
{
    ESP_LOGD(TAG, "refresh - Force:%d", force);

    if (!m_dirtywindow.isdirty)
        return;

    uint8_t columnstart{m_dirtywindow.leftcol}, columnend{m_dirtywindow.rightcol};
    uint8_t pagestart{m_dirtywindow.toppage};
    uint8_t pageend{m_dirtywindow.bottompage};
    if (pageend >= m_type)
        pageend = m_type - 1;
    uint8_t segments = 1 + columnend - columnstart;

    if (force)
    /*
     * Define dirty pages and columns
     */
    {
        columnstart = 0;
        columnend = COLUMNS - 1;
        pagestart = 0;
        pageend = m_type - 1;
        segments = COLUMNS;
    }

    uint8_t refreshcmd[] = {CMD_COLUMNADDR, columnstart, columnend, //  Column window
                            CMD_PAGEADDR, pagestart, pageend};      // Page window

    m_pif->command(refreshcmd, sizeof(refreshcmd));

    for (int page = pagestart; page <= pageend; page++)
    /*
     * Send data in up to page sized chunks
     */
    {
        m_pif->data(m_buffer[page] + columnstart, segments);
    }

    // Clear Dirty Window
    m_previous_dirtywindow = m_dirtywindow;
    m_dirtywindow.clear();
}

/**
 * @brief 
 * 
* @param   page   the page coord of the column
* @param   column   the column # in the page
* @param   bits   the segment bits to set
* @param   color   the color to set the segment bits
* @param   count   the number of consecutive segments in the page to draw
* @return  True if segment was drawn
* @return  True if segment was drawn 
* @return false 
*/
bool SSD1306::segment(uint8_t page, uint8_t column, uint8_t bits, color_t color, uint8_t count)
{
    // ESP_LOGD(TAG, "::segment page:%d column:%d count:%d", page, column, count);

    if (count == 0 || (page >= m_type) || (column >= m_width))
        return false;

    uint8_t stopbeforecolumn{static_cast<uint8_t>(column + count)};
    if (stopbeforecolumn >= COLUMNS)
        stopbeforecolumn = COLUMNS - 1;

    for (uint8_t i = column; i < stopbeforecolumn; i++)
    {
        switch (color)
        {
        case WHITE:
            m_buffer[page][i] |= bits;
            break;
        case BLACK:
            m_buffer[page][i] &= ~bits;
            break;
        case INVERT:
            m_buffer[page][i] ^= bits;
            break;
        default:
            break;
        } // switch
    }     // for

    m_dirtywindow.touch(page, column, stopbeforecolumn - 1);

    return true;
}

/**
 * @brief	Set the color of a single pixel
 * @param   x   the x coord of the pixel
 * @param   y   the y coord of the pixel
 * @param   color   the desired color of the pixel
 * @param	count	number of times to draw the bits
 * @return  True if pixel was drawn
 */
bool SSD1306::pixel(uint8_t x, uint8_t y, color_t color)
{
    ESP_LOGD(TAG, "pixel - x:%d  y:%d", x, y);
    return segment(y / 8, x, (1 << (y & 7)), color);
}

/**
 * @brief	Draw a filled box of the given color
 * @param   x   the starting x coord of the box
 * @param   y   the starting y coord of the box
 * @param   color   the color to set the box
 * @param   w   the width of the box in pixels to draw
 * @param   h   the height of the box in pixels
 * @return  True if line was drawn
 */
bool SSD1306::box(uint8_t x, uint8_t y, color_t color, uint8_t w, uint8_t h)
{
    ESP_LOGD(TAG, "box - x:%d y:%d w:%d h:%d", x, y, w, h);

    if (w == 0 || h == 0)
        return false;

    w = min(w, (uint8_t)(COLUMNS - x));  // Clip X
    h = min(h, (uint8_t)(m_height - y)); // Clip Y

    uint8_t pagestart = y / 8;
    uint8_t pageend = (y + h - 1) / 8; // Do not double count origin line

    uint8_t seg = y % 8;          // Start segment
    uint8_t yremainder = 8 - seg; // Number of bits to draw
    uint8_t filler = BITS[min(yremainder, h) - 1] << seg;

    /*
     * First page
     */
    segment(pagestart, x, filler, color, w);

    for (uint8_t p = (pagestart + 1); p < (pageend); p++)
    /*
     * Fill intermediate pages, if more than two pages
     */
    {
        segment(p, x, 0xFF, color, w);
    }

    /*
     * Last page, if multiple pages
     */
    if (pageend > pagestart)
    {
        seg = (y + h - 1) % 8; // Number of bits to draw
        filler = BITS[seg];    // Get bit pattern a
        segment(pageend, x, filler, color, w);
    }
    return true;
}

/**
 * @brief	Draw a horizontal line of the given color
 * @param   x   the starting x coord of the line
 * @param   y   the starting y coord of the line
 * @param   color   the color to set the segment bits
 * @param   w   the width of the line in pixels to draw
 * @param   h   the height of the line in pixels, defaults to 1
 * @return  True if line was drawn
 */
bool SSD1306::horizontal(uint8_t x, uint8_t y, color_t color, uint8_t w, uint8_t h)
{
    ESP_LOGD(TAG, "horizontal");
    return box(x, y, color, w, h);
}

/**
 * @brief	Draw a vertical line of the given color
 * @param   x   the starting x coord of the line
 * @param   y   the starting y coord of the line
 * @param   color   the color to set the segment bits
 * @param   h   the height of the line in pixels to draw
 * @param   w   the width of the line in pixels, defaults to 1
 * @return  True if line was drawn
 */
bool SSD1306::vertical(uint8_t x, uint8_t y, color_t color, uint8_t h, uint8_t w)
{
    ESP_LOGD(TAG, "vertical");
    return box(x, y, color, w, h);
}

/**
 * @brief   Draw an aliased line
 *
 * @param   x       X coordinate or starting (top) point
 * @param   y       Y coordinate or starting (top) point
 * @param   color   Color of the line
 * @param   xx      X coordinate or ending (bottom) point
 * @param   yy      Y coordinate or ending (bottom) point
 * @return  Display - Fluent
 */
void SSD1306::line(uint8_t x, uint8_t y, color_t color, uint8_t xx, uint8_t yy)
{
    ESP_LOGD(TAG, "line - %d,%d - %d,%d", x, y, xx, yy);

    if (xx < x) // Ensure we're going left>right
    {
        uint8_t tmp{x};
        x = xx;
        xx = tmp;
        tmp = y;
        y = yy;
        yy = tmp;
    }

    int dx{xx - x};
    int dy{yy - y};
    int ydelta{1};
    if (yy < y)
    {
        dy = y - yy;
        ydelta = -1;
    }

    int p{2 * dy - dx};

    do
    {
        pixel(x, y, color);

        p += (2 * dy);
        if (p >= 0)
        {
            p -= (2 * dx);
            y += ydelta;
        }
    } while ((x < m_width) && (x++ < xx));
}

/**
 * @brief   Set normal or inverted display
 * @param   invert      Invert display?
 */
void SSD1306::invert_display(bool invert)
{
    ESP_LOGD(TAG, "invert_display - invert: %d", invert);

    if (invert)
        m_pif->command(CMD_INVERTDISPLAY);
    else
        m_pif->command(CMD_NORMALDISPLAY);
}

/**
 * @brief   Direct update display buffer
 * @param   data        Data to fill display buffer, no length check is performed!
 * @param   length      Length of data
 */
void SSD1306::update_buffer(uint8_t *data, uint16_t length)
{
    ESP_LOGD(TAG, "update_buffer");
    memcpy(m_buffer, data, min(length, m_buffer_bytes));
}
