/*
 ESP32-SSD1306-Driver Library Example code

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

//#define __i2c
#define __i2c

#include <esp_system.h>
#include <driver/gpio.h>
#include <math.h>

#include "Display.h"
#include "SSD1306.h"
#include <OLED.h>

#ifdef __i2c

#include "I2C_PIF.h"
#define scl GPIO_NUM_19
#define sda GPIO_NUM_18
#define address 0x3c
#endif
#ifdef __spi

#include "SPI_PIF.h"
#define  mosi GPIO_NUM_13
#define  clk GPIO_NUM_14
#define  cs GPIO_NUM_15
#define  dc GPIO_NUM_2

#endif

extern "C" void app_main();

void rectangle( Display* display );
void lines( Display* display, int countdown );
void circles( Display* display, int n );

void app_main()
{

    PIF* pif;

#ifdef __i2c
    pif = new I2C_PIF { scl, GPIO_NUM_18, 0x3c };
    pif->info();
#endif

#ifdef __spi
    //SPI_PIF spi { mosi, clk, cs, dc };
#endif

    SSD1306 ssd1306( pif, SSD1306_128x64 );
    OLED display = OLED( ssd1306 );

    display.select_font( 19 ).clear();
//    display.draw_string( 0, 0, "ESP32-SSD1306-Driver", WHITE, BLACK ).refresh();
//    display.draw_string( 0, 8, "ESP32-SSD1306-Driver", WHITE, BLACK ).refresh();
//    display.draw_string( 0, 16, "ESP32-SSD1306-Driver", WHITE, BLACK ).refresh();
//    display.draw_string( 0, 24, "ESP32-SSD1306-Driver", WHITE, BLACK ).refresh();
//    display.draw_string( 0, 32, "ESP32-SSD1306-Driver", WHITE, BLACK ).refresh();
//    display.draw_string( 0, 48, "ESP32-SSD1306-Driver", WHITE, BLACK ).refresh();
//    display.draw_string( 0, 56, "ESP32-SSD1306-Driver", WHITE, BLACK ).refresh();

//    display.draw_string( 0, 0, "@", WHITE, BLACK ).refresh();
//    display.draw_string( 6, 0, "@", WHITE, BLACK ).refresh();
    display.draw_string( 0, 28, "@", WHITE, BLACK ).refresh();
    display.draw_string( 33, 29, "@", WHITE, BLACK ).refresh();
    display.draw_string( 77, 30, "@", WHITE, BLACK ).refresh();
    display.draw_string( 99, 31, "@", WHITE, BLACK ).refresh();

//    display.draw_string( 0, 8, "@", WHITE, BLACK ).refresh();
//    display.draw_string( 0, 16, "@", WHITE, BLACK ).refresh();
//    display.draw_string( 0, 24, "@", WHITE, BLACK ).refresh();
//    display.draw_string( 0, 32, "@", WHITE, BLACK ).refresh();
//    display.draw_string( 0, 48, "@", WHITE, BLACK ).refresh();
//    display.draw_string( 0, 56, "@", WHITE, BLACK ).refresh();
    sleep( .5 );
    // rectangle( &display );

//    sleep( 2 );
//
//    display.select_font( 0 ).clear();    //.invert( true );    //.refresh();
//    display.draw_string( 0, 0, "ESP32-SSD1306-Driver", WHITE, BLACK ).refresh();
//    lines( &display, 200 );
//    sleep( 2 );
//    circles( &display, 10 );
    // display.clear().draw_string( 0, 32, "ESP32-SSD1306-Driver", WHITE, BLACK ).refresh();
    // circles( &display, 10 );

//    display.draw_vline( 0, 0, 32, WHITE ).refresh();
//    display.draw_char( 0, 0, '@', WHITE, BLACK );
//    display.draw_char( 80, 8, '@', WHITE, BLACK ).refresh();
    // display.draw_circle( 35, 35, 10, WHITE );
    // display.fill_circle( 80, 35, 10, WHITE ).refresh( true );
    // display.fill_circle( 80, 80, 15, WHITE ).refresh( true );

//    display.draw_char( 0, 0, '@', WHITE, BLACK ).refresh();
//    sleep( 1 );
//    display.draw_char( 10, 1, '@', WHITE, BLACK ).refresh();
//    display.draw_char( 20, 2, '@', WHITE, BLACK ).refresh();
//    display.draw_char( 30, 3, '@', WHITE, BLACK ).refresh();
//    display.draw_char( 40, 4, '@', WHITE, BLACK ).refresh();
//    display.draw_char( 50, 5, '@', WHITE, BLACK ).refresh();
//    display.draw_char( 60, 6, '@', WHITE, BLACK ).refresh();
//    display.draw_char( 70, 7, '@', WHITE, BLACK ).refresh();
//    sleep( 1 );
//    display.draw_char( 90, 9, '@', WHITE, BLACK ).refresh();

//    uint8_t y { 1 };
//    display.clear();
//    for ( int x = 3; x < 120; x += 10 )
//    {
//        display.draw_char( x, y, '@', WHITE, BLACK ).refresh();
//        sleep( 0.4 );
//        y += 5;
//    }

//	i2c64.segment( 1, 0, 0x3e, WHITE );
//	i2c64.segment( 1, 1, 0x41, WHITE );
//	i2c64.segment( 1, 2, 0x5d, WHITE );
//	i2c64.segment( 1, 3, 0x59, WHITE );
//	i2c64.segment( 1, 4, 0x4e, WHITE );

    //  display.refresh( true );
}

/*
 * Talk to the wire
 */
void rectangle( Display* display )
{
    display->clear();
    for ( uint8_t d = 0; d < 18; d++ )
    {
        display->draw_pixel( d * 8, 2 * d + 0, WHITE ).fill_rectangle( 2 + ( d * 8 ), 1, 3, 2 * d, WHITE );
        display->draw_pixel( d * 8, 2 * d + 34, WHITE ).draw_rectangle( 2 + ( d * 8 ), 35, 3, 2 * d, WHITE );
    }
    display->refresh( true );
}

/*
 * Draw some circles
 */
void circles( Display* display, int n )
{
    uint32_t rnd;
    uint8_t x0;
    uint8_t y0;
    uint8_t r0;
    for ( int i = 0; i < n; i++ )
    {
        rnd = esp_random();
        x0 = ( rnd & 0x7E );
        y0 = ( rnd & 0x7E00 ) >> 8;
        r0 = ( rnd & 0xF0000 ) >> 15;
        display->draw_circle( x0, y0, r0, WHITE ).refresh();
        sleep( 1 );
        rnd = esp_random();
        x0 = ( rnd & 0x7E );
        y0 = ( rnd & 0x7E00 ) >> 8;
        r0 = ( rnd & 0xF0000 ) >> 16;
        display->fill_circle( x0, y0, r0, INVERT ).refresh();
        sleep( 1 );
    }
}

/*
 * Draw some lines
 */
void lines( Display* display, int countdown )
{
    const uint8_t x { 35 }, y { 60 }, xx { 97 }, yy { 23 };

    display->draw_line( 85, 16, 103, 58, WHITE );
    display->draw_line( 3, 36, 14, 47, WHITE );
    display->draw_line( 20, 20, 55, 55, WHITE );
    display->draw_line( 22, 45, 115, 55, WHITE );
    display->draw_line( 0, 0, 127, 63, WHITE );
    display->draw_line( 127, 0, 0, 63, WHITE );
    display->draw_line( 40, 60, 17, 57, WHITE ).refresh();
    sleep( 2 );

    uint32_t rnd { esp_random() };

    int i { countdown }, j { std::max( static_cast< int >( rnd & 0x3 ), 1 ) },    //
            k { std::max( static_cast< int >( ( rnd & 0xc ) >> 2 ), 4 ) },    //
            l { std::max( static_cast< int >( ( rnd & 0x30 ) >> 4 ), 2 ) },    //
            m { std::min( static_cast< int >( ( rnd & 0xc0 ) >> 6 ) * -1, -2 ) };

    rnd = esp_random();

    int jj { std::max( static_cast< int >( rnd & 0x3 ), 1 ) },    //
            kk { std::max( static_cast< int >( ( rnd & 0xc ) >> 2 ), 4 ) },    //
            ll { std::max( static_cast< int >( ( rnd & 0x30 ) >> 4 ), 2 ) },    //
            mm { std::min( static_cast< int >( ( rnd & 0xc0 ) >> 6 ) * -1, -2 ) };

    uint8_t n { 35 }, o { 60 }, p { 97 }, q { 23 };
    uint8_t r { 44 }, s { 35 }, t { 66 }, u { 54 };

    display->draw_line( n, o, p, q, WHITE ).draw_line( r, s, t, u, INVERT ).refresh();
    sleep( 1 );
    display->clear( true ).refresh();
    sleep( 1 );

    while ( i-- > 0 )
    {
        display->clear( true ).draw_line( n, o, p, q, WHITE ).draw_line( r, s, t, u, INVERT ).refresh();

        n += j;
        o += k;
        p += l;
        q += m;

        if ( n <= x || n >= xx ) j = -j;

        if ( p <= x || p >= xx ) l = -l;

        if ( o <= y || o >= yy ) k = -k;

        if ( q <= y || q >= yy ) m = -m;

        r += jj;
        s += kk;
        t += ll;
        u += mm;

        if ( r <= x || r >= xx ) jj = -jj;

        if ( s <= x || s >= xx ) ll = -ll;

        if ( t <= y || t >= yy ) kk = -kk;

        if ( u <= y || u >= yy ) mm = -mm;

    }
}
