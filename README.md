#ESP32 SSD1306 Driver

Software driver for the **SSD1306** OLED driver chip for the **ESP-IDF** toolchain, with pluggable wire protocol interface (with included I2C and 3-wire SPI implementations), written to efficiently manage and write to the paged-memory architecture of the SSD1306.

The included _main_ class will run through some graphic experiences for you :)


##Architecture and Operation

The driver is split into three components centered on the SDD1306 driver, with wire-protocol and graphics interfacing to it. It's designed to provide funtionality to draw basic shapes and text at the desired location on the display conected to an ESP32 via I2C or SPI. The SSD1306 driver component itself can be augmented with new wire-level protocols, and more advanced graphics interfaces.

###SSD1306 Driver
The _SSD1306 driver_ takes base pixel and area-orientated graphics commands from upstream, applies clipping, and convert these commands efficiently into the paged-memory orientated SSD1306 instructions and data, and spits them out through a provided wire-level Protocol Interface (PIF). 

The SSD1306 chip manages its display memory as sets of byte _segments_ that represent 8-verticle pixels in horizonal rows called _pages_. Updates to the dislay are sent as sets of segments with page and column coordinates. This driver implements orthogonal area commands that are processed with a bias towards the vertical axis, so that  the segment udaes are minimalized. The area commands themselves are pure point or horizonatal/vertical line or box draws. There is also a _segment_ command to update one or more segments that can be used by higher level optimized graphics drivers.

###Wire-level Protocol Interface
The PIF is abstraction of the SSD1306 communication tasks, essentialy send commonds or send data, thats allows a the SSD1306 driver to communicate to chip via whichever protocol the display implements. I also added a call to retrieve information on the protocol configuration, and if possible, identify connectd devices. 

Currently there are ESP-IDF v3 implementations of I2C and three-wire SPI. 

###Graphics
The graphics component adds a higher level commands to draw and fill boxes and circles, and also to output font characters. This interface is also aware of the SSD1306 paged-memory architecture and will look at distilling everything into SSD1306 segments. Font characters are drawn from horizontal scanned font representations and tranformed on-the-fly into vertical representations to avoid building characters one-pixel-at-a-time in the display memory.  

##Future Features
Thinking about what could be added:
* Virtual displays viewports (for split display regions, or multiple displays
* Virtual displays for multiple physical displays/drivers
* 4-wire SPI driver
* More and improved graphics funtions
* Antialiasing
* Vertical scanning fonts

##  Versions

* _0.1.0_	Initial commit and pre-release


## History and References

This repo is a fork of [imxieyi/esp32-i2c-ssd1306-oled](https://github.com/imxieyi/esp32-i2c-ssd1306-oled), an I2C-only driver for the eponomous SSD1306 OLED driver chip. Interested in an SPI wire protocol version also, I dug into the code and decided to refactor it, add a pluggable wire protocol component, and before I knew it changed the memory management and the base graphics too, and especially the font processing. 

The code has been reorganized too with the font management component, which appear to originate from [Baoshi's repo](https://github.com/baoshi/ESP-I2C-OLED), being moved out to its own components to ease integrating new font representation classes.


## License

ESPxRGB - Copyright 2019  technosf  [http://github.com/technosf]

Licensed under the GNU LESSER GENERAL PUBLIC LICENSE, Version 3.0 or greater (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
http://www.gnu.org/licenses/lgpl-3.0.en.html
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
![lglp3 logo](http://www.gnu.org/graphics/lgplv3-88x31.png)

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

