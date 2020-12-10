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

#include <OLED.h>

using std::max;
using std::min;
using std::vector;

/**
 * @brief Describes a line verticle slice
 * 
 */
struct slice
{
    uint8_t x;
    int8_t y;
    int8_t yy;
    bool top_inrange;
    bool bottom_inrange;

    slice(int8_t _x, int8_t _y, int8_t _yy)
    {
        x = _x;
        y = min(_y, _yy);
        yy = max(_y, _yy);
        top_inrange = (yy >= 0);
        bottom_inrange = (y >= 0);
    };
};

/**
 * @brief Compares to slices 
 * 
 * @param slice1 
 * @param slice2 
 * @return true slice1 is before slice2 on the x-axis
 * @return false if slice1 is not smaller than slice two
 */
bool compareSliceX(slice slice1, slice slice2)
{
    return slice1.x < slice2.x;
}

// -----------------------------------------------------------------------------------------

/**
 * @brief Calculates and store a verticle slice of a circle along the Y-axis
 * 
 * Only the visible perimiter of the circle is calculated
 * 
 * @param slices the vestor to store the result in
 * @param x the slice x coord
 * @param ymin the minimum Y coordinate of this slice
 * @param ymax the maximum Y coordinate of this slice
 */
void slicepush(std::vector<slice> &slices, int8_t x, int8_t y, int8_t yy)
{
    if (x < 0)
        return;
    slices.emplace_back(x, y, yy);
}

/**
 * @brief Calculates all the points on the circumference of a circle
 * 
 * @param x0 X-origin
 * @param y0 Y-origin
 * @param radius radius of the circle
 * 
 * @return vector< slice > The resulting set of slices that describe the circle
 */
vector<slice> circle_points(uint8_t x0, uint8_t y0, uint8_t radius)
{
    vector<slice> slices;

    if (radius == 0)
        return slices;

    int f = 1 - radius;
    int ddF_x = 0;
    int ddF_y = -2 * radius;
    int x = 0;
    int y = radius;

    slicepush(slices, x0, y0 - radius, y0 + radius);
    slicepush(slices, x0 + radius, y0, y0);
    slicepush(slices, x0 - radius, y0, y0);

    while (++x < y)
    {
        if (f >= 0)
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        ddF_x += 2;
        f += ddF_x + 1;
        slicepush(slices, x0 + x, y0 - y, y0 + y);
        slicepush(slices, x0 - x, y0 - y, y0 + y);
        if (x != y)
        {
            slicepush(slices, x0 + y, y0 - x, y0 + x);
            slicepush(slices, x0 - y, y0 - x, y0 + x);
        }
    } // while
    return slices;
}

// ---------------------------------------------------------------------------------------------------------------

/**
 * @brief Construct a new OLED::OLED object
 * 
 * @param ssd1306 
 */
OLED::OLED(SSD1306 &ssd1306) : m_ssd1306{ssd1306}
{
    m_ssd1306.init();
}

/**
 * @brief   Return OLED panel height
 * 
 * @return  Panel height, or return 0 if failed (panel not initialized)
 */
uint8_t OLED::width()
{
    return m_ssd1306.width();
}
/**
 * @brief   Return OLED panel height
 * 
 * @return  Panel height, or return 0 if failed (panel not initialized)
 */
uint8_t OLED::height()
{
    return m_ssd1306.height();
}

/**
 * @brief   Clear display buffer (fill with black)
 * 
 * @param   limit Cleared area is limited to the last refreshed area
 * @return  Display - Fluent
 */
Display &OLED::clear(bool limit)
{
    m_ssd1306.clear(limit);
    return *this;
}
/**
 * @brief   Refresh display (send display buffer to the panel)
 * 
 * @param   force   The program automatically tracks "dirty" region to minimize refresh area. Set #force to true
 *                  ignores the dirty region and refresh the whole screen.
 */
Display &OLED::refresh(bool force)
{
    m_ssd1306.refresh(force);
    return *this;
}

/**
 * @brief   Set normal or inverted display
 * 
 * @param   invert      Invert display?
 */
Display &OLED::invert(bool invert)
{
    m_ssd1306.invert_display(invert);
    return *this;
}
/**
 * @brief   Draw one pixel
 * 
 * @param   x       X coordinate
 * @param   y       Y coordinate
 * @param   color   Color of the pixel
 */
Display &OLED::draw_pixel(uint8_t x, uint8_t y, color_t color)
{
    m_ssd1306.pixel(x, y, color);
    return *this;
}

/**
 * @brief   Draw horizontal line
 * 
 * @param   x       X coordinate or starting (left) point
 * @param   y       Y coordinate or starting (left) point
 * @param   w       Line width
 * @param   color   Color of the line
 */
Display &OLED::draw_hline(uint8_t x, uint8_t y, uint8_t w, color_t color)
{
    if ((w == 0) || (x >= width()) || (y >= height()))
        return *this;

    m_ssd1306.horizontal(x, y, color, w);

    return *this;
}
/**
 * @brief   Draw vertical line
 * 
 * @param   x       X coordinate or starting (top) point
 * @param   y       Y coordinate or starting (top) point
 * @param   h       Line height
 * @param   color   Color of the line
 */
Display &OLED::draw_vline(uint8_t x, uint8_t y, uint8_t h, color_t color)
{
    if ((h == 0) || (x >= width()) || (y >= height()))
        return *this;

    m_ssd1306.vertical(x, y, color, h);

    return *this;
}

/**
 * @brief   Draw an aliased line
 * 
 * @param   x       X coordinate or starting (top) point
 * @param   y       Y coordinate or starting (top) point
 * @param   xx      X coordinate or ending (bottom) point
 * @param   yy      Y coordinate or ending (bottom) point
 * @param   color   Color of the line
 * @return  Display - Fluent
 */
Display &OLED::draw_line(uint8_t x, uint8_t y, uint8_t xx, uint8_t yy, color_t color)
{
    if ((x >= width()) || (y >= height()))
        return *this;

    m_ssd1306.line(x, y, color, xx, yy);

    return *this;
}

/**
 * @brief   Draw a rectangle
 * 
 * @param   x       X coordinate or starting (top left) point
 * @param   y       Y coordinate or starting (top left) point
 * @param   w       Rectangle width
 * @param   h       Rectangle height
 * @param   color   Color of the rectangle border
 */
Display &OLED::draw_rectangle(uint8_t x, uint8_t y, uint8_t w, uint8_t h, color_t color)
{
    if ((w == 0) || (h == 0) || (x >= width()) || (y >= height()))
        return *this;
    m_ssd1306.vertical(x, y, color, h);
    m_ssd1306.vertical(x + w, y, color, h);
    m_ssd1306.horizontal(x + 1, y, color, w - 1);
    m_ssd1306.horizontal(x + 1, y + h - 1, color, w - 1);
    return *this;
}

/**
 * @brief   Draw a filled rectangle
 * 
 * @param   x       X coordinate or starting (top left) point
 * @param   y       Y coordinate or starting (top left) point
 * @param   w       Rectangle width
 * @param   h       Rectangle height
 * @param   color   Color of the rectangle
 */
Display &OLED::fill_rectangle(uint8_t x, uint8_t y, uint8_t w, uint8_t h, color_t color)
{
    if ((w == 0) || (h == 0) || (x >= width()) || (y >= height()))
        return *this;

    m_ssd1306.box(x, y, color, w, h);

    return *this;
}

/**
 * @brief   Draw a filled circle
 * 
 * @param   x0      X coordinate or center
 * @param   y0      Y coordinate or center
 * @param   r       Radius
 * @param   color   Color of the circle
 */
Display &OLED::draw_circle(uint8_t x0, uint8_t y0, uint8_t r, color_t color)
{
    if (r == 0)
        return *this;

    vector<slice> points = circle_points(x0, y0, r);

    for (auto &point : points)
    {
        draw_pixel(point.x, point.y, color);
        draw_pixel(point.x, point.yy, color);
    }

    return *this;
}
/**
 * @brief   Draw a filled circle
 * 
 * @param   x0      X coordinate or center
 * @param   y0      Y coordinate or center
 * @param   r       Radius
 * @param   color   Color of the circle
 */
Display &OLED::fill_circle(uint8_t x0, uint8_t y0, uint8_t r, color_t color)
{
    if (r == 0)
        return *this;

    vector<slice> points = circle_points(x0, y0, r);
    sort(points.begin(), points.end(), compareSliceX); 

    uint8_t x{points[0].x};
    int8_t y;
    int8_t yy;

    for (auto &point : points)
    {
        if (point.x != x)
        {
            draw_vline(x, y, yy - y + 1, color);
            x = point.x;
            y = point.y;
            yy = point.yy;
        }
        y = min(y, point.y);
        yy = max(yy, point.yy);
    } // for
    draw_vline(x, y, yy - y + 1, color);
    return *this;
}

/**
 * @brief   Draw one character using currently selected font
 * 
 * Slices characters into segments rather than decompose as individual pixels
 * Requires L-R MSB-LSB scanning bitmaps to be remapped into T-B LSB-MSB segment bitmaps
 * 
 * @param   x           X position of character (top-left corner)
 * @param   y           Y position of character (top-left corner)
 * @param   c           The character to draw
 * @param   foreground  Character color
 * @param   background  Background color
 * @return  Width of the character
 */

Display &OLED::draw_char(uint8_t x, uint8_t y, unsigned char c, color_t foreground, color_t background,
                         uint8_t *outwidth)
{
    ESP_LOGD(TAG, "draw_char");

    if (m_font_manager == nullptr || c == 0)
    {
        if (outwidth != nullptr)
            *outwidth = 0;
        return *this;
    }

    Font_Manager::bitmap scan = m_font_manager->rasterize(c, y);
    uint8_t *d = scan.data;
    uint16_t ylimit = (y / 8) + scan.height_bytes;
    uint16_t xlimit = x + scan.width_bytes;

    for (int yy = y / 8; yy < ylimit; yy++)
    {
        for (int xx = x; xx < xlimit; xx++)
        {
            m_ssd1306.segment(yy, xx, *d++, foreground);
        }
    }

    if (outwidth != nullptr)
        *outwidth = (scan.bitwidth);
    return *this;
} // draw_char

/**
 * @brief   Draw string using currently selected font
 * 
 * @param   x           X position of string (top-left corner)
 * @param   y           Y position of string (top-left corner)
 * @param   str         The string to draw
 * @param   foreground  Character color
 * @param   background  Background color
 * @return  Width of the string (out-of-display pixels also included)
 * @return  Display - Fluent
 */
Display &OLED::draw_string(uint8_t x, uint8_t y, std::string str, color_t foreground, color_t background,
                           uint8_t *outwidth)
{
    if (m_font_manager == nullptr || str.empty())
    {
        if (outwidth != nullptr)
            *outwidth = 0;
        return *this;
    }

    Font_Manager::bitmap scan = m_font_manager->rasterize(str, y);
    uint8_t *d = scan.data;
    uint16_t ylimit = (y / 8) + scan.height_bytes;
    uint16_t xlimit = x + scan.width_bytes;

    for (int yy = y / 8; yy < ylimit; yy++)
    {
        for (int xx = x; xx < xlimit; xx++)
        {
            m_ssd1306.segment(yy, xx, *d++, foreground);
        }
    }

    if (outwidth != nullptr)
        *outwidth = (scan.bitwidth);
    return *this;
}

/**
 * @brief   Measure width of string with current selected font
 * 
 * @param   str         String to measure
 * @return  Width of the string
 */
uint8_t OLED::measure_string(std::string str) //final
{
    if (m_font_manager == NULL || str.empty())
        return 0;

    return m_font_manager->measure_string(str);
}

/**
 * @brief   Get the font name
 * 
 * @return  font name
 */
const char *OLED::font_name()
{
    return m_font_manager->font_name();
}

/**
 * @brief   Get the height of current selected font
 * 
 * @return  Height of the font (in pixels) or 0 if none font selected
 */
uint8_t OLED::font_height()
{
    if (m_font_manager == nullptr)
        return 0;
    return (m_font_manager->font_height());
}

/**
 * @brief   Get the "C" value (space between adjacent characters)
 * 
 * @return  "C" value
 */
uint8_t OLED::font_c()
{
    if (m_font_manager == NULL)
        return 0;
    return (m_font_manager->font_c());
}

/**
 * @brief   Select font for drawing
 * 
 * @param   idx     Font index, see fonts.c
 */
Display &OLED::select_font(uint8_t idx)
{
    if (idx < Font_Manager::fontcount())
        m_font_manager = new Font_Manager(idx, Font_Manager::TBLR);
    return *this;
}
