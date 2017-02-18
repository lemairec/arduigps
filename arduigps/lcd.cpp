#include "lcd.h"
#include "arduigps.h"
#include "pins.h"
#include <LiquidCrystal.h>
#include <Arduino.h>

#define LINE_NB_SAT 0
#define LINE_VITESSE 1
#define LINE_LARGEUR 2
#define LINE_TIME 3
#define LINE_LAT_LONG 4

LiquidCrystal lcd(PIN_LCD_1, PIN_LCD_2, PIN_LCD_3, PIN_LCD_4, PIN_LCD_5, PIN_LCD_6);
bool lcd_refresh = false;

LcdModule::LcdModule(){
}

void LcdModule::updateSat(int fix, int nbSat, double precision){
  m_fix = fix;
  m_nbSat = nbSat;
  m_precision = precision;
  m_update = true;
}

void LcdModule::updateLatLon(double latitude, double longitude){
  m_latitude = latitude;
  m_longitude = longitude;
}
void LcdModule::updateDistance(double distance){
  m_distance = distance;
}

void LcdModule::updateTime(double time){
  m_time = time;
}

void LcdModule::updateVitesse(double vitesse){
  m_vitesse = vitesse;
}

void LcdModule::updateLargeur(double largeur){
  m_largeur = largeur;
}
  
void LcdModule::updateStatusTrajectoire(StatusTrajectoire s){
  m_statusTrajectoire = s;
}
  
void LcdModule::init(){
  lcd.begin(LCD_WIDTH, LCD_HEIGHT);              // start the library
  lcd.setCursor(0,0);
  lcd.print("Push the buttons"); // print a simple message
}

void LcdModule::printEvent(char * s){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(s);
  delay(1000);
}

int LcdModule::readButtons(){
  int adc_key_in = analogRead(0);      // read the value from the sensor 
  // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
  // we add approx 50 to those values and check to see if we are close
  if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
  // For V1.1 us this threshold
  if (adc_key_in < 50)   return btnRIGHT;  
  if (adc_key_in < 250)  return btnUP; 
  if (adc_key_in < 450)  return btnDOWN; 
  if (adc_key_in < 650)  return btnLEFT; 
  if (adc_key_in < 850)  return btnSELECT;  

  // For V1.0 comment the other threshold and use the one below:
  /*
  if (adc_key_in < 50)   return btnRIGHT;  
  if (adc_key_in < 195)  return btnUP; 
  if (adc_key_in < 380)  return btnDOWN; 
  if (adc_key_in < 555)  return btnLEFT; 
  if (adc_key_in < 790)  return btnSELECT;
  */
  return btnNONE;  // when all others fail, return this...
}

void LcdModule::readKeyAndUpdate(){
  int button = readButtons();
  if(button != m_lastButton){
    m_lastButton = button;
    m_update = true;
    if(button == btnDOWN){
      --m_line;
      if(m_line < 0){
        m_line = 0;
      }
    } else if(button == btnUP){
      ++m_line;
      if(m_line > 10){
        m_line = 10;
      }
    } else if(button == btnLEFT){
      if(m_line == LINE_LARGEUR){
        largueurDecrease();
      } else {
        savePointA();
      }
    } else if(button == btnRIGHT){
      if(m_line == LINE_LARGEUR){
        largueurIncrease();
      } else {
        savePointB();
      }
    }
    if(button != btnNONE){
      delay(200);
    }
  }
  if(m_update){
    m_update = false;
    printLcd();
  }
  
}

void LcdModule::printLcd(){
  //lcd.clear();
  lcd.setCursor(0,0);
  if(m_line == LINE_NB_SAT){
    if(m_nbSat < 10){
      lcd.print("nbSat  ");
    } else {
      lcd.print("nbSat ");
    }
    lcd.print(m_nbSat);
    if(m_fix== 1){
      lcd.print("    gps ");
    } else if(m_fix== 2){
      lcd.print("   Dgps ");
    } else {
      lcd.print("  no gps");
    }
  } else if(m_line == LINE_LARGEUR){
    lcd.print("largeur ");
    lcd.print(m_largeur);
    lcd.print("    ");
  } else if(m_line == LINE_VITESSE){
    lcd.print("vit ");
    lcd.print(m_vitesse);
    lcd.print(" km/h        ");
  } else if(m_line == LINE_TIME){
    lcd.print("time ");
    lcd.print(m_time);
    lcd.print("    ");
  } else if(m_line == LINE_LAT_LONG){
    char s1[40];
    dtostrf(m_latitude, 4, 8, s1);
    char s2[40];
    dtostrf(m_longitude, 4, 8, s2);
    for(size_t i = 0; i < 8; ++i){
        lcd.print(s1[i]);
    }
    lcd.print(" ");
    for(size_t i = 0; i < 7; ++i){
        lcd.print(s2[i]);
    }
  } else if(m_line == 10){
    lcd.print("version 0.1     ");
  }else {
    lcd.print(m_line);
    lcd.print("                ");
  }

  /*m_distance += 0.20;
  if(m_distance > GPS_LINE_WIDTH){
    m_distance = -GPS_LINE_WIDTH/2;
  }*/
  drawDistanceAB();
}

void LcdModule::drawDistanceAB(){
  if(m_statusTrajectoire == NoPoint){
    lcd.setCursor(0,1);
    lcd.print("no point saved");
  } else if(m_statusTrajectoire == PointA){
    lcd.setCursor(0,1);
    lcd.print("d pA : ");
    lcd.print(m_distance);
    lcd.print(" m ");
  } else {
  lcd.setCursor(0,1);
  if(m_distance == 0){
    lcd.print(" <=  0.00 m =>");
  } else if(m_distance > 0){
    lcd.print(" <= ");
    if(m_distance<10.0){
      lcd.print(" ");
    }
    lcd.print(m_distance);
    lcd.print(" m    ");
  } else {
    lcd.print("    ");
    if(-m_distance<10.0){
      lcd.print(" ");
    }
    lcd.print(-m_distance);
    lcd.print(" m =>  ");
  }
  }
}

void LcdModule::drawDistanceA(){
  lcd.setCursor(0,1);
  m_distance = 1000;
  lcd.print("dist ");
  lcd.print(m_distance);
  lcd.print(" m");
}



void print_lcd(int lcd_key){

  lcd.setCursor(9,1);            // move cursor to second line "1" and 9 spaces over
  lcd.print(millis()/1000);      // display seconds elapsed since power-up


  lcd.setCursor(0,1);            // move to the begining of the second line
  switch (lcd_key)               // depending on which button was pushed, we perform an action
  {
   case btnRIGHT:
     {
     lcd.print("RIGHT ");
     break;
     }
   case btnLEFT:
     {
     lcd.print("LEFT   ");
     break;
     }
   case btnUP:
     {
     lcd.print("UP    ");
     break;
     }
   case btnDOWN:
     {
     lcd.print("DOWN  ");
     break;
     }
   case btnSELECT:
     {
     lcd.print("SELECT");
     break;
     }
     case btnNONE:
     {
     lcd.print("NONE  ");
     break;
     }
  }
}

