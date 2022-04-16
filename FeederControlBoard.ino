/*********************************************************************
  this sketch serves to handle the bluetooth, screen, button input, RTC and 
  servo control for a fish auto feeder.
  Kealym Bromley 05/04/2022
*********************************************************************/
#include "MenuDisplay.h"
#include "DateTime.h"
#include "DRV8825.h"

#include <SPI.h>
#include <Wire.h>
#include "Button2.h"
#include <Preferences.h>

//Button Def
#define BTN_SELECT_UP 34
#define BTN_SELECT_DOWN 35
#define BTN_ENTER 14
#define BTN_BACK 12

Button2 btnBack;
Button2 btnEnter;
Button2 btnUp;
Button2 btnDown;


//Stepper params
#define DIR 25
#define STEP 26
#define RPM 10
#define MICROSTEPS 1 //need to double check
#define MOTOR_STEPS 200 //need to double check

//Object for stepper motor
DRV8825 stepper(MOTOR_STEPS, DIR, STEP);

//reading and writing to eeprom
Preferences prefrences;

//Menu object for interacting with the display
MenuDisplay menuDisplay;

int feedState = -1; // -1: waiting for feedtime, 0: currently feeding, 1: finsihed feeding.

void setup (){
  //Init serial for debugging
  Serial.begin(115200);
  
  //wait for serial to start
  delay(200);

  stepper.begin(RPM, MOTOR_STEPS);

  //Get feedstate
  prefrences.begin("my-app", false); 
  prefrences.getInt("feedState", feedState);
  prefrences.end();

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
    feedHandler();
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

void feedHandler () {
  //waiting for feed
  if(feedState == -1){  
    if(menuDisplay.shouldFeed()){ //check if we are going to feed today
      feedState = 0;
      feedFish();
      return;
    }
  }else if(feedState == 1){
    if(!menuDisplay.shouldFeed()){ //wait to reset after an hour? 
      Serial.write("Done feeding...\n");
      feedState = -1;
    }
    return;
  }
}

void feedFish () {
  //turns stepper number of times to feed. also changes feed state when finished feeding
  Serial.write("Feeding...\n");
  stepper.rotate((menuDisplay.getHandfuls()+1)*90); //turn number of handfuls
  delay(5000);// wait for move to finish
  feedState = 1;
  // save feedState
  prefrences.begin("my-app", false); 
  prefrences.putInt("feedState", feedState);
  prefrences.end();
}
