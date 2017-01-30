#include "config.h"

#ifdef GPS
  #define GPS_SOFTWARE_SERIAL
  #ifdef GPS_SOFTWARE_SERIAL
    #define GPS_RX_PIN            12
    #define GPS_TX_PIN            13
  #endif
  #define GPS_BAUDRATE 9600
#endif
         

#ifdef ULTRA_LCD
  #define PIN_LCD_1 8
  #define PIN_LCD_2 9
  #define PIN_LCD_3 4
  #define PIN_LCD_4 5
  #define PIN_LCD_5 6
  #define PIN_LCD_6 7
#endif
