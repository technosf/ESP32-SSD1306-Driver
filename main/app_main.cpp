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
#define __spi

#include <driver/gpio.h>
#include <math.h>

#include "SSD1306.h"
#include <OLED.h>

#include "SPI_PIF.h"
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

    SPI_PIF spi { GPIO_NUM_13, GPIO_NUM_14, GPIO_NUM_15, GPIO_NUM_2 };
    SSD1306 p( &spi, SSD1306_128x64 );
    // p.init();
    // p.invert_display( true );
    OLED d = OLED( p );
    d.draw_char( 80, 8, '@', WHITE, BLACK ).refresh();
    p.invert_display( true );
    d.refresh( true );

    //SSD1306 ssd1306( pif, SSD1306_128x64 );

//	i2c64.init();
//	i2c64.refresh( true );
    //SSD1306 i2c64( &spi, SSD1306_128x64 );
    //i2c64.init();

//	uint8_t* data = new uint8_t [ 8 ];
//	for ( int i = 0; i < 8; i++ )
//	{
//		memset( data, 0, 8 );
//		data [ i ] = 0xFF;
//		i2c64.update_buffer( data, 8 );
//		i2c64.refresh();
//		sleep( 1 );
//	}
//
//	for ( int i = 0; i < 8; i++ )
//	{
//		memset( data, pow( 2.0, i ), 8 );
//		i2c64.update_buffer( data, 8 );
//		i2c64.refresh();
//		sleep( 1 );
//	}

//	OLED display = OLED( ssd1306 );
//	display.select_font( 0 ).clear();    //.invert( true );    //.refresh();
//	display.draw_char( 80, 8, '@', WHITE, BLACK ).refresh();

//	display.draw_char( 0, 0, '@', WHITE, BLACK ).refresh();
//	display.draw_char( 10, 1, '@', WHITE, BLACK ).refresh();
//	display.draw_char( 20, 2, '@', WHITE, BLACK ).refresh();
//	display.draw_char( 30, 3, '@', WHITE, BLACK ).refresh();
//	display.draw_char( 40, 4, '@', WHITE, BLACK ).refresh();
//	display.draw_char( 50, 5, '@', WHITE, BLACK ).refresh();
//	display.draw_char( 60, 6, '@', WHITE, BLACK ).refresh();
//	display.draw_char( 70, 7, '@', WHITE, BLACK ).refresh();
//	display.draw_char( 90, 9, '@', WHITE, BLACK ).refresh();

//	uint8_t y { 1 };
//	for ( int x = 3; x < 120; x += 10 )
//	{
//		//display.clear();
//		display.draw_char( x, y, '@', WHITE, BLACK ).refresh();
//		//sleep( 0.4 );
//		y += 5;
//	}

//	i2c64.segment( 1, 0, 0x3e, WHITE );
//	i2c64.segment( 1, 1, 0x41, WHITE );
//	i2c64.segment( 1, 2, 0x5d, WHITE );
//	i2c64.segment( 1, 3, 0x59, WHITE );
//	i2c64.segment( 1, 4, 0x4e, WHITE );

    //display.draw_hline( 0, 0, 4, WHITE ).refresh();
    //display.draw_pixel( 0, 0, WHITE );    //.draw_hline( 0, 32, 128, WHITE ).refresh();
    //display.draw_hline( 0, 15, 128, WHITE );    //.draw_hline( 0, 32, 128, WHITE ).refresh();
    //display.draw_hline( 0, 0, 128, WHITE ).draw_hline( 0, 15, 128, WHITE ).draw_hline( 0, 16, 128, WHITE ).refresh();
    //display.draw_hline( 0, 15, 128, WHITE ).draw_hline( 0, 16, 128, WHITE ).refresh();
    //printf( "String: %d\n", display.select_font( 0 ).measure_string( "SSD1306 Driver" ) );
    //printf( "String: 1\n" );
//	display.draw_char( 0, 0, '@', WHITE, BLACK );
//	display.draw_char( 0, 8, '@', WHITE, BLACK );
    //display.draw_string( 6, 8, "@", WHITE, BLACK );
    //display.draw_string( 50, 0, "@", WHITE, BLACK );
//	display.draw_string( 61, 0, "@", WHITE, BLACK );
    //display.draw_string( 70, 0, "@", WHITE, BLACK );
    //display.draw_string( 0, 0, "@@@", WHITE, BLACK );
    //display.draw_string( 6, 0, "@@", WHITE, BLACK ).refresh();
//	display.draw_string( 0, 16, "@", WHITE, BLACK ).refresh();
//	display.draw_string( 0, 24, "@@", WHITE, BLACK ).refresh();
//	display.draw_string( 0, 36, "@@@", WHITE, BLACK ).refresh();
    //display.draw_string( 24, 24, "SSD1306 Driver", WHITE, BLACK ).refresh();

//	display.draw_pixel( 0, 0, WHITE ).draw_pixel( 0, 6, WHITE ).draw_rectangle( 2, 0, 4, 0, WHITE );
//	display.draw_pixel( 0, 0, WHITE ).draw_pixel( 0, 6, WHITE ).draw_rectangle( 8, 0, 4, 1, WHITE );
    //display.draw_rectangle( 18, 3, 80, 55, WHITE );
//	display.draw_pixel( 10, 7, WHITE ).draw_rectangle( 12, 0, 4, 8, WHITE );
//	display.draw_pixel( 20, 8, WHITE ).draw_rectangle( 22, 0, 4, 9, WHITE );
//	for ( uint8_t d = 5; d < 18; d++ )
//	{
//		display.draw_pixel( d * 8, 2 * d + 0, WHITE ).draw_rectangle( 2 + ( d * 8 ), 1, 3, 2 * d, WHITE );
//		display.draw_pixel( d * 8, 2 * d + 34, WHITE ).draw_rectangle( 2 + ( d * 8 ), 35, 3, 2 * d, WHITE );
//	}
//	display.draw_rectangle( 40, 41, 10, 14, WHITE ).draw_pixel( 38, 54, WHITE );
//	display.draw_rectangle( 60, 42, 10, 18, WHITE ).draw_pixel( 58, 59, WHITE );
//	display.draw_hline( 25, 41, 10, WHITE ).draw_pixel( 38, 41, WHITE );
//	display.draw_hline( 25, 51, 10, WHITE ).draw_pixel( 38, 51, WHITE );
    //display.draw_circle( 50, 50, 15, WHITE ).refresh();
    //display.fill_circle( 50, 50, 15, WHITE ).refresh();
    //display.draw_vline( 64, 0, 64, WHITE ).refresh( true );    //.draw_vline( 0, 0, 64, WHITE ).refresh();

    //display.refresh( true );
}

