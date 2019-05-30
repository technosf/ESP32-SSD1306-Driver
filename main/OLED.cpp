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

#include <OLED.h>

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

Display &OLED::clear()
{
    m_ssd1306.clear();
    return *this;
}

Display &OLED::refresh( bool force )
{
    m_ssd1306.refresh( force );
    return *this;
}

Display &OLED::invert( bool invert )
{
    m_ssd1306.invert_display( invert );
    return *this;
}

Display &OLED::draw_pixel( uint8_t x, uint8_t y, color_t color )
{
    m_ssd1306.pixel( x, y, color );
    return *this;
}

Display &OLED::draw_hline( uint8_t x, uint8_t y, uint8_t w, color_t color )
{
    if ( ( w == 0 ) || ( x >= width() ) || ( y >= height() ) ) return *this;

    m_ssd1306.horizontal( x, y, color, w );

    return *this;
}

Display &OLED::draw_vline( uint8_t x, uint8_t y, uint8_t h, color_t color )
{
    if ( ( h == 0 ) || ( x >= width() ) || ( y >= height() ) ) return *this;

    m_ssd1306.vertical( x, y, color, h );

    return *this;
}

Display &OLED::draw_rectangle( uint8_t x, uint8_t y, uint8_t w, uint8_t h, color_t color )
{
    if ( ( w == 0 ) || ( h == 0 ) || ( x >= width() ) || ( y >= height() ) ) return *this;
    m_ssd1306.vertical( x, y, color, h );
    m_ssd1306.vertical( x + w, y, color, h );
    m_ssd1306.horizontal( x + 1, y, color, w - 1 );
    m_ssd1306.horizontal( x + 1, y + h - 1, color, w - 1 );
    return *this;
}

Display &OLED::fill_rectangle( uint8_t x, uint8_t y, uint8_t w, uint8_t h, color_t color )
{
    if ( ( w == 0 ) || ( h == 0 ) || ( x >= width() ) || ( y >= height() ) ) return *this;

    m_ssd1306.box( x, y, color, w, h );

    return *this;
}

Display &OLED::draw_circle( uint8_t x0, uint8_t y0, uint8_t r, color_t color )
{
// Refer to http://en.wikipedia.org/wiki/Midpoint_circle_algorithm for the algorithm

    if ( r == 0 ) return *this;;

    int8_t x = r;
    int8_t y = 1;
    int16_t radius_err = 1 - x;

    draw_pixel( x0 - r, y0, color );
    draw_pixel( x0 + r, y0, color );
    draw_pixel( x0, y0 - r, color );
    draw_pixel( x0, y0 + r, color );

    while ( x >= y )
    {
        draw_pixel( x0 + x, y0 + y, color );
        draw_pixel( x0 - x, y0 + y, color );
        draw_pixel( x0 + x, y0 - y, color );
        draw_pixel( x0 - x, y0 - y, color );
        if ( x != y )
        {
            /* Otherwise the 4 drawings below are the same as above, causing
             * problem when color is INVERT
             */
            draw_pixel( x0 + y, y0 + x, color );
            draw_pixel( x0 - y, y0 + x, color );
            draw_pixel( x0 + y, y0 - x, color );
            draw_pixel( x0 - y, y0 - x, color );
        }
        ++y;
        if ( radius_err < 0 )
        {
            radius_err += 2 * y + 1;
        }
        else
        {
            --x;
            radius_err += 2 * ( y - x + 1 );
        }

    }
    return *this;
}

Display &OLED::fill_circle( uint8_t x0, uint8_t y0, uint8_t r, color_t color )
{
    int8_t x = 1;
    int8_t y = r;
    int16_t radius_err = 1 - y;
    int8_t x1;

    if ( r == 0 ) return *this;;

    draw_vline( x0, y0 - r, 2 * r + 1, color );    // Center vertical line
    while ( y >= x )
    {
        draw_vline( x0 - x, y0 - y, 2 * y + 1, color );
        draw_vline( x0 + x, y0 - y, 2 * y + 1, color );
        if ( color != INVERT )
        {
            draw_vline( x0 - y, y0 - x, 2 * x + 1, color );
            draw_vline( x0 + y, y0 - x, 2 * x + 1, color );
        }
        ++x;
        if ( radius_err < 0 )
        {
            radius_err += 2 * x + 1;
        }
        else
        {
            --y;
            radius_err += 2 * ( x - y + 1 );
        }
    }

    if ( color == INVERT )
    {
        x1 = x;    // Save where we stopped

        y = 1;
        x = r;
        radius_err = 1 - x;
        draw_hline( x0 + x1, y0, r - x1 + 1, color );
        draw_hline( x0 - r, y0, r - x1 + 1, color );
        while ( x >= y )
        {
            draw_hline( x0 + x1, y0 - y, x - x1 + 1, color );
            draw_hline( x0 + x1, y0 + y, x - x1 + 1, color );
            draw_hline( x0 - x, y0 - y, x - x1 + 1, color );
            draw_hline( x0 - x, y0 + y, x - x1 + 1, color );
            ++y;
            if ( radius_err < 0 )
            {
                radius_err += 2 * y + 1;
            }
            else
            {
                --x;
                radius_err += 2 * ( y - x + 1 );
            }
        }
    }
    return *this;
}

/*
 * Slices characters into segments rather than decompose as individual pixels
 * Requires L-R MSB-LSB scanning bitmaps to be remapped into T-B LSB-MSB segment bitmaps
 */
Display &OLED::draw_char( uint8_t x, uint8_t y, unsigned char c, color_t foreground, color_t background,
        uint8_t* outwidth )
{
    if ( font == NULL )
    {
        if ( outwidth != nullptr ) *outwidth = 0;
        return *this;
    }

    if ( ( c < font->char_start ) || ( c > font->char_end ) ) c = ' ';
    if ( outwidth != nullptr ) *outwidth = font->char_descriptors [ c ].width;

    printf( "Char:\"%c\" X:%d  Y:%d\n", c, x, y );

    // Calculate the page split for the character
    uint8_t page_start = y / 8;
    uint8_t page_end = ( y + font->height ) / 8;
    uint8_t fontsegments [ font->char_descriptors [ c ].width ] [ 1 + page_end - page_start ] = {};    // Col|Page L-R, T-B, fill with background

    const uint8_t * bitmap = font->bitmap + font->char_descriptors [ c ].offset;
    uint8_t scan_length = 1 + ( font->char_descriptors [ c ].width / 8 );
    uint8_t scan;

    for ( uint8_t scan_line = 0; scan_line < font->height; scan_line++ )
    /*
     * Scan the char line by line, T-B
     */
    {
        uint8_t page = page_start + ( scan_line + y ) / 8;
        uint8_t vertical_shift = ( scan_line + y ) % 8;

        printf( "Shift y:%d Line %d, Pages %d %d  Vertical %d  \n", y, scan_line, page_start, page, vertical_shift );

        for ( uint8_t scan_byte = 0; scan_byte < scan_length; scan_byte++ )
        /*
         * Loop through bytes in a scan line
         */
        {
            scan = * ( bitmap + ( scan_line * scan_length ) + scan_byte );    // Horizontal scan line

            printf( "VShift %d  Char loc 0x%04x  byte 0x%02x\n", vertical_shift,
                    ( scan_line * scan_length ) + scan_byte, scan );

            for ( uint8_t col = 8 * scan_byte; col < font->char_descriptors [ c ].width; col++ )
            /*
             * Pick out the bits and OR them into the segment
             */
            {
                uint8_t bit = col % 8;

                if ( ( bit == 0 ) && ( col > 0 ) ) break;    // Byte boundary, breakout to incr scan byte

                bool scanbit = scan & ( 0x80 >> bit );    // Font scans are L-R MSB->LSB, so mask the bit L-R

//				printf( "Col %d  Scan bit 0x%02x  Shifted Scan bit 0x%02x  old Segs 0x%02x ", col, scanbit,
//						scanbit << vertical_shift, fontsegments [ col ] [ page ] );

                fontsegments [ col ] [ page ] |= ( scanbit << vertical_shift );    // Segment bit-or with scan, shifting it down

            }    // for bits
                 //printf( " New Segs 0x%02x\n", fontsegments [ col ] [ page ] );
        }    // for horizontal bytes
    }    // for horizontal lines

    // Push out the remapped character
    for ( uint8_t page = page_start; page <= page_end; page++ )
    {
        for ( uint8_t col = 0; col <= font->char_descriptors [ c ].width; col++ )
        {
            m_ssd1306.segment( page, x + col, fontsegments [ col ] [ page ], foreground );
        }    // for
    }    // for

    return *this;
}    // draw_char

