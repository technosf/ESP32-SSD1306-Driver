/*
 * SSD1306.h
 *
 *  Created on: May 9, 2019
 *      Author: technosf@github
 */
#ifndef SSD1306_H_
#define SSD1306_H_

#include <string.h>
#include <stdint.h>
#include <algorithm>

#include "PIF.h"

enum color_t
{
	TRANSPARENT = -1,    //!< Transparent (not drawing)
	BLACK = 0,        //!< Black (pixel off)
	WHITE = 1,        //!< White (or blue, yellow, pixel on)
	INVERT = 2,       //!< Invert pixel (XOR)
};

//! @brief Panel type
enum panel_type_t
{
	SSD1306_128x64 = 8,    //!< 128x32 panel, 8 pages of memory
	SSD1306_128x32 = 4    //!< 128x64 panel, 4 pages of memory
};

/*
 *
 */
class SSD1306
{
		static const uint8_t COLUMNS = 128;

	public:

		/**
		 * @brief   Constructor
		 * @param   pif     Wire protocol interface adaptor
		 * @param   scl_pin  SCL Pin
		 * @param   sda_pin  SDA Pin
		 */
		SSD1306( PIF* pif, panel_type_t type );

		/**
		 * @brief   Initialize OLED panel
		 * @return  true if successful
		 * @remark  Possible reasons for failure include non-configured panel type, out of memory or I2C not responding
		 */
		bool init();

		/**
		 * @brief   De-initialize OLED panel, turn off power and free memory
		 * @return  true if successful
		 * @remark  Possible reasons for failure include non-configured panel type, out of memory or I2C not responding
		 */
		void term();

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
		void clear();

		/**
		 * @brief   Refresh display (send display buffer to the panel)
		 */
		void refresh( bool force );

		/*
		 * @brief	Set the color of pixels in a segment
		 * @param   page   the page coord of the column
		 * @param   column   the column # in the page
		 * @param   bits   the segment bits to set
		 * @param   color   the color to set the segment bits
		 * @param   count   the number of consecutive segments in the page to draw
		 * @return  True if segment was drawn
		 */
		bool segment( uint8_t page, uint8_t column, uint8_t bits, color_t color, uint8_t count = 1 );

		/*
		 * @brief	Set the color of a single pixel
		 * @param   x   the x coord of the pixel
		 * @param   y   the y coord of the pixel
		 * @param   color   the desired color of the pixel
		 * @param	count	number of times to draw the bits
		 * @return  True if pixel was drawn
		 */
		bool pixel( uint8_t x, uint8_t y, color_t color );

		/*
		 * @brief	Draw a filled box of the given color
		 * @param   x   the starting x coord of the box
		 * @param   y   the starting y coord of the box
		 * @param   color   the color to set the box
		 * @param   w   the width of the box in pixels to draw
		 * @param   h   the height of the box in pixels
		 * @return  True if line was drawn
		 */
		bool box( uint8_t x, uint8_t y, color_t color, uint8_t w, uint8_t h );

		/*
		 * @brief	Draw a horizontal line of the given color
		 * @param   x   the starting x coord of the line
		 * @param   y   the starting y coord of the line
		 * @param   color   the color to set the segment bits
		 * @param   w   the width of the line in pixels to draw
		 * @param   h   the height of the line in pixels, defaults to 1
		 * @return  True if line was drawn
		 */
		bool horizontal( uint8_t x, uint8_t y, color_t color, uint8_t w, uint8_t h = 1 );

		/*
		 * @brief	Draw a vertical line of the given color
		 * @param   x   the starting x coord of the line
		 * @param   y   the starting y coord of the line
		 * @param   color   the color to set the segment bits
		 * @param   h   the height of the line in pixels to draw
		 * @param   w   the width of the line in pixels, defaults to 1
		 * @return  True if line was drawn
		 */
		bool vertical( uint8_t x, uint8_t y, color_t color, uint8_t h, uint8_t w = 1 );

		/**
		 * @brief   Set normal or inverted display
		 * @param   invert      Invert display?
		 */
		void invert_display( bool invert );

		/**
		 * @brief   Direct update display buffer
		 * @param   data        Data to fill display buffer, no length check is performed!
		 * @param   length      Length of data
		 */
		void update_buffer( uint8_t* data, uint16_t length );

	private:
		const uint8_t BITS [ 8 ] = { 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF };

		PIF * m_pif;	// Wire protocol adapter
		panel_type_t m_type;
		uint8_t (*m_buffer) [ COLUMNS ];    	// Display buffer - Page by Column
		uint16_t m_buffer_size;
		uint8_t m_width { COLUMNS };          			// panel width (128)
		uint8_t m_height;         	// panel height (32 or 64)
		uint16_t m_pixels;    // panel pixel count
		uint8_t m_refresh_top { 0 };    // "Dirty" window
		uint8_t m_refresh_left { 0 };
		uint8_t m_refresh_right { 0 };
		uint8_t m_refresh_bottom { 0 };
};

#endif  /* SSD1306_H */

