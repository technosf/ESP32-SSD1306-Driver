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
#include <esp_log.h>

#include "PIF.h"

/**
 * @brief Sets the DC pin HIGH
 *
 * If the transaction.user field has been set, set the DC pin HIGH
 *
 */
void static IRAM_ATTR dc_pin_high( spi_transaction_t* trans )
{
    ESP_LOGI( "SPI", "DC High Check" );
//    if ( trans->user == nullptr ) return;
//    ESP_LOGI( "SPI", "DC High " );
//    printf( "DC High" );
//    gpio_set_level( *(gpio_num_t*) trans->user, 1 );
//    ESP_LOGI( "SPI", "DC High High" );
}

/**
 * @brief Sets the DC pin LOW
 *
 * If the transaction.user field has been set, set the DC pin LOW
 *
 */
void static IRAM_ATTR dc_pin_low( spi_transaction_t* trans )
{
    printf( "DC Low Check" );
//    if ( trans->user == nullptr ) return;
//    printf( "DC Low" );
//    gpio_set_level( *(gpio_num_t*) trans->user, 0 );
//    printf( "DC LowLow" );
}

/**
 * \class SPI_PIF
 * @brief SPI Protocol Interface
 *
 * SPI wire protocol interface for both 3 & 4 wire SPI. If DC parameter is set, then 4-wire SPI is used, otherwise, 3-wire SPI.
 * 4-wire requires that a DC pin be pulled high when a Data is transmitted: This is accomplished through pre and post callbacks.
 * The callbacks check the transaction user field to determine if the pin should be toggled. The user field is per call per
 * protocol and data or command.
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

        uint8_t* const m_dma_buffer = (uint8_t *) heap_caps_malloc( 128, MALLOC_CAP_DMA );    // < DMA buffer

        gpio_num_t m_mosi, m_clk, m_cs, m_dc;    /// < dc is 4-wire SPI DC pin definition

        spi_device_handle_t m_spi { nullptr };    		/// < SPI device handle

        spi_transaction_t m_transaction;    /// < SPI transaction definition

        bool m_command_bits { false };

    public:

        /**
         * @brief Creates an SPI interface on the given pins
         * @param mosi
         * @param clk
         * @param cs
         * @param dc
         */
        SPI_PIF( gpio_num_t mosi, gpio_num_t clk, gpio_num_t cs, gpio_num_t dc = GPIO_NUM_NC ) :
                m_mosi { mosi }, m_clk { clk }, m_cs { cs }, m_dc { dc }
        {
            ESP_ERROR_CHECK( ::spicommon_periph_free( SPIHOST ) );    // Claim HSPI peripheral

            /*
             * SPI Bus
             */
            spi_bus_config_t bus_config;
            bus_config.miso_io_num = -1;
            bus_config.mosi_io_num = m_mosi;
            bus_config.sclk_io_num = m_clk;
            bus_config.quadwp_io_num = -1;
            bus_config.quadhd_io_num = -1;
            bus_config.max_transfer_sz = WORDSIZE * 128;
            bus_config.flags = SPICOMMON_BUSFLAG_MASTER | SPICOMMON_BUSFLAG_MOSI | SPICOMMON_BUSFLAG_SCLK;
            bus_config.intr_flags = 0;
            ESP_ERROR_CHECK( ::spi_bus_initialize( SPIHOST, &bus_config, 2 ) );    // Select 2nd DMA channel

            /*
             * Device interface
             */
            spi_device_interface_config_t device;
            device.address_bits = 0;
            device.dummy_bits = 0;    //note this; if dropping bits at start of cycle
            device.mode = 0;    //0,1,2,3.  has to do with clock polarity, phase, and edge.  See wikipedia.org for spi mode numbers
            device.duty_cycle_pos = 0;    // setting to 0 defaults to 128, 50% duty cycle
            device.cs_ena_pretrans = 0;    //amount of spi bit-cycles cs activated before transmission.
            device.cs_ena_posttrans = 0;    //sim to pretrans, but trailing
            device.clock_speed_hz = 100000;    //10 khz; clock speed in hz
            device.spics_io_num = m_cs;    //CS GPIO pin, -1 if not used
            device.flags = SPI_DEVICE_HALFDUPLEX;    //spi_device_ flags
            device.queue_size = 8;    //queue depth.  0 will cause crash

            if ( dc == GPIO_NUM_NC )
            /*
             * 3-Wire I/f
             */
            {
                device.command_bits = 1;
                device.pre_cb = nullptr;
                device.post_cb = nullptr;
            }
            else
            /*
             * 4-wire i/f
             */
            {
                // Set I/O
                gpio_config_t gpio_config = { .pin_bit_mask = ( 1ULL << dc ), /*!< GPIO pin: set with bit mask, each bit maps to a GPIO */
                .mode = GPIO_MODE_OUTPUT, /*!< GPIO mode: set input/output mode                     */
                .pull_up_en = GPIO_PULLUP_ENABLE, /*!< GPIO pull-up                                         */
                .pull_down_en = GPIO_PULLDOWN_ENABLE, /*!< GPIO pull-down                                       */
                .intr_type = GPIO_INTR_DISABLE, };
                ESP_ERROR_CHECK( ::gpio_config( &gpio_config ) );

                // Config device
                device.command_bits = 0;
                device.pre_cb = dc_pin_high;    //callback before trans - set d/c high if flag set
                device.post_cb = dc_pin_low;    //callback after trans set d/c low if flag unset
            }
            ESP_ERROR_CHECK( ::spi_bus_add_device( SPIHOST, &device, &m_spi ) );

            /*
             * Transaction
             */
            m_transaction.cmd = 0;
            m_transaction.addr = 0;
            m_transaction.flags = 0;
            m_transaction.length = 0;
            m_transaction.rxlength = 0;
            m_transaction.user = nullptr;
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
            m_transaction.user = nullptr;
            memset( m_transaction.tx_data, 0, sizeof ( m_transaction.tx_data ) );
            m_transaction.tx_data [ 0 ] = cmd;
            ESP_ERROR_CHECK( ::spi_device_transmit( m_spi, &m_transaction ) );
        }

        /**
         * @brief Transmit commands to the device
         * @param cmd pointer to the commands
         * @param size the amount of command data in bytes
         */
        void command( uint8_t* cmd, uint8_t size )
        {
            if ( size == 0 )
            {
                size = sizeof ( cmd );
            }

            printf( "SzC: %d\n", size );
            m_transaction.cmd = 0x0;
            m_transaction.length = ( m_command_bits + WORDSIZE ) * size;
            m_transaction.user = nullptr;
            printf( "SzC1: %d\n", size );
            if ( size > 4 )
            {
                printf( "SzC2: %d\n", size );
                m_transaction.flags = 0;
                memcpy( m_dma_buffer, cmd, size );
                m_transaction.tx_buffer = m_dma_buffer;
            }
            else
            {
                printf( "SzC3: %d\n", size );
                m_transaction.flags = SPI_TRANS_USE_TXDATA;
                memcpy( m_transaction.tx_data, cmd, size );
            }
            printf( "SzCT: %d spi:%p tx:%p\n", size, m_spi, &m_transaction );
            ESP_ERROR_CHECK( ::spi_device_transmit( m_spi, &m_transaction ) );
            printf( "SzCE: %d\n", size );
        }

        /**
         * @brief Transmit data to the device
         * @param data pointer to the data
         * @param size the amount of data in bytes
         */
        void data( uint8_t* data, uint8_t size )
        {
            m_transaction.flags = 0;
            m_transaction.length = ( m_command_bits + WORDSIZE ) * size;
            m_transaction.tx_buffer = m_dma_buffer;
            memcpy( m_dma_buffer, data, size );

            if ( m_dc == GPIO_NUM_NC )
            /*
             * 3-Wire, set data bit, clear user field
             */
            {
                printf( "SzD3: %d\n", size );
                m_transaction.cmd = 0x1;
                m_transaction.length += 1;
                m_transaction.user = nullptr;
                ESP_ERROR_CHECK( ::spi_device_transmit( m_spi, &m_transaction ) );
            }
            else
            /*
             * 4-Wire data - pass the DC pin in 'user' field of transaction fo it can be set
             */
            {
                m_transaction.user = &m_dc;    // Transaction 'user' is used to indicate 4-wire data send and to set the DC pin
                printf( "SzD4: %d\n", size );
                ESP_ERROR_CHECK( ::spi_device_transmit( m_spi, &m_transaction ) );
            }
        }

        /**
         *
         */
        virtual void info()
        {
            printf( "\nSPI Bus Info\n" );
            printf( "\nPin-Out - MOSI:%d  CLK:%d  CS:%d  DC:%d\n", m_mosi, m_clk, m_cs, m_dc );
        }    // info

};
#endif  // SSD1306_SPI_PIF_H_
