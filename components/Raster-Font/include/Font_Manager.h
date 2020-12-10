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
#include <cstring>
#include <string>

#include "fonts.h"

/**
 * @brief Manager for bitmapped font descriptions
 * 
 * Provides functions to rasterizes strings in the given font
 */
class Font_Manager
{
public:
    /**
     * @brief Raster type
     * 
     * Left->Right_Top->Bottom
     * Top->Bottom Left->Right
     */
    enum Raster
    {
        LRTB,
        TBLR,
    };

    /**
     * @brief Contains data for rastered content and its placement 
     * 
     */
    struct bitmap
    {
        const Raster raster;
        uint16_t bitwidth;          ///< bits
        uint8_t bitheight;          ///< bits
        uint8_t bitwidthoffset{0};  ///< bits
        uint8_t bitheightoffset{0}; ///< bits
        uint16_t width_bytes{0};    ///< bytes needed
        uint16_t height_bytes{0};   ///< bytes needed
        uint16_t xpoint{0};         ///< Current bit-point to place scan data
        uint8_t *data{nullptr};     ///< The rastered data

        /**
         * @brief Construct a new bitmap object
         * 
         * @param r raster model
         * @param w bitmap witdth in bits
         * @param h bitmap heigth in bits
         * @param bitoffset the offset the result is going to be pushed when displayed
         */
        bitmap(Raster r, uint16_t w, uint8_t h, uint16_t bitoffset) : raster{r}, bitwidth{w}, bitheight{h}
        {
            switch (raster)
            /*
            * Calc width, height in Bytes
            */
            {
            case LRTB:
                bitwidthoffset = bitoffset % 8;
                bitwidth += bitwidthoffset;
                width_bytes = ((bitwidth - 1) / 8) + 1;
                height_bytes = bitheight; // Bytes
                xpoint = bitwidthoffset;  // Bytes
                break;
            case TBLR:
                bitheightoffset = bitoffset % 8;
                bitheight += bitheightoffset;
                width_bytes = bitwidth;                   // Bytes
                height_bytes = ((bitheight - 1) / 8) + 1; // Bytes
                break;
            }
            data = (uint8_t *)calloc(width_bytes, height_bytes);
        }

        ~bitmap()
        {
            delete[] data;
        }
    };

    Font_Manager(uint8_t fontindex, Raster raster);

    virtual ~Font_Manager()
    {
    }

    static uint8_t fontcount();
    static const char **fontlist();
    const virtual char *font_name();
    uint8_t font_height();
    uint8_t font_c();
    uint16_t measure_string(std::string str);
    bitmap rasterize(std::string str, uint16_t bitoffset = 0);
    bitmap rasterize(unsigned char c, uint16_t bitoffset = 0);

private:
    const uint8_t MSBITS[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01}; /// < Segment bit mask
    const font_info_t *m_font;                                                  /// < Current font

    Raster m_raster; ///< The raster type of this Font Manager

    void raster(unsigned char c, bitmap &scan);
};

#endif /* INCLUDE_FONT_MANAGER_H_ */
