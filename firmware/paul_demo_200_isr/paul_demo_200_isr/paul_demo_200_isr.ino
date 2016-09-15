// Demo code for epapercard
// by Paul Schow
// https://github.com/paulschow/epapercard
// Switches between four images on button press
// Based on demo_200.ino by Pervasive Displays, Inc.
// Original copyright statement below

// Copyright 2013-2015 Pervasive Displays, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at:
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
// express or implied.  See the License for the specific language
// governing permissions and limitations under the License.


#if defined(ENERGIA)
#include <Energia.h>
#else
#include <Arduino.h>
#endif

#include <inttypes.h>
#include <ctype.h>

// required libraries
#include <SPI.h>
//#include <EPD_FLASH.h>
#include "EPD_V110_G1.h"
//#include <EPD_V231_G2.h>
#define SCREEN_SIZE 200
#include "EPD_PANELS.h"
#include "EPD_PINOUT.h"


// select two images from:  text_image text-hello cat aphrodite venus saturn
#define IMAGE_1  xsaturn
#define IMAGE_2  prev
#define IMAGE_3  font
#define IMAGE_4  x2saturn

// Error message for MSP430
//#if (SCREEN_SIZE == 270) && defined(__MSP430G2553__)
//#error MSP430: not enough memory
//#endif

// no futher changed below this point

// current version number
#define DEMO_VERSION "5"

// pre-processor convert to string
#define MAKE_STRING1(X) #X
#define MAKE_STRING(X) MAKE_STRING1(X)

// other pre-processor magic
// token joining and computing the string for #include
#define ID(X) X
#define MAKE_NAME1(X,Y) ID(X##Y)
#define MAKE_NAME(X,Y) MAKE_NAME1(X,Y)
#define MAKE_JOIN(X,Y) MAKE_STRING(MAKE_NAME(X,Y))

// calculate the include name and variable names
#define IMAGE_1_FILE MAKE_JOIN(IMAGE_1,EPD_IMAGE_FILE_SUFFIX)
#define IMAGE_1_BITS MAKE_NAME(IMAGE_1,EPD_IMAGE_NAME_SUFFIX)
#define IMAGE_2_FILE MAKE_JOIN(IMAGE_2,EPD_IMAGE_FILE_SUFFIX)
#define IMAGE_2_BITS MAKE_NAME(IMAGE_2,EPD_IMAGE_NAME_SUFFIX)
#define IMAGE_3_FILE MAKE_JOIN(IMAGE_3,EPD_IMAGE_FILE_SUFFIX)
#define IMAGE_3_BITS MAKE_NAME(IMAGE_3,EPD_IMAGE_NAME_SUFFIX)
#define IMAGE_4_FILE MAKE_JOIN(IMAGE_4,EPD_IMAGE_FILE_SUFFIX)
#define IMAGE_4_BITS MAKE_NAME(IMAGE_4,EPD_IMAGE_NAME_SUFFIX)

// Add Images library to compiler path
#include "Images.h"  // this is just an empty file

// images
PROGMEM const
#define unsigned
#define char uint8_t
#include IMAGE_1_FILE
#undef char
#undef unsigned

PROGMEM const
#define unsigned
#define char uint8_t
#include IMAGE_2_FILE
#undef char
#undef unsigned

PROGMEM const
#define unsigned
#define char uint8_t
#include IMAGE_3_FILE
#undef char
#undef unsigned

PROGMEM const
#define unsigned
#define char uint8_t
#include IMAGE_4_FILE
#undef char
#undef unsigned

// LED anode through resistor to I/O pin
// LED cathode to Ground
#define LED_ON  HIGH
#define LED_OFF LOW

// define the E-Ink display
EPD_Class EPD(EPD_SIZE,
          Pin_PANEL_ON,
          Pin_BORDER,
          Pin_DISCHARGE,
#if EPD_PWM_REQUIRED
          Pin_PWM,
#endif
          Pin_RESET,
          Pin_BUSY,
          Pin_EPD_CS);




// I/O setup
void setup() {

#if EPD_PWM_REQUIRED
    pinMode(Pin_PWM, OUTPUT);
#endif
    pinMode(Pin_BUSY, INPUT);
    pinMode(Pin_RESET, OUTPUT);
    pinMode(Pin_PANEL_ON, OUTPUT);
    pinMode(Pin_DISCHARGE, OUTPUT);
    pinMode(Pin_BORDER, OUTPUT);
    pinMode(Pin_EPD_CS, OUTPUT);
    //pinMode(Pin_EPD_FLASH_CS, OUTPUT);

#if EPD_PWM_REQUIRED
    digitalWrite(Pin_PWM, LOW);
#endif
    digitalWrite(Pin_RESET, LOW);
    digitalWrite(Pin_PANEL_ON, LOW);
    digitalWrite(Pin_DISCHARGE, LOW);
    digitalWrite(Pin_BORDER, LOW);
    digitalWrite(Pin_EPD_CS, LOW);
    //digitalWrite(Pin_EPD_FLASH_CS, HIGH);

    pinMode(Pin_RED_LED, OUTPUT);

    pinMode(P1_3, INPUT_PULLUP); // Attach pullup to 1.3 (button)
    attachInterrupt(P1_3, awake, FALLING); // Attach falling interrupt to 1.3
}


static int state = 1;


// main loop
void loop() {
  digitalWrite(Pin_RED_LED, LED_ON);
  changeimage(); // Change the picture
  digitalWrite(Pin_RED_LED, LED_OFF);
  suspend(); // Enter low power mode 4 (0.1uA)
}


void changeimage(){
    //int temperature = 40; // set temp to 40 for faster changing
    EPD.begin(); // power up the EPD panel
    EPD.setFactor(45); // adjust for current temperature
    //int delay_counts = 0;
    switch(state) {
    default:

    case 1:         // clear -> text
#if EPD_IMAGE_ONE_ARG
        EPD.image(IMAGE_1_BITS);
#elif EPD_IMAGE_TWO_ARG
        EPD.image_0(IMAGE_1_BITS);
#else
#error "unsupported image function"
#endif
        ++state;
        break;

    case 2:         // text -> picture
#if EPD_IMAGE_ONE_ARG
        EPD.image(IMAGE_2_BITS);
#elif EPD_IMAGE_TWO_ARG
        EPD.image(IMAGE_1_BITS, IMAGE_2_BITS);
#else
#error "unsupported image function"
#endif
        ++state;
        break;

    case 3:        // picture -> text
#if EPD_IMAGE_ONE_ARG
        EPD.image(IMAGE_3_BITS);
#elif EPD_IMAGE_TWO_ARG
        EPD.image(IMAGE_2_BITS, IMAGE_3_BITS);
#else
#error "unsupported image function"
#endif
        ++state;
        break;

        case 4:        // picture -> text
#if EPD_IMAGE_ONE_ARG
        EPD.image(IMAGE_4_BITS);
#elif EPD_IMAGE_TWO_ARG
        EPD.image(IMAGE_3_BITS, IMAGE_4_BITS);
#else
#error "unsupported image function"
#endif
        ++state;
        break;

        case 5:        // picture -> text
#if EPD_IMAGE_ONE_ARG
        EPD.image(IMAGE_1_BITS);
#elif EPD_IMAGE_TWO_ARG
        EPD.image(IMAGE_4_BITS, IMAGE_1_BITS);
#else
#error "unsupported image function"
#endif
        state = 2;  // back to picture next time
        //++state;
        break;

    }
    EPD.end();   // power down the EPD panel

}

// interrupt service routine
void awake(void)
{
  wakeup(); // wakeup from lpm4
}
