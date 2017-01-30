#ifndef TRAJECTOIRE_H
#define TRAJECTOIRE_H

#include "gps.h"


class GpsFramework{
 
public:
  GpsFramework();

  void onNewPoint(GpsPoint & gpsPoint);
  
  void savePointA(GpsPoint & gpsPoint);
  void savePointB(GpsPoint & gpsPoint);
 
  StatusTrajectoire m_status;
  GpsPoint m_pointA;
  GpsPoint m_pointB;

  double m_distance = 0.0;
  double m_vitesse = 0.0;

  double m_largeur = LARGEUR;
private:
  //AB
  double m_a;
  double m_b;
  double m_c; //m_a*x + m_b*y + m_c = 0
  double m_sqrt_m_a_m_b;
  
  double m_ab_x = 0.0;
  double m_ab_y = 0.0;

  void setAB();

  //vitesse deplacement
  GpsPoint m_lastPointVitesseDeplacement;
  
  double m_deplacementX;
  double m_deplacementY;
  double m_deplacementAngle;
  bool m_sensAB = true;
  void calculVitesseDeplacement(GpsPoint & gpsPoint);

  //distance
  void calculDistanceA(GpsPoint & gpsPoint);
  void calculDistanceAB(GpsPoint & gpsPoint);
  
  double distanceBetween(GpsPoint & gpsPoint1, GpsPoint & gpsPoint2);
  void changeStatusTrajectoire(StatusTrajectoire status);
};

void test_trajectoire();

#endif
