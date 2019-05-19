/*
 * display.h
 *
 *  Created on: May 9, 2019
 *      Author: technosf@github
 */

#ifndef SSD1306_OLED_H_
#define SSD1306_OLED_H_

#include <string>
#include <stdint.h>
#include <algorithm>
using std::min;

#include <oled_fonts.h>

#include "SSD1306.h"

/*
 *
 */
class OLED
{
	private:
		SSD1306& m_ssd1306;

		const font_info_t* font { NULL };    // current font

	public:

		OLED( SSD1306& ssd1306 );

		virtual ~OLED()
		{
		}

		/**
		 * @brief   Return OLED panel height
		 * @return  Panel height, or return 0 if failed (panel not initialized)
		 */
		uint8_t width();

		/**
		 * @brief   Return OLED panel height
		 * @return  Panel height, or return 0 if failed (panel not initialized)
		 */
		uint8_t height();

		/**
		 * @brief   Clear display buffer (fill with black)
		 */
		OLED &clear();

		/**
		 * @brief   Refresh display (send display buffer to the panel)
		 * @param   force   The program automatically tracks "dirty" region to minimize refresh area. Set #force to true
		 *                  ignores the dirty region and refresh the whole screen.
		 */
		OLED &refresh( bool force = false );

		/**
		 * @brief   Set normal or inverted display
		 * @param   invert      Invert display?
		 */
		OLED &invert( bool invert );

		/**
		 * @brief   Draw one pixel
		 * @param   x       X coordinate
		 * @param   y       Y coordinate
		 * @param   color   Color of the pixel
		 */
		OLED &draw_pixel( uint8_t x, uint8_t y, color_t color );

		/**
		 * @brief   Draw horizontal line
		 * @param   x       X coordinate or starting (left) point
		 * @param   y       Y coordinate or starting (left) point
		 * @param   w       Line width
		 * @param   color   Color of the line
		 */
		OLED &draw_hline( uint8_t x, uint8_t y, uint8_t w, color_t color );

		/**
		 * @brief   Draw vertical line
		 * @param   x       X coordinate or starting (top) point
		 * @param   y       Y coordinate or starting (top) point
		 * @param   h       Line height
		 * @param   color   Color of the line
		 */
		OLED &draw_vline( uint8_t x, uint8_t y, uint8_t h, color_t color );
		/**
		 * @brief   Draw a rectangle
		 * @param   x       X coordinate or starting (top left) point
		 * @param   y       Y coordinate or starting (top left) point
		 * @param   w       Rectangle width
		 * @param   h       Rectangle height
		 * @param   color   Color of the rectangle border
		 */
		OLED &draw_rectangle( uint8_t x, uint8_t y, uint8_t w, uint8_t h, color_t color );

		/**
		 * @brief   Draw a filled rectangle
		 * @param   x       X coordinate or starting (top left) point
		 * @param   y       Y coordinate or starting (top left) point
		 * @param   w       Rectangle width
		 * @param   h       Rectangle height
		 * @param   color   Color of the rectangle
		 */
		OLED &fill_rectangle( uint8_t x, uint8_t y, uint8_t w, uint8_t h, color_t color );

		/**
		 * @brief   Draw a filled circle
		 * @param   x0      X coordinate or center
		 * @param   y0      Y coordinate or center
		 * @param   r       Radius
		 * @param   color   Color of the circle
		 */
		OLED &draw_circle( uint8_t x0, uint8_t y0, uint8_t r, color_t color );

		/**
		 * @brief   Draw a filled circle
		 * @param   x0      X coordinate or center
		 * @param   y0      Y coordinate or center
		 * @param   r       Radius
		 * @param   color   Color of the circle
		 */
		OLED &fill_circle( uint8_t x0, uint8_t y0, uint8_t r, color_t color );

		/**
		 * @brief   Select font for drawing
		 * @param   idx     Font index, see fonts.c
		 */
		OLED &select_font( uint8_t idx );
		/**
		 * @brief   Draw one character using currently selected font
		 * @param   x           X position of character (top-left corner)
		 * @param   y           Y position of character (top-left corner)
		 * @param   c           The character to draw
		 * @param   foreground  Character color
		 * @param   background  Background color
		 * @return  Width of the character
		 */
		OLED &draw_char( uint8_t x, uint8_t y, unsigned char c, color_t foreground, color_t background,
				uint8_t* outwidth = nullptr );

		/**
		 * @brief   Draw string using currently selected font
		 * @param   x           X position of string (top-left corner)
		 * @param   y           Y position of string (top-left corner)
		 * @param   str         The string to draw
		 * @param   foreground  Character color
		 * @param   background  Background color
		 * @return  Width of the string (out-of-display pixels also included)
		 */
		OLED &draw_string( uint8_t x, uint8_t y, std::string str, color_t foreground, color_t background,
				uint8_t* outwidth = nullptr );

		/**
		 * @brief   Measure width of string with current selected font
		 * @param   str         String to measure
		 * @return  Width of the string
		 */
		uint8_t measure_string( std::string str );
		/**
		 * @brief   Get the height of current selected font
		 * @return  Height of the font (in pixels) or 0 if none font selected
		 */
		uint8_t get_font_height();

		/**
		 * @brief   Get the "C" value (space between adjacent characters)
		 * @return  "C" value
		 */
		uint8_t get_font_c();

};

#endif  /* SSD1306_OLED_H_ */

