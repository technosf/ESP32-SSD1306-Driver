/*
 ESP32-SSD1306-Driver Library 3-Wire SPI Driver

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

#ifndef SSD1306_SPI_PIF_H_
#define SSD1306_SPI_PIF_H_

#include <stdint.h>

#include <driver/spi_master.h>
#include <driver/spi_common.h>
#include <driver/gpio.h>

#include "PIF.h"

/**
 * @brief Sets the DC pin HIGH
 *
 */
void IRAM_ATTR dc_pin_high( spi_transaction_t* trans )
{
    gpio_set_level( *(gpio_num_t*) trans->user, 1 );
}

/**
 * @brief Sets the DC pin LOW
 *
 */
void IRAM_ATTR dc_pin_low( spi_transaction_t* trans )
{
    gpio_set_level( *(gpio_num_t*) trans->user, 0 );
}

/**
 * \class SPI_PIF
 * @brief SPI Protocol Interface
 *
 */
class SPI_PIF : public PIF
{
        static const spi_host_device_t SPIHOST { HSPI_HOST };
        static const uint8_t WORDSIZE { 8 };
        static const constexpr gpio_config_t DC_PIN_CONFIG =    			//
                { .pin_bit_mask = 0x0,    						//
                        .mode = GPIO_MODE_OUTPUT,    			//
                        .pull_up_en = GPIO_PULLUP_ENABLE,    	//
                        .pull_down_en = GPIO_PULLDOWN_ENABLE,    //
                        .intr_type = GPIO_INTR_DISABLE    		//
                };
    private:

        uint8_t* m_dma_buffer = (uint8_t *) heap_caps_malloc( 128, MALLOC_CAP_DMA );    // < DMA buffer

        gpio_num_t m_dc;

        //spi_bus_config_t m_bus_config;

        spi_device_handle_t m_spi { nullptr };    		// Default SPI device handle for 3 & 4 wire, except 4 wire data
        spi_device_handle_t m_spi_4wire_data { nullptr };    // SPI device for 4 wire data

        //	spi_device_interface_config_t m_device;

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

        /**
         * @brief Creates an SPI interface on the given pins
         * @param mosi
         * @param clk
         * @param cs
         * @param dc
         */
        SPI_PIF( gpio_num_t mosi, gpio_num_t clk, gpio_num_t cs, gpio_num_t dc = GPIO_NUM_NC )
        {
            m_dc = dc;

            ::spicommon_periph_free( SPIHOST );    // Claim HSPI peripheral

            /*
             * SPI Bus
             */
            spi_bus_config_t m_bus_config;
            m_bus_config.miso_io_num = -1;
            m_bus_config.mosi_io_num = mosi;
            m_bus_config.sclk_io_num = clk;
            m_bus_config.quadwp_io_num = -1;
            m_bus_config.quadhd_io_num = -1;
            m_bus_config.max_transfer_sz = WORDSIZE * 128;
            m_bus_config.flags = SPICOMMON_BUSFLAG_MASTER | SPICOMMON_BUSFLAG_MOSI | SPICOMMON_BUSFLAG_SCLK;
            m_bus_config.intr_flags = 0;
            ESP_ERROR_CHECK( ::spi_bus_initialize( SPIHOST, &m_bus_config, 2 ) );    // Select 2nd DMA channel

            /*
             * Device interface
             */
            spi_device_interface_config_t m_device;
            m_device.address_bits = 0;
            m_device.dummy_bits = 0;    //note this; if dropping bits at start of cycle
            m_device.mode = 0;    //0,1,2,3.  has to do with clock polarity, phase, and edge.  See wikipedia.org for spi mode numbers
            m_device.duty_cycle_pos = 0;    // setting to 0 defaults to 128, 50% duty cycle
            m_device.cs_ena_pretrans = 0;    //amount of spi bit-cycles cs activated before transmission.
            m_device.cs_ena_posttrans = 0;    //sim to pretrans, but trailing
            m_device.clock_speed_hz = 100000;    //10 khz; clock speed in hz
            m_device.spics_io_num = cs;    //CS GPIO pin, -1 if not used
            m_device.flags = SPI_DEVICE_HALFDUPLEX;    //spi_device_ flags
            m_device.queue_size = 8;    //queue depth.  0 will cause crash

            if ( dc == GPIO_NUM_NC )
            /*
             * 3-Wire I/f
             */
            {
                m_device.command_bits = 1;
                m_device.pre_cb = nullptr;
                m_device.post_cb = nullptr;
                ESP_ERROR_CHECK( ::spi_bus_add_device( SPIHOST, &m_device, &m_spi ) );
            }
            else
            /*
             * 4-wire i/f
             */
            {
                m_device.command_bits = 0;
                ESP_ERROR_CHECK( ::spi_bus_add_device( SPIHOST, &m_device, &m_spi ) );
                m_device.pre_cb = dc_pin_high;    //callback before trans - set d/c high
                m_device.post_cb = dc_pin_low;    //callback after trans set d/c low
                ESP_ERROR_CHECK( ::spi_bus_add_device( SPIHOST, &m_device, &m_spi_4wire_data ) );
            }

            /*
             * Transaction
             */
            m_transaction.cmd = 0;
            m_transaction.addr = 0;
            m_transaction.flags = 0;
            m_transaction.length = 0;
            m_transaction.rxlength = 0;
            m_transaction.user = &m_dc;
            m_transaction.tx_buffer = m_dma_buffer;    //docs specify NULL for no MOSI phase.
            m_transaction.rx_buffer = NULL;

        }

        virtual ~SPI_PIF()
        {
        }

        /**
         * @brief Transmit a single command byte
         * @param cmd Command to transmit
         */
        void command( uint8_t cmd )
        {
            m_transaction.flags = SPI_TRANS_USE_TXDATA;
            m_transaction.cmd = 0x0;
            m_transaction.length = WORDSIZE;
            memset( m_transaction.tx_data, 0, sizeof ( m_transaction.tx_data ) );
            m_transaction.tx_data [ 0 ] = cmd;
            spi_device_transmit( m_spi, &m_transaction );
        }

        /**
         * @brief Transmit commands to the device
         * @param cmd pointer to the commands
         * @param size the amount of command data in bytes
         */
        void command( uint8_t* const cmd, uint8_t size )
        {
            if ( size == 0 )
            {
                size = sizeof ( cmd );
            }

            printf( "SzC: %d\n", size );
            m_transaction.flags = 0;
            m_transaction.cmd = 0x0;
            m_transaction.length = WORDSIZE * size;
            memcpy( m_dma_buffer, cmd, size );
            spi_device_transmit( m_spi, &m_transaction );
        }

        /**
         * @brief Transmit data to the device
         * @param data pointer to the data
         * @param size the amount of data in bytes
         */
        void data( uint8_t* const data, uint8_t size )
        {
            m_transaction.flags = 0;
            m_transaction.length = WORDSIZE * size;
            memcpy( m_dma_buffer, data, size );

            if ( m_dc == GPIO_NUM_NC )
            /*
             * 3-Wire, set data bit
             */
            {
                m_transaction.cmd = 0x1;
                spi_device_transmit( m_spi, &m_transaction );
            }
            else
            /*
             * 4-Wire, use 4-wire data device
             */
            {
                spi_device_transmit( m_spi_4wire_data, &m_transaction );
            }
        }

        /**
         *
         */
        virtual void info()
        {
            printf( "\nSPI Bus Info\n" );
        }    // info

};
#endif  // SSD1306_SPI_PIF_H_
