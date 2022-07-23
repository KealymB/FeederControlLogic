/*********************************************************************
  this sketch serves to handle the bluetooth, screen, button input, RTC and 
  servo control for a fish auto feeder.
  Kealym Bromley 05/04/2022
*********************************************************************/
#include "MenuDisplay.h"
#include "DateTime.h"

#include <SPI.h>
#include <Wire.h>
#include "Button2.h"
#include <EEPROM.h>

//Button Def
#define BTN_SELECT_DOWN  6
#define BTN_SELECT_UP    7
#define BTN_BACK         8
#define BTN_ENTER        9

//Stepper params
#define DIR_PIN          3 // Direction
#define STEP_PIN         4 // Step
#define EN_PIN           5 // Enable

//Hall effect sensor
#define HALL_PIN         10

Button2 btnBack;
Button2 btnEnter;
Button2 btnUp;
Button2 btnDown;

//Menu object for interacting with the display
MenuDisplay menuDisplay;

//class variables
int feedState = -1; // -1: waiting for feedtime, 0: currently feeding, 1: finsihed feeding.
bool positionKnown = true; //every time there is a power loss, position should be unknown. Need to home the stepper...
int hallSensorReading = HIGH; //Stores reading of halleffect sensor

void setup (){
  //Init serial for debugging
  Serial.begin(115200);
  
  //wait for serial to start
  delay(200);

  Serial.write("Starting setup");

  //Stepper setup 
  pinMode(EN_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  digitalWrite(EN_PIN, LOW);      // Enable stepper
  
  //Get feedstate
  EEPROM.get(0, feedState); //if not there needs to return -1

  //init menu
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

  pinMode(HALL_PIN, INPUT);    //sets the hall effect sensor pin as input

  Serial.write("Setup finished");
  
  if(not positionKnown){
    menuDisplay.setError(2);
  }
}
void loop()
{
    btnBack.loop();
    btnEnter.loop();
    btnUp.loop();
    btnDown.loop();

    if(positionKnown){
      feedHandler();
    }else{
      homeStepper();
    }

    menuDisplay.drawMenu();
    delay(20); //refresh time
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
  // rotate stepper 90*number of hands degrees
  delay(5000);// wait for move to finish
  feedState = 1;
  // save feedState
  EEPROM.put(0, feedState);
}

void homeStepper () {
  Serial.write("Hall reading: ");
  Serial.print(hallSensorReading, DEC);
  Serial.write("\n");

  // move 1/8 steps
  while(hallSensorReading != LOW){
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(500);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(500);
    hallSensorReading = digitalRead(HALL_PIN);
  }
  
  if(hallSensorReading == LOW){
    positionKnown = true;
    menuDisplay.setError(0);
  }
}
