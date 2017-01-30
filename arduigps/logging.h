#ifndef LOGGING_H
#define LOGGING_H

#include <SoftwareSerial.h>
#include <Arduino.h>
#include "config.h" 

template<class T> inline Print &operator <<(Print &obj, T arg) { obj.print(arg); return obj; } 

enum StatusTrajectoire{
  NoPoint, PointA, PointAB
};


#ifdef JUST_LOG_GPS_FRAME
#  define LOG_GPS_FRAME(x) Serial << x
#  define INFO(x)
#else
#  define LOG_GPS_FRAME(x)
#  define INFO(x) Serial << x << "\n"
#endif

#endif
