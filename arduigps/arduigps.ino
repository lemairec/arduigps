#include "logging.h"
#include "gps.h"
#include "lcd.h"
#include "framework.h"


GpsFramework gpsFramework;
LcdModule lcdModule;

void callbackGpsGpGGA(GGAFrame & ggaFrame){
  //gpsEvent.debug();
  gpsFramework.onNewPoint(ggaFrame);
  lcdModule.updateSat(ggaFrame.m_fix, ggaFrame.m_nbrSat, ggaFrame.m_precision);
  lcdModule.updateLatLon(ggaFrame.m_latitude, ggaFrame.m_longitude);
  lcdModule.updateDistance(gpsFramework.m_distance);
  lcdModule.updateTime(ggaFrame.m_time);
  lcdModule.updateVitesse(gpsFramework.m_vitesse);
  lcdModule.readKeyAndUpdate();
  //INFO("fix " << fixggaFrame.m_fix<< " nbst " << ggaFrame.m_nbrSat << " prec " << ggaFrame.m_precision);
  
}

void callbackStatusTrajectoire(StatusTrajectoire & s){
  lcdModule.updateStatusTrajectoire(s);
}

void print_message(char *c){
  lcdModule.printEvent(c);
}


void savePointA(){
  gpsFramework.savePointA(gpsFramework.m_gpsModule.m_lastGGAEvent);
}
void savePointB(){
  gpsFramework.savePointB(gpsFramework.m_gpsModule.m_lastGGAEvent);
}

void largueurDecrease(){
  gpsFramework.m_largeur -= LARGEUR_PAS;
  lcdModule.updateLargeur(gpsFramework.m_largeur);
}
void largueurIncrease(){
  gpsFramework.m_largeur += LARGEUR_PAS;
  lcdModule.updateLargeur(gpsFramework.m_largeur);
}

void setup(){
  Serial.begin(9600);
  Serial.println("Listening...");
  lcdModule.init();
#ifdef ENABLE_TEST
  lcdModule.printEvent("test");
  test_trajectoire();
#endif
  lcdModule.printEvent("begin");
  gpsFramework.m_gpsModule.init();

  
}

void loop(){
  //INFO(freeRam());
  gpsFramework.m_gpsModule.readNextFrame();
  lcdModule.readKeyAndUpdate();
}

