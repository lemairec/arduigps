#ifndef LCD_SCREEN
#define LCD_SCREEN

#include "config.h"
#include "logging.h"

#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

class LcdModule {
public:
  LcdModule();
  void init();
  void updateSat(int fix, int nbSat, double precision);
  void updateLatLon(double latitude, double longitude);
  void updateTime(double time);
  void updateVitesse(double vitesse);
  void updateLargeur(double largeur);
  void updateDistance(double distance);
  void updateStatusTrajectoire(StatusTrajectoire s);
  void printEvent(char *);

  void readKeyAndUpdate();
  
private:
  StatusTrajectoire m_statusTrajectoire = NoPoint;
  bool m_update = false;
  int m_line = 0;
  int m_lastButton = btnNONE;

  int m_fix = 0;
  int m_nbSat = 0;
  double m_precision = 0.0;

  double m_latitude = 0.0;
  double m_longitude = 0.0;

  double m_time = 0.0;
  double m_vitesse = 0.0;
  
  double m_largeur = LARGEUR;
  
  double m_distance = 0;

  
  
  void printLcd();
  int readButtons();
  void drawDistanceAB();
  void drawDistanceA();
};

#endif //LCD_SCREEN
