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

//Stepper params
#define DIR 25
#define STEP 26
#define RPM 10
#define MICROSTEPS 1
#define MOTOR_STEPS 200

//Hall effect sensor
#define HALL_PIN 23

Button2 btnBack;
Button2 btnEnter;
Button2 btnUp;
Button2 btnDown;

//Object for stepper motor
DRV8825 stepper(MOTOR_STEPS, DIR, STEP);

//reading and writing to eeprom
Preferences prefrences;

//Menu object for interacting with the display
MenuDisplay menuDisplay;

//class variables
int feedState = -1; // -1: waiting for feedtime, 0: currently feeding, 1: finsihed feeding.
bool positionKnown = false; //every time there is a power loss, position should be unknown. Need to home the stepper...
int hallSensorReading = HIGH; //Stores reading of halleffect sensor

void setup (){
  //Init serial for debugging
  Serial.begin(115200);
  
  //wait for serial to start
  delay(200);
  
  stepper.begin(RPM, MOTOR_STEPS);

  //Get feedstate
  prefrences.begin("my-app", false); 
  feedState = prefrences.getInt("feedState", -1);
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

  pinMode(HALL_PIN, INPUT);    //sets the hall effect sensor pin as input

  if(not positionKnown){
    menuDisplay.setError(2);
  }
}
void loop()
{
    menuDisplay.clearDisplay();
    
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
  stepper.rotate((menuDisplay.getHandfuls()+1)*90); //turn number of handfuls
  delay(5000);// wait for move to finish
  feedState = 1;
  // save feedState
  prefrences.begin("my-app", false); 
  prefrences.putInt("feedState", feedState);
  prefrences.end();
}

void homeStepper () {
  Serial.write("Hall reading: ");
  Serial.print(hallSensorReading, DEC);
  Serial.write("\n");
    
  stepper.move(5*MICROSTEPS);    
  hallSensorReading = digitalRead(HALL_PIN);
  if(hallSensorReading == LOW){
    positionKnown = true;
    menuDisplay.setError(0);
  }
}
