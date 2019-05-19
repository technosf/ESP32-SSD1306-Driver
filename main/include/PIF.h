/*
 * PIF.h
 *
 *  Created on: May 9, 2019
 *      Author: technosf@github
 */

#ifndef SSD1306_PIF_H_
#define SSD1306_PIF_H_

/*
 * Protocol Interface
 *
 * Abstract SSD1306-to-wire-protocol interface specification
 *
 */
class PIF
{
	public:

		virtual ~PIF()
		{
		}

		/*
		 * Prints out information on the particular connected protocol bus
		 */
		virtual void info() = 0;

		/*
		 * Sends a SSD1306 command over the wire protocol to the SSD1306 from the ESP32
		 */
		virtual void command( uint8_t cmd ) = 0;

		/*
		 * Sends SSD1306 commands over the wire protocol to the SSD1306 from the ESP32
		 */
		virtual void command( uint8_t* cmd, uint8_t size ) = 0;

		/*
		 * Sends SSD1306 data over the wire protocol to the SSD1306 from the ESP32
		 */
		virtual void data( uint8_t* data, uint8_t size ) = 0;
};

#endif // SSD1306_PIF_H_
