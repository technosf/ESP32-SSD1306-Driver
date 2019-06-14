/*
 * oled_fonts.c
 *
 *  Created on: Jan 3, 2015
 *      Author: Baoshi
 */
//#include "esp_common.h"
#include "fonts.h"
#include "font_bitocra_4x7_ascii.h"
#include "font_bitocra_6x11_iso8859_1.h"
#include "font_bitocra_7x13_iso8859_1.h"
#include "font_glcd_5x7.h"
#include "font_roboto_8pt.h"
#include "font_roboto_10pt.h"
#include "font_tahoma_8pt.h"
#include "font_terminus_6x12_iso8859_1.h"
#include "font_terminus_8x14_iso8859_1.h"
#include "font_terminus_10x18_iso8859_1.h"
#include "font_terminus_11x22_iso8859_1.h"
#include "font_terminus_12x24_iso8859_1.h"
#include "font_terminus_14x28_iso8859_1.h"
#include "font_terminus_16x32_iso8859_1.h"
#include "font_terminus_bold_8x14_iso8859_1.h"
#include "font_terminus_bold_10x18_iso8859_1.h"
#include "font_terminus_bold_11x22_iso8859_1.h"
#include "font_terminus_bold_12x24_iso8859_1.h"
#include "font_terminus_bold_14x28_iso8859_1.h"
#include "font_terminus_bold_16x32_iso8859_1.h"
#include "font_terminus_6x12_koi8_r.h"
#include "font_terminus_8x14_koi8_r.h"
#include "font_terminus_14x28_koi8_r.h"
#include "font_terminus_16x32_koi8_r.h"
#include "font_terminus_bold_8x14_koi8_r.h"
#include "font_terminus_bold_14x28_koi8_r.h"
#include "font_terminus_bold_16x32_koi8_r.h"

const font_info_t * fonts [ NUM_FONTS ] = { &_fonts_bitocra_4x7_ascii_info, &_fonts_bitocra_6x11_iso8859_1_info,
        &_fonts_bitocra_7x13_iso8859_1_info, &_fonts_glcd_5x7_info, &_fonts_roboto_8pt_info, &_fonts_roboto_10pt_info,
        &_font_tahoma_8pt_info, &_fonts_terminus_6x12_iso8859_1_info, &_fonts_terminus_8x14_iso8859_1_info,
        &_fonts_terminus_10x18_iso8859_1_info, &_fonts_terminus_11x22_iso8859_1_info,
        &_fonts_terminus_12x24_iso8859_1_info, &_fonts_terminus_14x28_iso8859_1_info,
        &_fonts_terminus_16x32_iso8859_1_info, &_fonts_terminus_bold_8x14_iso8859_1_info,
        &_fonts_terminus_bold_10x18_iso8859_1_info, &_fonts_terminus_bold_11x22_iso8859_1_info,
        &_fonts_terminus_bold_12x24_iso8859_1_info, &_fonts_terminus_bold_14x28_iso8859_1_info,
        &_fonts_terminus_bold_16x32_iso8859_1_info,
//    &_fonts_terminus_6x12_koi8_r_info,
//    &_fonts_terminus_8x14_koi8_r_info,
//    &_fonts_terminus_14x28_koi8_r_info,
//    &_fonts_terminus_16x32_koi8_r_info,
//    &_fonts_terminus_bold_8x14_koi8_r_info,
//    &_fonts_terminus_bold_14x28_koi8_r_info,
//    &_fonts_terminus_bold_16x32_koi8_r_info
        };
