#include "framework.h"
#include "arduigps.h"
#include <Arduino.h> 
#include <math.h>

GpsFramework::GpsFramework(){
  m_status = NoPoint;
}

void GpsFramework::savePointA(GpsPoint & gpsPoint){
  m_pointA = gpsPoint;
  changeStatusTrajectoire(PointA);
  print_message("savePointA");
#ifdef JUST_LOG_GPS_FRAME
  Serial.print("\n$SPA,");
  m_pointA.debug();
#endif
}
void GpsFramework::savePointB(GpsPoint & gpsPoint){
  if(m_status == PointA){
    m_pointB = gpsPoint;
    setAB();
    changeStatusTrajectoire(PointAB);
    print_message("savePointB");
  } else {
    print_message("error saveA first");
  }
#ifdef JUST_LOG_GPS_FRAME
  Serial.print("\n$SPB,");
  m_pointB.debug();
#endif
}

void GpsFramework::onNewPoint(GpsPoint & gpsPoint){
  calculVitesseDeplacement(gpsPoint);
  if(m_status == PointA){
    calculDistanceA(gpsPoint);
  }else if(m_status == PointAB){
    calculDistanceAB(gpsPoint);
  }
}

void GpsFramework::calculVitesseDeplacement(GpsPoint & gpsPoint){
  m_deplacementX = m_lastPointVitesseDeplacement.m_x - gpsPoint.m_x;
  m_deplacementY = m_lastPointVitesseDeplacement.m_y - gpsPoint.m_y;

  if(m_ab_x != 0 || m_ab_y != 0){
    double det = m_ab_x*m_deplacementY + m_ab_y*m_deplacementX;
    m_sensAB = (det < 0);
  }
  
  double distance = sqrt(m_deplacementX*m_deplacementX + m_deplacementY*m_deplacementY)/1000.0;;
  double delta_time = gpsPoint.m_timeHour - m_lastPointVitesseDeplacement.m_timeHour;
  m_vitesse = distance/delta_time;
  m_lastPointVitesseDeplacement = gpsPoint;
}



void GpsFramework::calculDistanceA(GpsPoint & gpsPoint){
  //double dist = distanceBetween(m_pointA, gpsPoint);
  double dx = m_pointA.m_x - gpsPoint.m_x;
  double dy = m_pointA.m_y - gpsPoint.m_y;
  m_distance = sqrt(dx*dx + dy*dy);
}

void GpsFramework::calculDistanceAB(GpsPoint & gpsPoint){
  if(m_sqrt_m_a_m_b==0){
    m_distance = 0.0;
  } else {    
    //INFO("ym "<< gpsPoint.m_y << " xm " << gpsPoint.m_x << " " << gpsPoint.m_y - m_a * gpsPoint.m_x - m_b);
    //INFO("m_a " << m_a << " m_b " << m_b << " " << m_sqrt_m_a_m_b);
    m_distance =  (m_a * gpsPoint.m_x + m_b * gpsPoint.m_y + m_c)/m_sqrt_m_a_m_b;
    if(!m_sensAB){
      m_distance = -m_distance;
    }
    m_distance = fmod(m_distance, m_largeur);
    if(m_distance > m_largeur/2){
      m_distance -= m_largeur;
    }
    if(m_distance < -m_largeur/2){
      m_distance += m_largeur;
    }
    if(m_distance < -m_largeur/2){
      m_distance += m_largeur;
    }
    if(m_distance > m_largeur/2){
      m_distance -= m_largeur;
    }
  }
}

void GpsFramework::setAB(){
  m_ab_x = m_pointB.m_x - m_pointA.m_x;
  m_ab_y = m_pointB.m_y - m_pointA.m_y;
  m_a = -(m_pointB.m_y - m_pointA.m_y);
  m_b = m_pointB.m_x - m_pointA.m_x;
  m_c = -m_a * m_pointA.m_x - m_b *  m_pointA.m_y;
  m_sqrt_m_a_m_b = sqrt(m_a*m_a + m_b*m_b);
  INFO("yb  " << m_pointB.m_y << " ya " << m_pointA.m_y << " xb " << m_pointB.m_x << " xa " << m_pointA.m_x);
  INFO(m_a << "*x + " << m_b << "*y + " << m_c << " = 0; " << m_sqrt_m_a_m_b);
}

void GpsFramework::changeStatusTrajectoire(StatusTrajectoire status){
  m_status = status;
  callbackStatusTrajectoire(status);
}


void test_trajectoire(){
  GpsFramework f;
  GpsPoint pointA;
  pointA.m_x = 1;
  pointA.m_y = 3;
  GpsPoint pointB;
  pointB.m_x = 5;
  pointB.m_y = 1;
  
  f.savePointA(pointA);
  f.savePointB(pointB);

  GpsPoint point;
  point.m_x = 3;
  point.m_y = 3;
  f.onNewPoint(point);
}

