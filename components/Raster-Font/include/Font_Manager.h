/*
 Raster-Font Library Font Manager

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

#ifndef INCLUDE_FONT_MANAGER_H_
#define INCLUDE_FONT_MANAGER_H_

#include <stdint.h>
#include <stdlib.h>
#include <algorithm>
#include <cstring>
#include <string>

#include "fonts.h"

/**
 *
 */
class Font_Manager
{
    public:

        /**
         *
         */
        enum Raster
        {
            LRTB,
            TBLR,
        };

        /**
         *
         */
        struct bitmap
        {
                const Raster raster;
                uint16_t bitwidth;    // bits
                uint8_t bitheight;    // bits

                uint8_t widthoffset { 0 };    // bits
                uint8_t heightoffset { 0 };    // bits
                uint16_t width { 0 };       // bytes needed
                uint16_t height { 0 };      // bytes needed
                uint16_t xpoint { 0 };      // Current bit-point to place scan data
                uint8_t * data { nullptr };

                bitmap( Raster r, uint16_t w, uint8_t h, uint16_t bitoffset ) :
                        raster { r }, bitwidth { w }, bitheight { h }
                {
                    switch ( raster )
                    /*
                     * Calc width, height in Bytes
                     */
                    {
                        case LRTB:
                            widthoffset = bitoffset % 8;
                            bitwidth += widthoffset;
                            width = ( ( bitwidth - 1 ) / 8 ) + 1;
                            height = bitheight;     // Bytes
                            xpoint = widthoffset;     // Bytes
                            break;
                        case TBLR:
                            heightoffset = bitoffset % 8;
                            bitheight += heightoffset;
                            width = bitwidth;     // Bytes
                            height = ( ( bitheight - 1 ) / 8 ) + 1;     // Bytes
                            break;
                    }
                    data = (uint8_t*) malloc( width * height );
                    memset( data, 0, width * height );
                }

                ~bitmap()
                {
                    delete [] data;
                }
        };

        /**
         * @brief Instantiates a Font_manager for the given font and raster orientation
         *
         * @param fontindex the font to produce
         * @param raster The direction to rasterize the font
         */
        Font_Manager( uint8_t fontindex, Raster raster );

        virtual ~Font_Manager()
        {
        }

        /**
         * @brief The number of fonts available
         * @return the number of fonts
         */
        static uint8_t fontcount();

        /**
         * @brief A list of the available fonts
         * @return the font list
         */
        static const char** fontlist();

        /**
         * @brief   Get the font name
         * @return  font name
         */
        const virtual char * font_name();

        /**
         * @brief   Get the height of current selected font
         * @return  Height of the font (in pixels) or 0 if none font selected
         */
        virtual uint8_t font_height();

        /**
         * @brief   Get the "C" value (space between adjacent characters)
         * @return  "C" value
         */
        virtual uint8_t font_c();

        /**
         * @brief   Measure width of string with current selected font
         * @param   str         String to measure
         * @return  Width of the string
         */
        virtual uint16_t measure_string( std::string str );

        /**
         * @brief Bitmaps a string using the font, shifting the bitmap as required.
         *
         * Raster the string using the font manager settings into a new bitmap structure.
         * The string can be shifted along or down in the bitmap by up to 7 bits -
         * the x or y shift is calculated from modulus 8 of the shift parameters, allowing
         * absolute position to be used without prior calculation from the caller.
         *
         * @param str String to bitmap
         * @param bitoffset The number of bits to shift the bitmap
         * @return Bitmap of the string
         */
        virtual bitmap rasterize( std::string str, uint16_t bitoffset = 0 );

        /**
         * @brief Bitmaps a character using the font, shifting the bitmap as required.
         *
         * Raster the character using the font manager settings into a new bitmap structure.
         * The character can be shifted along or down in the bitmap by up to 7 bits -
         * the x or y shift is calculated from modulus 8 of the shift parameters, allowing
         * absolute position to be used without prior calculation from the caller.
         *
         * @param c The character to bitmap
         * @param bitoffset The number of bits to shift the bitmap
         * @return Bitmap of the char
         */
        virtual bitmap rasterize( unsigned char c, uint16_t bitoffset = 0 );

    private:

        void raster( unsigned char c, bitmap& scan );

        const uint8_t MSBITS [ 8 ] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };    /// < Segment bit mask

        const font_info_t* m_font;    /// < Current font
        Raster m_raster;
};

#endif /* INCLUDE_FONT_MANAGER_H_ */
