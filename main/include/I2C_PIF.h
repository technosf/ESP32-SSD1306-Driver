/*
 ESP32-SSD1306-Driver Library I2C Driver

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

#ifndef SSD1306_I2C_PIF_H_
#define SSD1306_I2C_PIF_H_

#include <stdint.h>

#include <driver/i2c.h>

#include "PIF.h"

/**
 * @brief I2C implementation of PIF 
 * 
 */
class I2C_PIF : public PIF
{
        static const constexpr int BUSSPEEDHZ = 1000000;

    private:

        i2c_port_t i2c_master_port = I2C_NUM_0;
        gpio_num_t m_scl;
        gpio_num_t m_sda;
        uint8_t m_address_read;
        uint8_t m_address_write;
        i2c_cmd_handle_t cmdlink { NULL };

        /**
         * @brief
         * @param ctrl
         * @param data
         */
        void write(const uint8_t ctrl, const uint8_t data )
        {
            i2c_cmd_handle_t cmdlink = i2c_cmd_link_create();
            i2c_master_start( cmdlink );
            i2c_master_write_byte( cmdlink, m_address_write, 1 );
            i2c_master_write_byte( cmdlink, ctrl, 1 );
            i2c_master_write_byte( cmdlink, data, 1 );
            i2c_master_stop( cmdlink );
            i2c_master_cmd_begin( i2c_master_port, cmdlink, 50 / portTICK_RATE_MS );
            i2c_cmd_link_delete( cmdlink );
        }

        /**
         *
         * @param ctrl
         * @param data
         * @param size
         */
        void write(const uint8_t ctrl, const uint8_t* data, uint8_t size )
        {
            uint8_t* d =  (uint8_t*)data;
            i2c_cmd_handle_t cmdlink = i2c_cmd_link_create();
            i2c_master_start( cmdlink );
            i2c_master_write_byte( cmdlink, m_address_write, 1 );
            i2c_master_write_byte( cmdlink, ctrl, 1 );
            i2c_master_write( cmdlink, d, size, 1 );
            i2c_master_stop( cmdlink );
            i2c_master_cmd_begin( i2c_master_port, cmdlink, 50 / portTICK_RATE_MS );
            i2c_cmd_link_delete( cmdlink );
        }

    public:

        /**
         *
         * @param scl
         * @param sda
         * @param address
         */
        I2C_PIF( gpio_num_t scl, gpio_num_t sda, uint8_t address ) :
                m_scl { scl }, m_sda { sda }
        {
            m_address_read = ( address << 1 ) | I2C_MASTER_READ;
            m_address_write = ( address << 1 ) | I2C_MASTER_WRITE;
            i2c_config_t conf;
            conf.mode = I2C_MODE_MASTER;
            conf.sda_io_num = sda;
            conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
            conf.scl_io_num = scl;
            conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
            conf.master.clk_speed = BUSSPEEDHZ;

            ESP_ERROR_CHECK( i2c_param_config( i2c_master_port, &conf ) );
            ESP_ERROR_CHECK( i2c_driver_install( i2c_master_port, I2C_MODE_MASTER, 0, 0, 0 ) );

        }

        virtual ~I2C_PIF()
        {
            i2c_cmd_link_delete( cmdlink );
        }

        /**
         *
         * @param cmd
         */
        void command(const uint8_t cmd )
        {
            write( 0x00, cmd );
        }

        void command(const uint8_t* cmd, uint8_t size )
        {
            if ( size > 0 )
            {
                write( 0x00, cmd, size );
            }
            else
            {
                write( 0x00, cmd, sizeof ( cmd ) );
            }
        }

        /**
         *
         * @param data
         * @param size
         */
        void data( uint8_t* data, uint8_t size )
        {
            write( 0x40, data, size );
        }

        /**
         *
         */
        virtual void info()
        {
            printf( "\nI2C Bus Scan\n" );
            uint8_t address;
            printf( "     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\r\n" );
            for ( int i = 0; i < 128; i += 16 )
            {
                printf( "%02x: ", i );
                for ( int j = 0; j < 16; j++ )
                {
                    fflush( stdout );
                    address = i + j;
                    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
                    i2c_master_start( cmd );
                    i2c_master_write_byte( cmd, ( address << 1 ) | I2C_MASTER_WRITE, 1 );
                    i2c_master_stop( cmd );
                    esp_err_t ret = i2c_master_cmd_begin( i2c_master_port, cmd, 50 / portTICK_RATE_MS );
                    i2c_cmd_link_delete( cmd );
                    if ( ret == ESP_OK )
                    {
                        printf( "%02x ", address );
                    }
                    else if ( ret == ESP_ERR_TIMEOUT )
                    {
                        printf( "UU " );
                    }
                    else
                    {
                        printf( "-- " );
                    }
                }
                printf( "\r\n" );
            }
            printf( "\n\n" );

        }    // info
};

#endif  // SSD1306_I2C_PIF_H_
