/*********************************************************************
  this sketch serves to handle the bluetooth, screen, button input, RTC and 
  servo control for a fish auto feeder.
  Kealym Bromley 05/04/2022
*********************************************************************/
#include "MenuDisplay.h"

#include <SPI.h>
#include <Wire.h>
#include "Button2.h"

//Button Def
#define BTN_SELECT_UP 34
#define BTN_SELECT_DOWN 35
#define BTN_ENTER 14
#define BTN_BACK 12

Button2 btnBack;
Button2 btnEnter;
Button2 btnUp;
Button2 btnDown;

//Menu object for interacting with the display
MenuDisplay menuDisplay;

void setup (){
  //Init serial for debugging
  Serial.begin(115200);
  
  //wait for serial to start
  delay(200);

  menuDisplay.initValues();
  
  //Setup buttons
  btnBack.begin(BTN_BACK);
  btnEnter.begin(BTN_ENTER);
  btnDown.begin(BTN_SELECT_DOWN);
  btnUp.begin(BTN_SELECT_UP);

  btnBack.setTapHandler(btnPressed);
  btnEnter.setTapHandler(btnPressed);
  btnDown.setTapHandler(btnPressed);
  btnUp.setTapHandler(btnPressed);
}
void loop()
{
    menuDisplay.clearDisplay();
    
    btnBack.loop();
    btnEnter.loop();
    btnUp.loop();
    btnDown.loop();

    menuDisplay.drawMenu();
    delay(50); //refresh time
}

void btnPressed (Button2& btn) {
   if(btn == btnBack){
      menuDisplay.backPressed();
   }else if(btn == btnEnter){
      menuDisplay.enterPressed();
   }else if(btn == btnUp){
      menuDisplay.upPressed();
   }else if(btn == btnDown){
      menuDisplay.downPressed();
   } 
}
