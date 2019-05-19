/*
 * OLED.cpp
 *
 *  Created on: May 9, 2019
 *      Author: technosf@github
 */

#include <stdio.h>

//#include <algorithm>
//using std::min;

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

OLED &OLED::clear()
{
	m_ssd1306.clear();
	return *this;
}

OLED &OLED::refresh( bool force )
{
	m_ssd1306.refresh( force );
	return *this;
}

OLED &OLED::invert( bool invert )
{
	m_ssd1306.invert_display( invert );
	return *this;
}

OLED &OLED::draw_pixel( uint8_t x, uint8_t y, color_t color )
{
	m_ssd1306.pixel( x, y, color );
	return *this;
}

OLED &OLED::draw_hline( uint8_t x, uint8_t y, uint8_t w, color_t color )
{
	if ( ( w == 0 ) || ( x >= width() ) || ( y >= height() ) ) return *this;

	m_ssd1306.horizontal( x, y, color, w );

	return *this;
}

OLED &OLED::draw_vline( uint8_t x, uint8_t y, uint8_t h, color_t color )
{
	if ( ( h == 0 ) || ( x >= width() ) || ( y >= height() ) ) return *this;

	m_ssd1306.vertical( x, y, color, h );

	return *this;
}

OLED &OLED::draw_rectangle( uint8_t x, uint8_t y, uint8_t w, uint8_t h, color_t color )
{
	if ( ( w == 0 ) || ( h == 0 ) ) return *this;
	draw_vline( x, y, h, color );
	draw_vline( x + w, y, h, color );
	draw_hline( x + 1, y, w - 1, color );
	draw_hline( x + 1, y + h - 1, w - 1, color );
	return *this;
}

OLED &OLED::fill_rectangle( uint8_t x, uint8_t y, uint8_t w, uint8_t h, color_t color )
{
	if ( ( w == 0 ) || ( h == 0 ) || ( x >= width() ) || ( y >= height() ) ) return *this;

	m_ssd1306.box( x, y, color, w, h );

	return *this;
}

OLED &OLED::draw_circle( uint8_t x0, uint8_t y0, uint8_t r, color_t color )
{
// Refer to http://en.wikipedia.org/wiki/Midpoint_circle_algorithm for the algorithm

	int8_t x = r;
	int8_t y = 1;
	int16_t radius_err = 1 - x;

	if ( r == 0 ) return *this;;

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

OLED &OLED::fill_circle( uint8_t x0, uint8_t y0, uint8_t r, color_t color )
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

OLED &OLED::select_font( uint8_t idx )
{
	if ( idx < NUM_FONTS ) font = fonts [ idx ];
	return *this;
}

/*
 * Slices characters into segments rather than decompose as individual pixels
 * Requires L-R scanning bitmaps to be remapped into T-B segment bitmaps
 */
OLED &OLED::draw_char( uint8_t x, uint8_t y, unsigned char c, color_t foreground, color_t background,
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

	uint8_t page_start = y / 8;
	uint8_t page_end = ( y + font->height ) / 8;

	uint8_t segments [ font->char_descriptors [ c ].width ] [ 1 + page_end - page_start ] = {};    // L-R, T-B
	printf( "Seg width:%d  pages:%d  zz:%d\n", font->char_descriptors [ c ].width, ( 1 + page_end - page_start ),
			segments [ 0 ] [ 0 ] );

	const uint8_t * bitmap = font->bitmap + font->char_descriptors [ c ].offset;
	uint8_t scan_length = 1 + ( font->char_descriptors [ c ].width / 8 );
	uint8_t scan;

	for ( uint8_t scan_line = 0; scan_line < font->height; scan_line++ )
	/*
	 * Scan the char line by line, T-B
	 */
	{
		uint8_t page = page_start + ( scan_line / 8 );
		uint8_t vertical_shift = ( scan_line + y ) % 8;

		for ( uint8_t scan_byte = 0; scan_byte < scan_length; scan_byte++ )
		/*
		 * Loop through bytes in a scan line
		 */
		{
			scan = * ( bitmap + ( scan_line * scan_length ) + scan_byte );    // Scan line

			printf( "VShift %d  Char loc 0x%04x  byte 0x%02x\n", vertical_shift,
					( scan_line * scan_length ) + scan_byte, scan );

			for ( uint8_t col = 8 * scan_byte; col < font->char_descriptors [ c ].width; col++ )
			/*
			 * Pick out the bits and OR them into the segment
			 */
			{
				uint8_t bit = col % 8;
				uint8_t bitmask = 0x80 >> bit;

				if ( ( bit == 0 ) && ( col > 0 ) ) break;    // Byte boundary, breakout to incr scan byte

				bool scanbit = scan & bitmask;

				printf( "Col %d  Scan bit 0x%02x  Shifted Scan bit 0x%02x  old Segs 0x%02x ", col, scanbit,
						scanbit << vertical_shift, segments [ col ] [ page ] );

				segments [ col ] [ page ] |= ( scanbit << vertical_shift );    // Segment bit-or with scan

				printf( " New Segs 0x%02x\n", segments [ col ] [ page ] );
			}
		}
	}

	// Push out the remapped character
	for ( uint8_t page = page_start; page <= page_end; page++ )
	{
		for ( uint8_t col = 0; col < font->char_descriptors [ c ].width; col++ )
		{
			m_ssd1306.segment( page, 20 + x + col, segments [ col ] [ page ], foreground );
		}    // for
	}    // for

	return *this;
}    // draw_char

OLED &OLED::draw_string( uint8_t x, uint8_t y, std::string str, color_t foreground, color_t background,
		uint8_t* outwidth )
{
	if ( font == NULL || str.empty() )
	{
		if ( outwidth != nullptr ) *outwidth = 0;
		return *this;
	}

	uint8_t t { x };
	for ( char& c : str )
	{
		uint8_t _x { 0 };
		draw_char( t, y, c, foreground, background, &_x );
		t += ( _x + font->c + 1 );
	}

	if ( outwidth != nullptr ) *outwidth = ( t - x );
	return *this;
}

uint8_t OLED::measure_string( std::string str )
{
	uint8_t w = 0;
	unsigned char c;

	if ( font == NULL || str.empty() ) return 0;

	for ( std::string::iterator i = str.begin(); i < str.end(); i++ )
	{
		c = *i;
// we always have space in the font set
		if ( ( c < font->char_start ) || ( c > font->char_end ) ) c = ' ';
		c = c - font->char_start;    // c now become index to tables
		w += font->char_descriptors [ c ].width;
		if ( *i ) w += font->c;
	}

	return w;
}

uint8_t OLED::get_font_height()
{
	if ( font == nullptr ) return 0;

	return ( font->height );
}

uint8_t OLED::get_font_c()
{
	if ( font == NULL ) return 0;

	return ( font->c );
}

