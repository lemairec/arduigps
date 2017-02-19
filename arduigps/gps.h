#ifndef GPS_H
#define GPS_H
#include "logging.h"

struct GpsPoint {
  double m_altitude;
    
  double m_latitude = 0.0;
  double m_longitude = 0.0;
    
  double m_x = 0.0;
  double m_y = 0.0;

  double m_time;
  double m_timeHour;

  void reset();
  bool isValid();

  void debug();
};

struct GGAFrame : public GpsPoint {
  int m_fix;
  int m_nbrSat;
  double m_precision;

  void reset();
};

class GpsModule {
public:
  GpsModule();
  void init();

  void readNextFrame();
  void readChar(char c);
  
  
  GGAFrame m_lastGGAEvent;

  void setReferencePoint(GpsPoint & gpsPoint);
  void setXY(GpsPoint & gpsPoint);
private:
  int m_bufferIndLast = 0;
  char m_buffer[200];
  int m_tempInd = 0;

  void printBuffer();
  void resetBuffer();
  void error();
  void debug();

  void parseBuffer();
  void parseGGA();
  void parseRMC();

  bool isEqual(const char * c, size_t size);
  void readUntilCommat();
  int getOneInt();
  int getIntWithChar(char c);
  int readInt();
  double readDouble();
  double readDeg();

  double getTimeHour(double d);
};

#endif //GPS_H
