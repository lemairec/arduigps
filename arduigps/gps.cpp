#include "gps.h"
#include "pins.h"
#include "utils.h"
#include "arduigps.h"

#include <Arduino.h> 

#ifdef GPS_SOFTWARE_SERIAL
  #include <SoftwareSerial.h>
  SoftwareSerial mySerial(GPS_RX_PIN, GPS_TX_PIN);
  #define GPS_SERIAL mySerial
#else
  #define GPS_SERIAL Serial1
#endif

//#define GPS_DEBUG

AngleGps::AngleGps()
:m_part0(0), m_part1(0){
};
  AngleGps::AngleGps(int part0, int part1)
  :m_part0(part0), m_part1(part1){
    
  }

double AngleGps::toDouble(){
  int h = m_part0/100;
  double minu = (m_part0-(h*100));
  return h + (minu + m_part1*0.00001)/60.0;
}

AngleGps AngleGps::operator - (const AngleGps &a){
  AngleGps temp = *this;
  temp.m_part0 = m_part0 - a.m_part0;
  temp.m_part1 = m_part1 - a.m_part1;
  return temp;
}

byte set_10HZ[] = {0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0x64, 0x00, 0x01, 0x00, 0x01, 0x00, 0x7A, 0x12}; //10HZ
byte set_5HZ[] = {0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0xc8, 0x00, 0x01, 0x00, 0x01, 0x00, 0xDE, 0x6A}; //5HZ
byte set_2HZ[] = {0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0xf4, 0x01, 0x01, 0x00, 0x01, 0x00, 0x0B, 0x77}; //2HZ

void sendUBX(byte *UBXmsg, byte msgLength) {
  for(int i = 0; i < msgLength; i++) {
    GPS_SERIAL.write(UBXmsg[i]);
    GPS_SERIAL.flush();
  }
  GPS_SERIAL.println();
  GPS_SERIAL.flush();
}

void GpsPoint::reset(){
    m_latitude = AngleGps();
    m_longitude = AngleGps();
    m_x = 0;
    m_y = 0;
}

bool GpsPoint::isValid(){
    //return m_latitude > 0.1;
}

void GpsPoint::debug(){
    char m_data[40];
    for(size_t i = 0; i < 40; ++i){
      m_data[i] = ' ';
    }
    /*char s1[40];
    dtostrf(m_latitude, 4, 8, s1);
    char s2[40];
    dtostrf(m_longitude, 4, 8, s2);
    for(size_t i = 0; i < 11; ++i){
        m_data[i] = s1[i];
        m_data[i + 12] = s2[i];
    }*/
    m_data[10] = ',';
    print_serial_chars(m_data, 40);
    Serial.print(m_time);
    Serial.print('\n');
}

GpsModule::GpsModule()
{
  resetBuffer();
}

void GpsModule::init(){
#ifdef GPS_SOFTWARE_SERIAL
  INFO("init Gps software gps " << GPS_RX_PIN << " " << GPS_TX_PIN);
#else
  INFO("init Gps SERIAL_GPS");
#endif  
  GPS_SERIAL.begin(GPS_BAUDRATE);
  sendUBX(set_2HZ, sizeof(set_5HZ));
}

void GpsModule::readNextFrame(){
    while ( GPS_SERIAL.available()){
        char c = GPS_SERIAL.read();
        LOG_GPS_FRAME(c);
        this->readChar(c);
    }
}

void GpsModule::readChar(char c){
  if(c == '$'){
    //INFO("readChar");
    resetBuffer();
  } else if(c == '\n'){
    parseBuffer();
  } else {
    this->m_buffer[m_bufferIndLast] = c;
    m_bufferIndLast++;
  }
}

  

void GpsModule::printBuffer(){
  print_serial_chars(m_buffer, m_bufferIndLast);
  Serial.print('\n');
}

void GpsModule::resetBuffer(){
  m_bufferIndLast = 0;
  //for(size_t i = 0; i < 200; ++i){
  //  m_buffer[i] = ' ';
  //}
}


void GpsModule::error(){
    Serial.print("error");
    debug();
}

void GpsModule::debug(){
    Serial.print(m_tempInd);
    Serial.print(" ");
    Serial.print(m_buffer[m_tempInd-1]);
    Serial.print(m_buffer[m_tempInd]);
    Serial.print(m_buffer[m_tempInd+1]);
    Serial.print("\n");
}


//GPRMC,210136.00,A,4902.71610,N,00324.04158,E,0.150,,180516,
//$GPGGA,114608.00,4905.46094,N,00332.09303,E,2,07,1.46,87.8,M,46.3,M,,0000*6B

void GpsModule::parseBuffer(){
  m_tempInd = 0;
  #ifdef GPS_DEBUG
    printBuffer();
  #endif
  if(m_buffer[0] == 'G'){
    if(m_buffer[1] == 'N' || m_buffer[1] == 'P'){
      if(m_buffer[2] == 'G' && m_buffer[3] == 'G' && m_buffer[4] == 'A'){
        return parseGGA();
      }
    }
  }
}


void GpsModule::parseGGA(){
  readUntilCommat();
  m_lastGGAEvent.m_time = readDouble();
  readAngle(m_lastGGAEvent.m_latitude);
  readUntilCommat();
  readAngle(m_lastGGAEvent.m_longitude);
  readUntilCommat();
  m_lastGGAEvent.m_fix = readInt();
  m_lastGGAEvent.m_nbrSat = readInt();
  m_lastGGAEvent.m_precision = readDouble();
  m_lastGGAEvent.m_altitude = readDouble();

  m_lastGGAEvent.m_timeHour = getTimeHour(m_lastGGAEvent.m_time);
  setXY(m_lastGGAEvent);
  callbackGpsGpGGA(m_lastGGAEvent);
}


void GpsModule::parseRMC(){
  readUntilCommat();
  readUntilCommat();
  //debug();
  //readAngle(lastLatitude);
  readUntilCommat();
  //debug();

  //readAngle(lastLongitude);
  //m_lastFix = getOneInt();
}

bool GpsModule::isEqual(const char * c, size_t size){
  size_t j = m_tempInd;
  for(size_t i = 0; i < size; ++i){
    if(c[i] == m_buffer[j]){
      ++j;
    } else {
      //  INFO(c[i] << " " << m_buffer[j] << " " << i);
      return false;
    }
  }
  return true;
}

void GpsModule::readUntilCommat(){
    while(m_tempInd < m_bufferIndLast){
        if(m_buffer[m_tempInd] == ','){
            ++m_tempInd;
            return;
        }
        ++m_tempInd;
    }
    error();
}

int GpsModule::getOneInt(){
  char c = m_buffer[m_tempInd];
  m_tempInd++;  
  return getIntWithChar(c);
}

int GpsModule::getIntWithChar(char c){
  if(c =='0'){
    return 0;
  } else if(c =='1'){
    return 1;
  } else if(c =='2'){
    return 2;
  } else if(c =='3'){
    return 3;
  } else if(c =='4'){
    return 4;
  } else if(c =='5'){
    return 5;
  } else if(c =='6'){
    return 6;
  } else if(c =='7'){
    return 7;
  } else if(c =='8'){
    return 8;
  } else if(c =='9'){
    return 9;
  } else {
    error();
    return 0;
  }
}

int GpsModule::readInt(){
  int res = 0;
  while(m_tempInd < m_bufferIndLast){
    char c = m_buffer[m_tempInd];
    if(c == ','){
      ++m_tempInd;
      return res;
    } else {
      res = res*10 + getIntWithChar(c);
      ++m_tempInd;
    }
  }
  error();
  return 0;
}

double GpsModule::readDouble(){
  double res = 0;
  double virgule_part = 1;
  bool virgule = false;
  while(m_tempInd < m_bufferIndLast){
    char c = m_buffer[m_tempInd];
    int number = 0;
    if(c == ','){
      ++m_tempInd;
      return res;
    } else if(c =='.'){
      virgule = true;
    } else {
      number = getIntWithChar(c);
    }
    if(!virgule){
      res = res * 10 + number;
    } else {
      res = res + number * virgule_part;
      virgule_part = virgule_part * 0.1;
    }
    ++m_tempInd;
  }
  error();
  return 0.0;
}

  
void GpsModule::readAngle(AngleGps & angle)
{
  bool virgule = false;
  int virgule_part = 1;
  long res0 = 0;
  long res1 = 0;
  while(m_tempInd < m_bufferIndLast){
    char c = m_buffer[m_tempInd];
    int number = 0;
    if(c == ','){
      ++m_tempInd;
      for(int i = virgule_part; i < 6; ++i){
        res1 = res1*10;
      }
      angle.m_part0 = res0;
      angle.m_part1 = res1;
      return;
    } else if(c =='.'){
      virgule = true;
    } else {
      number = getIntWithChar(c);
    }

    if(!virgule){
      res0 = res0 * 10 + number;
    } else {
      res1 = res1 * 10 + number;
      ++virgule_part;
    }
    ++m_tempInd;
  }
}

double GpsModule::getTimeHour(double d)
{
    int h = d/10000;
    int minu = (d/100-(h*100));
    double s = d - h*10000.0 - minu*100.0;
    double res = h + minu/60.0 + s/3600.0;
    //INFO(d << " " << h << ":" << minu << ":" << s << " " << res);
    return res;
}

#define DEG2RAD(a)   ((a) / (180.0 / M_PI))
#define RAD2DEG(a)   ((a) * (180.0 / M_PI))

/**
 * TRANSFORME LAMBERT
 **/

//Algo 01
double __YGLatitudeISOFromLatitude(double lat, double e)
{
  return log(tan(M_PI_4+lat/2)*pow((1-e*sin(lat))/(1+e*sin(lat)),e/2));
}

double __YGLatitudeISO(double lat, double e)
{
  return log(tan(M_PI_4 + lat/2)*pow((1-e*sin(lat))/(1+e*sin(lat)),e/2));
}

void __YGCoordinatesTransform(GpsPoint & gpsPoint, double e, double n, double c, double lambda_c, double x_s, double y_s)
{
  double lon = DEG2RAD(gpsPoint.m_longitude.toDouble());
  double lat = DEG2RAD(gpsPoint.m_latitude.toDouble());
  double latiso = __YGLatitudeISO(lat,e);
  gpsPoint.m_x = x_s + c*exp(-n*latiso)*sin(n*(lon-lambda_c));
  gpsPoint.m_y = y_s - c*exp(-n*latiso)*cos(n*(lon-lambda_c));
}

static double lambert_n[6] = {0.7604059656, 0.7289686274, 0.6959127966, 0.6712679322, 0.7289686274, 0.7256077650};
static double lambert_c[6] = {11603796.98, 11745793.39, 11947992.52, 12136281.99, 11745793.39, 11754255.426};
static double lambert_xs[6]= {600000.0, 600000.0, 600000.0, 234.358, 600000.0, 700000.0};
static double lambert_ys[6]= {5657616.674, 6199695.768, 6791905.085, 7239161.542, 8199695.768, 12655612.050};
#define E_CLARK_IGN 0.08248325676
#define LON_MERID_GREENWICH 0.04079234433

#define LAMBERT_I 0
#define LAMBERT_II 1
#define LAMBERT_III 2
#define LAMBERT_IV 3
#define LAMBERT_II_E 4
#define LAMBERT_93 5
#define LAMBERT LAMBERT_I

/**
 * Cordonnees spheriques
 * https://en.wikipedia.org/wiki/Equirectangular_projection
  */

#define EARTH_RADIUS 6378137

AngleGps lat_0(4902,716100);
AngleGps lon_0(00324,41580);
double temp_x = EARTH_RADIUS * cos(-lat_0.toRadians());

void __SetXYSpherique(GpsPoint & gpsPoint){
    double lon = (gpsPoint.m_longitude - lon_0).toRadians();
    double lat = (gpsPoint.m_latitude - lat_0).toRadians();
    gpsPoint.m_x = temp_x * lon;
    gpsPoint.m_y = EARTH_RADIUS * lat;
}

void GpsModule::setReferencePoint(GpsPoint & gpsPoint){
  lat_0 = gpsPoint.m_latitude;
  lon_0 = gpsPoint.m_longitude;
  temp_x = EARTH_RADIUS * cos(-lat_0.toRadians());
}

void GpsModule::setXY(GpsPoint & gpsPoint){
  __SetXYSpherique(gpsPoint);
  //__YGCoordinatesTransform(gpsPoint, E_CLARK_IGN, lambert_n[LAMBERT], lambert_c[LAMBERT], LON_MERID_GREENWICH, lambert_xs[LAMBERT], lambert_ys[LAMBERT]);
}

/*
#include "logging.h"
#include "gps_reader.h"
GpsReader gpsReader;
GpsEvent gpsEvent;

void setup(){
  Serial.begin(9600);
  
  Serial.println("Listening...");
  gpsReader.init();
  while(!gpsEvent.isValid()){
    gpsReader.readNextFrame(gpsEvent);
  }
  
}

void loop(){
  gpsReader.readNextFrame(gpsEvent);
  if(gpsEvent.isValid()){
    gpsEvent.debug();
  }
}

#include <Arduino.h>
#include "pins.h"
#ifdef GPS_SOFTWARE_SERIAL
  #include <SoftwareSerial.h>
  SoftwareSerial mySerial2(GPS_RX_PIN, GPS_TX_PIN);
  #define GPS_SERIAL mySerial2
#else
  #define GPS_SERIAL Serial1
#endif
  
void setup(){
  mySerial2.begin(9600);
  Serial.begin(9600);
}

void loop(){
  //Serial.print("loop");
  while ( mySerial2.available()){
    char c = mySerial2.read();
    Serial.print(c);
  }
}

*/
