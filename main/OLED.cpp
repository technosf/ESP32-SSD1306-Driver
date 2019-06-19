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

#include <stdio.h>
#include <stdint.h>
#include <algorithm>
#include <esp_log.h>

using std::min;
using std::max;

#include <OLED.h>

struct slice
{
        uint8_t x;
        uint8_t y;
        uint8_t yy;
        uint8_t ymin()
        {
            return min( y, yy );
        }

        uint8_t ymax()
        {
            return max( y, yy );
        }

        uint8_t height()
        {
            return abs( yy - y );
        }
};

void slicepush( std::vector< slice > &slices, int8_t x, int8_t ymin, int8_t ymax );
std::vector< slice > circle_points( uint8_t x0, uint8_t y0, uint8_t r );

OLED::OLED( SSD1306& ssd1306 ) :
        m_ssd1306 { ssd1306 }
{
    m_ssd1306.init();
}

uint8_t OLED::width()
{
    return m_ssd1306.width();
}

uint8_t OLED::height()
{
    return m_ssd1306.height();
}

Display& OLED::clear( bool limit )
{
    m_ssd1306.clear( limit );
    return *this;
}

Display& OLED::refresh( bool force )
{
    m_ssd1306.refresh( force );
    return *this;
}

Display& OLED::invert( bool invert )
{
    m_ssd1306.invert_display( invert );
    return *this;
}

Display& OLED::draw_pixel( uint8_t x, uint8_t y, color_t color )
{
    m_ssd1306.pixel( x, y, color );
    return *this;
}

Display& OLED::draw_hline( uint8_t x, uint8_t y, uint8_t w, color_t color )
{
    if ( ( w == 0 ) || ( x >= width() ) || ( y >= height() ) ) return *this;

    m_ssd1306.horizontal( x, y, color, w );

    return *this;
}

Display& OLED::draw_vline( uint8_t x, uint8_t y, uint8_t h, color_t color )
{
    if ( ( h == 0 ) || ( x >= width() ) || ( y >= height() ) ) return *this;

    m_ssd1306.vertical( x, y, color, h );

    return *this;
}

Display& OLED::draw_line( uint8_t x, uint8_t y, uint8_t xx, uint8_t yy, color_t color )
{
    if ( ( x >= width() ) || ( y >= height() ) ) return *this;

    m_ssd1306.line( x, y, color, xx, yy );

    return *this;
}

Display& OLED::draw_rectangle( uint8_t x, uint8_t y, uint8_t w, uint8_t h, color_t color )
{
    if ( ( w == 0 ) || ( h == 0 ) || ( x >= width() ) || ( y >= height() ) ) return *this;
    m_ssd1306.vertical( x, y, color, h );
    m_ssd1306.vertical( x + w, y, color, h );
    m_ssd1306.horizontal( x + 1, y, color, w - 1 );
    m_ssd1306.horizontal( x + 1, y + h - 1, color, w - 1 );
    return *this;
}

Display& OLED::fill_rectangle( uint8_t x, uint8_t y, uint8_t w, uint8_t h, color_t color )
{
    if ( ( w == 0 ) || ( h == 0 ) || ( x >= width() ) || ( y >= height() ) ) return *this;

    m_ssd1306.box( x, y, color, w, h );

    return *this;
}

Display& OLED::draw_circle( uint8_t x0, uint8_t y0, uint8_t r, color_t color )
{
    if ( r == 0 ) return *this;

    for ( auto& point : circle_points( x0, y0, r ) )
    {
        draw_pixel( point.x, point.ymin(), color );
        draw_pixel( point.x, point.ymax(), color );
    }

    return *this;
}

Display& OLED::fill_circle( uint8_t x0, uint8_t y0, uint8_t r, color_t color )
{
    if ( r == 0 ) return *this;

    for ( auto& point : circle_points( x0, y0, r ) )
    {
        draw_vline( point.x, point.ymin(), point.height(), color );
    }

    return *this;
}

/*
 * Slices characters into segments rather than decompose as individual pixels
 * Requires L-R MSB-LSB scanning bitmaps to be remapped into T-B LSB-MSB segment bitmaps
 */
Display& OLED::draw_char( uint8_t x, uint8_t y, unsigned char c, color_t foreground, color_t background,
        uint8_t* outwidth )
{
    ESP_LOGD( TAG, "::draw_char" );

    if ( fm == nullptr || c == 0 )
    {
        if ( outwidth != nullptr ) *outwidth = 0;
        return *this;
    }

    Font_Manager::bitmap scan = fm->rasterize( c, y );
    uint8_t * d = scan.data;

    for ( int i = y / 8; i < ( y / 8 ) + scan.height; i++ )
    {
        for ( int j = x; j < x + scan.width; j++ )
        {
            m_ssd1306.segment( i, j, *d++, foreground );
        }
    }

    if ( outwidth != nullptr ) *outwidth = ( scan.bitwidth );
    return *this;
}    // draw_char

/**
 * @brief   Draw string using currently selected font
 * @param   x           X position of string (top-left corner)
 * @param   y           Y position of string (top-left corner)
 * @param   str         The string to draw
 * @param   foreground  Character color
 * @param   background  Background color
 * @return  Width of the string (out-of-display pixels also included)
 * @return  Display - Fluent
 */
Display& OLED::draw_string( uint8_t x, uint8_t y, std::string str, color_t foreground, color_t background,
        uint8_t* outwidth )
{
    ESP_LOGD( TAG, "::draw_string" );

    if ( fm == nullptr || str.empty() )
    {
        if ( outwidth != nullptr ) *outwidth = 0;
        return *this;
    }

    Font_Manager::bitmap scan = fm->rasterize( str, y );
    uint8_t * d = scan.data;

    for ( int i = y / 8; i < ( y / 8 ) + scan.height; i++ )
    {
        for ( int j = x; j < x + scan.width; j++ )
        {
            m_ssd1306.segment( i, j, *d++, foreground );
        }
    }

    if ( outwidth != nullptr ) *outwidth = ( scan.bitwidth );
    return *this;
}

/**
 * @brief   Measure width of string with current selected font
 * @param   str         String to measure
 * @return  Width of the string
 */
uint8_t OLED::measure_string( std::string str )    //final
{
    if ( fm == NULL || str.empty() ) return 0;

    return fm->measure_string( str );
}

/**
 * @brief   Get the font name
 * @return  font name
 */
const char * OLED::font_name()
{
    return fm->font_name();
}

/**
 * @brief   Get the height of current selected font
 * @return  Height of the font (in pixels) or 0 if none font selected
 */
uint8_t OLED::font_height()
{
    if ( fm == nullptr ) return 0;
    return ( fm->font_height() );
}

/**
 * @brief   Get the "C" value (space between adjacent characters)
 * @return  "C" value
 */
uint8_t OLED::font_c()
{
    if ( fm == NULL ) return 0;
    return ( fm->font_c() );
}

/**
 * @brief   Select font for drawing
 * @param   idx     Font index, see fonts.c
 */
Display& OLED::select_font( uint8_t idx )
{
    if ( idx < Font_Manager::fontcount() ) fm = new Font_Manager( idx, Font_Manager::TBLR );
    return *this;
}

void slicepush( std::vector< slice > &slices, int8_t x, int8_t ymin, int8_t ymax )
{
    static int8_t zero { 0 };
    if ( x < 0 ) return;
    slices.push_back( { (uint8_t) x, (uint8_t) max( zero, ymin ), (uint8_t) max( zero, ymax ) } );
}

std::vector< slice > circle_points( uint8_t x0, uint8_t y0, uint8_t radius )
{
    std::vector< slice > slices;
    if ( radius == 0 ) return slices;

    int f = 1 - radius;
    int ddF_x = 0;
    int ddF_y = -2 * radius;
    int x = 0;
    int y = radius;

    slicepush( slices, x0, y0 - radius, y0 + radius );
    slicepush( slices, x0 + radius, y0, y0 );
    slicepush( slices, x0 - radius, y0, y0 );

    while ( ++x < y )
    {
        if ( f >= 0 )
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        ddF_x += 2;
        f += ddF_x + 1;
        slicepush( slices, x0 + x, y0 - y, y0 + y );
        slicepush( slices, x0 - x, y0 - y, y0 + y );
        if ( x != y )
        {
            slicepush( slices, x0 + y, y0 - x, y0 + x );
            slicepush( slices, x0 - y, y0 - x, y0 + x );
        }
    }

    return slices;
}
