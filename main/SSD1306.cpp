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

#include <esp_log.h>
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
    ESP_LOGD( TAG, "::init" );
    if ( m_init ) return false;
    m_init = true;
    powerdown();

    // Now we assume all sending will be successful
    if ( m_type == SSD1306_128x64 )
    {
        ESP_LOGD( TAG, "\tcmd: init 64" );
        m_pif->command( initcmds64, sizeof ( initcmds64 ) );
    }
    else if ( m_type == SSD1306_128x32 )
    {
        ESP_LOGD( TAG, "\tcmd: init 32" );
        m_pif->command( initcmds32, sizeof ( initcmds32 ) );
    }

    clear();
    refresh( true );

    ESP_LOGD( TAG, "\tcmd: ON" );
    m_pif->command( SSD1306_DISPLAYON );

    ESP_LOGD( TAG, "::init - complete" );
    return true;
}

void SSD1306::powerdown()
{
    ESP_LOGI( TAG, "::powerdown" );

    m_pif->command( pwrdwncmds, sizeof ( pwrdwncmds ) );
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

void SSD1306::clear( bool limit )
{
    ESP_LOGD( TAG, "::clear  limit:%d", limit );

    if ( !limit )
    {
        memset( m_buffer, 0, m_buffer_size );
        m_dirtywindow.touch();
        return;
    }

    // Clear the dirty window only

    uint8_t segments = 1 + m_previous_dirtywindow.rightcol - m_previous_dirtywindow.leftcol;
    for ( int page = m_previous_dirtywindow.toppage; page <= m_previous_dirtywindow.bottompage; page++ )
    {
        memset( m_buffer [ page ] + m_previous_dirtywindow.leftcol, 0, segments );
        m_dirtywindow.touch( page, m_previous_dirtywindow.leftcol, m_previous_dirtywindow.rightcol );
    }
}

void SSD1306::refresh( bool force )
{
    ESP_LOGD( TAG, "::refresh Force:%d", force );

    if ( !m_dirtywindow.isdirty ) return;

    uint8_t columnstart { m_dirtywindow.leftcol }, columnend { m_dirtywindow.rightcol };
    uint8_t pagestart { m_dirtywindow.toppage };
    uint8_t pageend { m_dirtywindow.bottompage };
    if ( pageend >= m_type ) pageend = m_type - 1;
    uint8_t segments = 1 + columnend - columnstart;

    if ( force )
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

    uint8_t refreshcmd [] = { SSD1306_COLUMNADDR, columnstart, columnend,    //  Column window
            SSD1306_PAGEADDR, pagestart, pageend };    // Page window

    m_pif->command( refreshcmd, sizeof ( refreshcmd ) );

    for ( int page = pagestart; page <= pageend; page++ )
    /*
     * Send data in up to page sized chunks
     */
    {
        m_pif->data( m_buffer [ page ] + columnstart, segments );
    }

    // Clear Dirty Window
    m_previous_dirtywindow = m_dirtywindow;
    m_dirtywindow.clear();
}

bool SSD1306::segment( uint8_t page, uint8_t column, uint8_t bits, color_t color, uint8_t count )
{
    ESP_LOGD( TAG, "::segment page:%d column:%d count:%d", page, column, count );

    if ( count == 0 || ( page >= m_type ) || ( column >= m_width ) ) return false;

    uint8_t stopbeforecolumn { static_cast< uint8_t >( column + count ) };
    if ( stopbeforecolumn >= COLUMNS ) stopbeforecolumn = COLUMNS - 1;

    for ( uint8_t i = column; i < stopbeforecolumn; i++ )
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

    m_dirtywindow.touch( page, column, stopbeforecolumn - 1 );

    return true;
}

bool SSD1306::pixel( uint8_t x, uint8_t y, color_t color )
{
    ESP_LOGD( TAG, "::pixel  x:%d  y:%d", x, y );
    return segment( y / 8, x, ( 1 << ( y & 7 ) ), color );
}

bool SSD1306::box( uint8_t x, uint8_t y, color_t color, uint8_t w, uint8_t h )
{
    ESP_LOGD( TAG, "::box x:%d y:%d w:%d h:%d", x, y, w, h );

    if ( w == 0 || h == 0 ) return false;

    w = min( (int) w, COLUMNS - x );		// Clip X
    h = min( (int) h, m_height - y );    // Clip Y

    uint8_t pagestart = y / 8;
    uint8_t pageend = ( y + h - 1 ) / 8;    // Do not double count origin line

    uint8_t seg = y % 8;        // Start segment
    uint8_t yremainder = 8 - seg;    // Number of bits to draw
    uint8_t filler = BITS [ min( yremainder, h ) - 1 ] << seg;

    /*
     * First page
     */
    segment( pagestart, x, filler, color, w );

    for ( uint8_t p = ( pagestart + 1 ); p < ( pageend ); p++ )
    /*
     * Fill intermediate pages, if more than two pages
     */
    {
        segment( p, x, 0xFF, color, w );
    }

    /*
     * Last page, if multiple pages
     */
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
    ESP_LOGD( TAG, "::horizontal >" );
    return box( x, y, color, w, h );
}

bool SSD1306::vertical( uint8_t x, uint8_t y, color_t color, uint8_t h, uint8_t w )
{
    ESP_LOGD( TAG, "::vertical >" );
    return box( x, y, color, w, h );
}

void SSD1306::line( uint8_t x, uint8_t y, color_t color, uint8_t xx, uint8_t yy )
{
    ESP_LOGD( TAG, "::line  %d,%d - %d,%d", x, y, xx, yy );

    if ( xx < x )    // Ensure we're going left>right
    {
        uint8_t tmp { x };
        x = xx;
        xx = tmp;
        tmp = y;
        y = yy;
        yy = tmp;
    }

    int dx { xx - x };
    int dy { yy - y };
    int ydelta { 1 };
    if ( yy < y )
    {
        dy = y - yy;
        ydelta = -1;
    }

    int p { 2 * dy - dx };

    do
    {
        if ( p >= 0 )
        {
            pixel( x, y, color );
            p = p + ( 2 * dy ) - ( 2 * dx );
            y += ydelta;
        }
        else
        {
            pixel( x, y, color );
            p = p + ( 2 * dy );
        }
    }
    while ( ( x < m_width ) && ( x++ < xx ) );
}

void SSD1306::invert_display( bool invert )
{
    ESP_LOGD( TAG, "::invert_display invert:%d", invert );

    if ( invert )
        m_pif->command( SSD1306_INVERTDISPLAY );
    else
        m_pif->command( SSD1306_NORMALDISPLAY );
}

void SSD1306::update_buffer( uint8_t* data, uint16_t length )
{
    ESP_LOGD( TAG, "::update_buffer" );
    memcpy( m_buffer, data, min( length, m_buffer_size ) );
}
