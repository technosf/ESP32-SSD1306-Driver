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

#include <stdio.h>

#include <algorithm>
using std::min;
using std::max;

#include "SSD1306.h"

SSD1306::SSD1306( PIF* pif, panel_type_t type )
{
    m_pif = pif;
    m_type = type;
    m_buffer = new uint8_t [ m_type ] [ COLUMNS ];    	// Display buffer - Page by Column
    m_buffer_size = m_type * COLUMNS;
    printf( "SSD1306 type:%d buffer size: %d buffer at %p\n", m_type, m_buffer_size, m_buffer );
    m_height = m_type * 8;         	// panel height (32 or 64)
    m_pixels = m_width * m_height;    // panel pixel count
}

bool SSD1306::init()
{
    if ( m_init ) return false;
    m_init = true;
    term();

    // Now we assume all sending will be successful
    if ( m_type == SSD1306_128x64 )
    {
        printf( "SSD init 64\n" );

        m_pif->command( initcmds64, sizeof ( initcmds64 ) );
    }
    else if ( m_type == SSD1306_128x32 )
    {
        printf( "SSD init 32\n" );
        m_pif->command( initcmds32, sizeof ( initcmds32 ) );
    }

    printf( "SSD init clear\n" );
    clear();
    printf( "SSD init refresh\n" );
    refresh( true );

    printf( "SSD init ON\n" );
    m_pif->command( SSD1306_DISPLAYON );
    printf( "SSD init exit\n" );
    return true;
}

void SSD1306::term()
{
    printf( "SSD term()\n" );
    m_pif->command( termcmds, sizeof ( termcmds ) );
    memset( m_buffer, 0, m_height / 8 );
}

uint8_t SSD1306::width()
{
    return m_width;
}

uint8_t SSD1306::height()
{
    return m_height;
}

void SSD1306::clear()
{
    printf( "SSD Clear buffer size:%d\n", m_buffer_size );

    memset( m_buffer, 0, m_buffer_size );
    // Maximize "Dirty" window
    m_refresh_top = 0;
    m_refresh_left = 0;
    m_refresh_bottom = m_type - 1;
    m_refresh_right = m_width - 1;
}

void SSD1306::refresh( bool force )
{
    if ( ( m_refresh_right < m_refresh_left ) || ( m_refresh_top > m_refresh_bottom ) ) return;

    uint8_t columnstart { m_refresh_left }, columnend { m_refresh_right };
    uint8_t pagestart { m_refresh_top };
    uint8_t pageend { m_refresh_bottom };
    uint8_t segments = m_refresh_right - m_refresh_left;

    if ( force )
    /*
     * Define dirty pages and columns
     */
    {
        columnstart = 0;
        columnend = COLUMNS - 1;
        pagestart = 0;
        pageend = static_cast< uint8_t >( m_type - 1 );
        segments = COLUMNS;
    }

    printf( "SSD Refresh force:%d cs:%d ce:%d ps:%d pe:%d segs:%d\n", force, columnstart, columnend, pagestart, pageend,
            segments );
    uint8_t refresh [] = { SSD1306_COLUMNADDR, columnstart, columnend,    //  Column window
            SSD1306_PAGEADDR, pagestart, pageend };    // Page window

    m_pif->command( refresh, sizeof ( refresh ) );

    for ( int i = pagestart; i <= pageend; i++ )
    {
        printf( "SSD data %p\n", m_buffer [ i ] );
        m_pif->data( m_buffer [ i ] + columnstart, segments );
    }

    // Minimize Dirty Window
    m_refresh_top = 255;
    m_refresh_bottom = 0;
    m_refresh_left = 255;
    m_refresh_right = 0;
}

bool SSD1306::segment( uint8_t page, uint8_t column, uint8_t bits, color_t color, uint8_t count )
{
    if ( count == 0 || ( page >= m_type ) || ( column >= m_width ) ) return false;

    uint8_t endcolumn = ( ( column + count ) > COLUMNS ) ? COLUMNS : ( column + count );

    m_refresh_top = min( m_refresh_top, page );
    m_refresh_bottom = max( m_refresh_bottom, page );
    m_refresh_left = min( m_refresh_left, column );
    m_refresh_right = max( m_refresh_right, static_cast< uint8_t >( endcolumn - 1 ) );

    for ( uint8_t i = column; i < endcolumn; i++ )
    {
        switch ( color )
        {
            case WHITE:
                m_buffer [ page ] [ i ] |= bits;
                break;
            case BLACK:
                m_buffer [ page ] [ i ] &= ~bits;
                break;
            case INVERT:
                m_buffer [ page ] [ i ] ^= bits;
                break;
            default:
                break;
        }    // switch
    }    // for

    return true;
}

bool SSD1306::pixel( uint8_t x, uint8_t y, color_t color )
{
    return segment( y / 8, x, ( 1 << ( y & 7 ) ), color );
}

bool SSD1306::box( uint8_t x, uint8_t y, color_t color, uint8_t w, uint8_t h )
{

    if ( w == 0 || h == 0 ) return false;

    w = min( (int) w, COLUMNS - x );		// Clip X
    h = min( (int) h, m_height - y );    // Clip Y

    uint8_t pagestart = y / 8;
    uint8_t pageend = ( y + h - 1 ) / 8;    // Do not double count origin line

    for ( uint8_t p = ( pagestart + 1 ); p < ( pageend ); p++ )
    /*
     * Fill intermediate pages, if any
     */
    {
        segment( p, x, 0xFF, color, w );
    }

    uint8_t seg = y % 8;		// Start segment
    uint8_t yremainder = 8 - seg;    // Number of bits to draw
    uint8_t filler = BITS [ min( yremainder, h ) - 1 ] << seg;
    segment( pagestart, x, filler, color, w );

    if ( pageend > pagestart )
    {
        seg = ( y + h - 1 ) % 8;		// Number of bits to draw
        filler = BITS [ seg ];    // Get bit pattern a
        segment( pageend, x, filler, color, w );
    }
    return true;
}

bool SSD1306::horizontal( uint8_t x, uint8_t y, color_t color, uint8_t w, uint8_t h )
{
    return box( x, y, color, w, h );
}

bool SSD1306::vertical( uint8_t x, uint8_t y, color_t color, uint8_t h, uint8_t w )
{
    return box( x, y, color, w, h );
}

void SSD1306::invert_display( bool invert )
{
    if ( invert )
        m_pif->command( SSD1306_INVERTDISPLAY );
    else
        m_pif->command( SSD1306_NORMALDISPLAY );
}

void SSD1306::update_buffer( uint8_t* data, uint16_t length )
{
    memcpy( m_buffer, data, min( length, m_buffer_size ) );
}
