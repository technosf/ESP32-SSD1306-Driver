/*
 * SPI-PIF.h
 *
 *  Created on: May 9, 2019
 *      Author: technosf@github
 */

#ifndef SSD1306_SPI_PIF_H_
#define SSD1306_SPI_PIF_H_

#include <stdint.h>

#include <driver/spi_master.h>
#include <driver/spi_common.h>

#include "PIF.h"

class SPI_PIF : public PIF
{
		static const spi_host_device_t SPIHOST { HSPI_HOST };
		static const uint8_t WORDSIZE { 9 };

	private:

		//gpio_num_t m_dc;

		spi_bus_config_t m_bus_config;

		spi_device_handle_t m_spi { nullptr };

		spi_device_interface_config_t m_device;

		spi_transaction_t m_transaction;

		//	uint8_t m_data [ 256 ];    //16 bit

//		void write( uint8_t ctrl, uint8_t data )
//		{
//			spi_device_transmit( m_spi, &m_transaction );
//		}
//
//		void write( uint8_t ctrl, uint8_t* data, uint8_t size )
//		{
//			spi_device_transmit( m_spi, &m_transaction );
//		}

	public:

		//SPI_PIF( gpio_num_t miso, gpio_num_t mosi, gpio_num_t clk, gpio_num_t cs, gpio_num_t dc ) :
		SPI_PIF( gpio_num_t mosi, gpio_num_t clk, gpio_num_t cs )
		{
//			m_dc
//			{	dc};

			m_bus_config.miso_io_num = -1;
			m_bus_config.mosi_io_num = mosi;
			m_bus_config.sclk_io_num = clk;
			m_bus_config.quadwp_io_num = -1;
			m_bus_config.quadhd_io_num = -1;
			m_bus_config.max_transfer_sz = WORDSIZE * 128;
			m_bus_config.flags = SPICOMMON_BUSFLAG_MASTER | SPICOMMON_BUSFLAG_MOSI | SPICOMMON_BUSFLAG_SCLK;
			m_bus_config.intr_flags = 0;

			m_device.command_bits = 1;
			m_device.address_bits = 0;
			m_device.dummy_bits = 0;    //note this; if dropping bits at start of cycle
			m_device.mode = 0;    //0,1,2,3.  has to do with clock polarity, phase, and edge.  See wikipedia.org for spi mode numbers
			m_device.duty_cycle_pos = 0;    // setting to 0 defaults to 128, 50% duty cycle
			m_device.cs_ena_pretrans = 0;    //amount of spi bit-cycles cs activated before transmission.
			m_device.cs_ena_posttrans = 0;    //sim to pretrans, but trailing
			m_device.clock_speed_hz = 100000;    //10 khz; clock speed in hz
			m_device.spics_io_num = cs;    //CS GPIO pin, -1 if not used
			m_device.flags = SPI_DEVICE_HALFDUPLEX;    //spi_device_ flags
			m_device.queue_size = 2;    //queue depth.  0 will cause crash
			m_device.pre_cb = nullptr;    //callback before trans ========not sure here
			m_device.post_cb = nullptr;    //callback after trans ========not sure here

			m_transaction.cmd = 0;
			m_transaction.addr = 0;
			m_transaction.flags = 0;
			m_transaction.length = 0;
			m_transaction.rxlength = 0;
			m_transaction.tx_buffer = NULL;    //docs specify NULL for no MOSI phase.
			m_transaction.rx_buffer = NULL;

			::spicommon_periph_free( SPIHOST );    // Claim HSPI peripheral
			//ESP_LOGI( TAG, "SPI in use?: %d", ::spicommon_periph_in_use( SPIHOST ) );
			//::spicommon_periph_claim( SPIHOST, TAG );		// Claim HSPI peripheral

			ESP_ERROR_CHECK( ::spi_bus_initialize( SPIHOST, &m_bus_config, 0 ) );

			//Attach the MAX6675 to the SPI bus, get the handle
			ESP_ERROR_CHECK( ::spi_bus_add_device( SPIHOST, &m_device, &m_spi ) );

		}

		virtual ~SPI_PIF()
		{
		}

		void command( uint8_t cmd )
		{
			m_transaction.flags = SPI_TRANS_USE_TXDATA;
			m_transaction.cmd = 0x0;
			m_transaction.length = WORDSIZE;
			memset( m_transaction.tx_data, 0, sizeof ( m_transaction.tx_data ) );
			m_transaction.tx_data [ 0 ] = cmd;
			spi_device_transmit( m_spi, &m_transaction );
		}

		void command( uint8_t* cmd, uint8_t size )
		{
			if ( size == 0 )
			{
				size = sizeof ( cmd );
			}

//			printf( "SzC: %d\n", 1 );
//			m_transaction.flags = SPI_TRANS_USE_TXDATA;
//			m_transaction.cmd = 0x0;
//			m_transaction.length = WORDSIZE;
//			memset( m_transaction.tx_data, 0, sizeof ( m_transaction.tx_data ) );
//			spi_device_transmit( m_spi, &m_transaction );

			printf( "SzC: %d\n", size );
			m_transaction.flags = 0;
			m_transaction.cmd = 0x0;
			m_transaction.length = WORDSIZE * size;
			m_transaction.tx_buffer = cmd;
			spi_device_transmit( m_spi, &m_transaction );
		}

		void data( uint8_t* data, uint8_t size )
		{
//			printf( "SzD: %d\n", 1 );
//			m_transaction.flags = SPI_TRANS_USE_TXDATA;
//			m_transaction.cmd = 0x1;
//			m_transaction.length = WORDSIZE;
//			memset( m_transaction.tx_data, 0, sizeof ( m_transaction.tx_data ) );
//			m_transaction.tx_data [ 0 ] = 0x40;
//			spi_device_transmit( m_spi, &m_transaction );

			printf( "SzD: %d\n", size );
			m_transaction.flags = 0;
			m_transaction.cmd = 0x1;
			m_transaction.length = WORDSIZE * size;
			m_transaction.tx_buffer = data;
			spi_device_transmit( m_spi, &m_transaction );
		}

		virtual void info()
		{
			printf( "\nSPI Bus Scan\n" );
//			uint8_t address;
//			printf( "     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\r\n" );
//			for ( int i = 0; i < 128; i += 16 )
//			{
//				printf( "%02x: ", i );
//				for ( int j = 0; j < 16; j++ )
//				{
//					fflush( stdout );
//					address = i + j;
//					i2c_cmd_handle_t cmd = i2c_cmd_link_create();
//					i2c_master_start( cmd );
//					i2c_master_write_byte( cmd, ( address << 1 ) | I2C_MASTER_WRITE, 1 );
//					i2c_master_stop( cmd );
//					esp_err_t ret = i2c_master_cmd_begin( i2c_master_port, cmd, 50 / portTICK_RATE_MS );
//					i2c_cmd_link_delete( cmd );
//					if ( ret == ESP_OK )
//					{
//						printf( "%02x ", address );
//					}
//					else if ( ret == ESP_ERR_TIMEOUT )
//					{
//						printf( "UU " );
//					}
//					else
//					{
//						printf( "-- " );
//					}
//				}
//				printf( "\r\n" );
//			}
//			printf( "\n\n" );

		}    // info
};

#endif  // SSD1306_SPI_PIF_H_
