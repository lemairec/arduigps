#ifndef AGRI_GPS_H
#define AGRI_GPS_H

#include "gps.h"

void callbackGpsGpGGA(GGAFrame & ggaFrame);
void callbackStatusTrajectoire(StatusTrajectoire & s);
void callbackGpsError();
void savePointA();
void savePointB();
void largueurDecrease();
void largueurIncrease();
void print_message(char *c);

#endif
