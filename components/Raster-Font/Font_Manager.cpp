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

#include "Font_Manager.h"

/**
 * @brief Instantiates a Font_manager for the given font and raster orientation
 *
 * @param fontindex the font to produce
 * @param raster The direction to rasterize the font
 */
Font_Manager::Font_Manager(uint8_t fontindex, Raster raster)
{
    m_font = fonts[fontindex]; // Err out if out of bounds
    m_raster = raster;
}

/**
 * @brief The number of fonts available
 * 
 * @return the number of fonts
 */
uint8_t Font_Manager::fontcount()
{
    return NUM_FONTS;
}

/**
 * @brief A list of the available fonts
 * 
 * @return the font list
 */
const char **Font_Manager::fontlist()
{
    static const char **fontlist = new const char *[NUM_FONTS];
    for (int i = 0; i < NUM_FONTS; i++)
    {
        fontlist[i] = fonts[i]->name;
        if (fontlist[i] == NULL)
            fontlist[i] = "** Font Name Missing **";
    }
    return fontlist;
}

/**
 * @brief   Get the font name
 * 
 * @return  font name
 */
const char *Font_Manager::font_name()
{
    return (m_font->name);
}

/**
 * @brief   Get the height of current selected font
 * 
 * @return  Height of the font (in pixels) or 0 if none font selected
 */
uint8_t Font_Manager::font_height()
{
    return (m_font->height);
}

/**
 * @brief   Get the "C" value (space between adjacent characters)
 * 
 * @return  "C" value
 */
uint8_t Font_Manager::font_c()
{
    return (m_font->c);
}

/**
 * @brief   Measure width of string with current selected font
 * 
 * @param   str  String to measure
 * @return  Width of the string
 */
uint16_t Font_Manager::measure_string(std::string str)
{
    if (str.empty())
        return 0;

    uint16_t w = 0;
    unsigned char c;

    for (std::string::iterator i = str.begin(); i < str.end(); i++)
    {
        c = *i;
        // we always have space in the font set
        if ((c < m_font->char_start) || (c > m_font->char_end))
            c = ' ';
        c = c - m_font->char_start;
        w += m_font->char_descriptors[c].width;
        if (*i)
            w += m_font->c;
    }

    return w;
}

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
Font_Manager::bitmap Font_Manager::rasterize(std::string str, uint16_t bitoffset)
{
    bitmap scan(m_raster, measure_string(str), m_font->height, bitoffset);

    for (char &c : str)
    {
        if ((c < m_font->char_start) || (c > m_font->char_end))
            c = ' ';
        c = c - m_font->char_start;
        raster(c, scan);
    };

    return scan;
}

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
Font_Manager::bitmap Font_Manager::rasterize(unsigned char c, uint16_t bitoffset)
{
    if ((c < m_font->char_start) || (c > m_font->char_end))
        c = ' ';
    c = c - m_font->char_start;

    bitmap scan(m_raster, m_font->char_descriptors[c].width, m_font->height, bitoffset);

    raster(c, scan);
    return scan;
}

/**
 * @brief rasterizes a character
 * 
 * @param c character to rasterize
 * @param scan the output raster scan of the character
 */
void Font_Manager::raster(unsigned char c, bitmap &bm)
{
    font_char_desc_t char_desc = m_font->char_descriptors[c];
    const uint8_t *bitmap = m_font->bitmap + char_desc.offset;       // Pointer to L-R bitmap
    uint8_t horizontal_read_bytes = 1 + ((char_desc.width - 1) / 8); // Bytes to read for horizontal
    uint8_t *data;                                                   // Data byte placement
    uint8_t shiftright{0};
    uint8_t linecoefficient{8}; // TBLR default
    uint8_t rowcoefficient{1};  // TBLR default

    if (bm.raster == LRTB)
    {
        shiftright = bm.xpoint % 8; // Number of bits to shift right on placement
        linecoefficient = 1;        // For vertical raster, each line is 1/8th shift
        rowcoefficient = 8;
    }

    for (uint8_t line = 0; line < m_font->height; line++)
    {
        uint16_t rowdataoffset = bm.width_bytes * ((line + bm.bitheightoffset) / linecoefficient);
        data = bm.data + rowdataoffset + (bm.xpoint / rowcoefficient); // address plus lines plus offset

        for (uint8_t chunk = 0; chunk < horizontal_read_bytes; chunk++)
        /*
         *  Read a horizontal slice of the current character, place it in the scan
         */
        {
            uint8_t word = *bitmap++; // Read the next byte

            switch (bm.raster)
            {
            case LRTB:
                /*
                * Process the byte into the current location, across byte boundaries if needed
                */
                *data++ |= (word >> shiftright); // Font char MSBs shifted to end of destination byte
                if (shiftright)
                {
                    *data |= (word << (8 - shiftright)); // Font char LSB shifted to start of next destination byte
                }
                break;

            case TBLR:
                /*
                * Process the bits, each going into a different vertical segment in the same bit position
                */
                uint8_t bitposition = (line + bm.bitheightoffset) % 8; // Vertical in the byte, little endian
                uint8_t seg = (8 * chunk);
                uint8_t limit = seg + 7;
                if (limit > char_desc.width)
                    limit = char_desc.width;

                for (; seg <= limit; seg++)
                /*
                * Bit Cycle through this horizontal byte, each goes to a different segment
                * Font is Big-Endian, Segment is Little-Endian
                */
                {
                    if (word & MSBITS[seg % 8]) // Font bit is set in this bit position
                    {
                        *(data + seg) |= (1 << bitposition); // Set bit
                    }
                }
                break;
            }
        }
    }
    bm.xpoint += char_desc.width + m_font->c; // Increment pointer to next char
}
