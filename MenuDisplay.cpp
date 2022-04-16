#include "MenuDisplay.h"
#include "DateTime.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Preferences.h>

//screen def
#define i2c_Address 0x3c 
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//global error var
uint8_t Error = 0;

//Settings variables
int selectedSetting = -1;
uint8_t hoveredSetting = 0;
uint8_t selectedWDay = 0;
uint8_t selectedHour = 0;
uint8_t selectedMin = 0;

//Feed variables
uint8_t feedQuantity = 0;

//Time variables
bool timesFeed[] = {false, false, false};
uint8_t selectedTime = 0;

//Day variables
bool daysFeed[] = {false, false, false, false, false, false, false};
int selectedDay = 0;

//Menu variables
uint8_t selectedScreen = 0;
uint8_t selectedItem = 0;

//Date time object for interacting with RTC
DateTime dateTime;

//reading and writing to eeprom
Preferences prefs;

MenuDisplay::MenuDisplay() {
}

void MenuDisplay::initValues() {
  delay(250); // wait for the OLED to power up
  display.begin(i2c_Address, true);  
  display.clearDisplay();
  
  //set up RTC read
  dateTime.init();
  
  //Read values stored in prefrences
  prefs.begin("my-app", false); 
  
  prefs.getBytes("feedTimes", &timesFeed, 4*sizeof(bool));
  prefs.getBytes("feedDays", &daysFeed, 7*sizeof(bool));
  feedQuantity = (uint8_t) prefs.getUChar("quantity", 0);
  
  prefs.end();
}

void MenuDisplay::clearDisplay() {
  display.clearDisplay();
}

void MenuDisplay::drawMenu() {
    //draws border line
    display.drawRoundRect(0, 0, display.width(), display.height(), 3, SH110X_WHITE);
    
    //clock draw
    display.drawRoundRect(87, 4, 37, 13, 3, SH110X_WHITE); // clock outline

    if(!dateTime.readDateTime()){
      setError(1);
      Serial.println("Failed to read date Time");
    }else{
      setError(0);
    }
    
    char *day = (char*) malloc(4);
    writeStr(dateTime.getDayString(day), 6, 7);
    free(day);

    char *_time = (char*) malloc(20);
    writeStr(dateTime.getTimeString(_time), 91, 7, 1);
    free(_time);
    
    //Menu logic
    if (selectedScreen == 0){
      mainMenu();
    }else if(selectedScreen == 1){
      feedMenu();
    }else if(selectedScreen == 2){
      timeMenu();
    }else if(selectedScreen == 3){
      dayMenu();
    }else if(selectedScreen == 4){
      settingsMenu();
    }
    
    display.display();
}

//selectedScreen == 0
void MenuDisplay::mainMenu(){
  //next feeding time display if no errors
  if(Error == 0){
    nextFeed();
  }else{
    //error display
    if(Error == 1){
      writeStr("Error reading RTC", 5, 28, 1);
    }
  }
  
  // feed, time and days outlines
  display.drawRoundRect(5, 47, 36, 13, 3, SH110X_WHITE);
  display.drawRoundRect(46, 47, 35, 13, 3, SH110X_WHITE);
  display.drawRoundRect(86, 47, 36, 13, 3, SH110X_WHITE);
  
  if(selectedItem == 0){
    display.drawRoundRect(4, 46, 38, 15, 4, SH110X_WHITE);
  }
  writeStr("FEED", 12, 50, 1);
  
  if(selectedItem == 1){
    display.drawRoundRect(45, 46, 37, 15, 4, SH110X_WHITE);
  }
  writeStr("TIME", 53, 50, 1);
  
  if(selectedItem == 2){
    display.drawRoundRect(85, 46, 38, 15, 4, SH110X_WHITE);
  }
  writeStr("DAYS", 93, 50, 1);

  //hover clock if selected
  if(selectedItem == 3){
    display.drawRoundRect(86, 3, 39, 15, 4, SH110X_WHITE);
  }
}

//selectedScreen == 1
void MenuDisplay::feedMenu(){
  //clock
  display.fillRoundRect(29, 47, 70, 13, 3, SH110X_WHITE);
  writeStr("Handfuls", 40, 50, 0);
  // feed counter
  display.drawRoundRect(18, 19, 92, 26, 3, SH110X_WHITE);
  for(uint8_t i = 0; i < 5; i++){
    display.drawRoundRect(21+i*18, 22, 14, 20, 3, SH110X_WHITE);
  }

  for(uint8_t i = 0; i < feedQuantity+1; i++){
    display.fillRoundRect(21+i*18, 22, 14, 20, 3, SH110X_WHITE);
  }
}

//selectedScreen == 2
void MenuDisplay::timeMenu(){
  //bottom label
  display.fillRoundRect(29, 47, 70, 13, 3, SH110X_WHITE);
  writeStr("Feed Times", 35, 50, 0);

  if(timesFeed[0]){
    display.fillRoundRect(5, 20, 36, 21, 3, SH110X_WHITE);
    writeStr("7 AM", 11, 27, 0);
  }else{
      display.drawRoundRect(5, 20, 36, 21, 3, SH110X_WHITE);
      writeStr("7 AM", 11, 27, 1);
  }
  if(selectedTime == 0){
    display.drawRoundRect(4, 19, 38, 23, 4, SH110X_WHITE);
  }

  if(timesFeed[1]){
    display.fillRoundRect(46, 20, 36, 21, 3, SH110X_WHITE);
    writeStr("12 PM", 49, 27, 0);
  }else{
      display.drawRoundRect(46, 20, 36, 21, 3, SH110X_WHITE);
      writeStr("12 PM", 49, 27, 1);
  }
  if(selectedTime == 1){
    display.drawRoundRect(45, 19, 38, 23, 4, SH110X_WHITE);
  }

  if(timesFeed[2]){
    display.fillRoundRect(87, 20, 36, 21, 3, SH110X_WHITE);
    writeStr("6 PM", 93, 27, 0);
  }else{
      display.drawRoundRect(87, 20, 36, 21, 3, SH110X_WHITE);
      writeStr("6 PM", 93, 27, 1);
  }
  if(selectedTime == 2){
    display.drawRoundRect(86, 19, 38, 23, 4, SH110X_WHITE);
  }
}

//selectedScreen == 3
void MenuDisplay::dayMenu(){
  //clock
  display.fillRoundRect(29, 47, 70, 13, 3, SH110X_WHITE);
  writeStr("Feed Days", 35, 50, 0);

  if(daysFeed[0]){
    display.fillRoundRect(14, 20, 22, 11, 3, SH110X_WHITE);
    writeStr("S", 22, 22, 0);
  }else{
    display.drawRoundRect(14, 20, 22, 11, 3, SH110X_WHITE);
    writeStr("S", 22, 22, 1);
  }

  if(selectedDay == 0){
    display.drawRoundRect(13, 19, 24, 13, 4, SH110X_WHITE);
  }

  if(daysFeed[1]){
    display.fillRoundRect(40, 20, 22, 11, 3, SH110X_WHITE);
    writeStr("M", 48, 22, 0);
  }else{
    display.drawRoundRect(40, 20, 22, 11, 3, SH110X_WHITE);
    writeStr("M", 48, 22, 1);
  }

  if(selectedDay == 1){
    display.drawRoundRect(39, 19, 24, 13, 4, SH110X_WHITE);
  }

  if(daysFeed[2]){
    display.fillRoundRect(66, 20, 22, 11, 3, SH110X_WHITE);
    writeStr("T", 74, 22, 0);
  }else{
    display.drawRoundRect(66, 20, 22, 11, 3, SH110X_WHITE);
    writeStr("T", 74, 22, 1);
  }

  if(selectedDay == 2){
    display.drawRoundRect(65, 19, 24, 13, 4, SH110X_WHITE);
  }

  if(daysFeed[3]){
    display.fillRoundRect(92, 20, 22, 11, 3, SH110X_WHITE);
    writeStr("W", 100, 22, 0);
  }else{
    display.drawRoundRect(92, 20, 22, 11, 3, SH110X_WHITE);
    writeStr("W", 100, 22, 1);
  }

  if(selectedDay == 3){
    display.drawRoundRect(91, 19, 24, 13, 4, SH110X_WHITE);
  }

  if(daysFeed[4]){
    display.fillRoundRect(28, 33, 22, 11, 3, SH110X_WHITE);
    writeStr("T", 36, 35, 0);
  }else{
    display.drawRoundRect(28, 33, 22, 11, 3, SH110X_WHITE);
    writeStr("T", 36, 35, 1);
  }

  if(selectedDay == 4){
    display.drawRoundRect(27, 32, 24, 13, 4, SH110X_WHITE);
  }

  if(daysFeed[5]){
    display.fillRoundRect(54, 33, 22, 11, 3, SH110X_WHITE);
    writeStr("F", 62, 35, 0);
  }else{
    display.drawRoundRect(54, 33, 22, 11, 3, SH110X_WHITE);
    writeStr("F", 62, 35, 1);
  }
  if(selectedDay == 5){
    display.drawRoundRect(53, 32, 24, 13, 4, SH110X_WHITE);
  }

  if(daysFeed[6]){
    display.fillRoundRect(80, 33, 22, 11, 3, SH110X_WHITE);
    writeStr("S", 88, 35, 0);
  }else{
    display.drawRoundRect(80, 33, 22, 11, 3, SH110X_WHITE);
    writeStr("S", 88, 35, 1);
  }
  if(selectedDay == 6){
    display.drawRoundRect(79, 32, 24, 13, 4, SH110X_WHITE);
  }
}

//selectedScreen 4
void MenuDisplay::settingsMenu(){
  //clock
  display.fillRoundRect(29, 47, 70, 13, 3, SH110X_WHITE);
  writeStr("Settings", 42, 50, 0);

  //string of current day of week 
  char* weekDayString = (char*) malloc(4);
  dateTime.getDayString(weekDayString, selectedWDay);

  if(selectedSetting == 0){
    display.fillRoundRect(5, 20, 36, 21, 3, SH110X_WHITE);
    writeStr(weekDayString, 14, 27, 0);
  }else{
    display.drawRoundRect(5, 20, 36, 21, 3, SH110X_WHITE);
    writeStr(weekDayString, 14, 27, 1);
  }
  free(weekDayString);
  if(hoveredSetting == 0){
    display.drawRoundRect(4, 19, 38, 23, 4, SH110X_WHITE);
  }

  char hourText [3];
  snprintf(hourText, sizeof(hourText), "%u", selectedHour);

  if(selectedSetting == 1){
    display.fillRoundRect(46, 20, 36, 21, 3, SH110X_WHITE);
    writeStr(hourText, 60, 27, 0);
  }else{
      display.drawRoundRect(46, 20, 36, 21, 3, SH110X_WHITE);
      writeStr(hourText, 60, 27, 1);
  }

  if(hoveredSetting == 1){
    display.drawRoundRect(45, 19, 38, 23, 4, SH110X_WHITE);
  }

  char minText [3];
  snprintf(minText, sizeof(minText), "%u", selectedMin);

  if(selectedSetting == 2){
    display.fillRoundRect(87, 20, 36, 21, 3, SH110X_WHITE);
    writeStr(minText, 100, 27, 0);
  }else{
      display.drawRoundRect(87, 20, 36, 21, 3, SH110X_WHITE);
      writeStr(minText, 100, 27, 1);
  }

  if(hoveredSetting == 2){
    display.drawRoundRect(86, 19, 38, 23, 4, SH110X_WHITE);
  }
}

//Display middle text
void MenuDisplay::nextFeed() {  
  int _hour = dateTime.getHour();
  int _wday = dateTime.getWeekDay();
  int nextFeed = -1;
  bool emptyTime = true;
  bool emptyDay = true;
  char *FeedStrings[] = {"07:00", "12:00", "18:00"};
  uint8_t FeedInts[] = {7, 12, 18};

  //determine errors
  for(uint8_t i = 0; i < 3; i++){
    if(timesFeed[i]){
      emptyTime = false;
    }
  }
  if(emptyTime){
     writeStr("No feeding times set", 5, 28, 1);
     return;
  }
  for(uint8_t i = 0; i < 7; i++){
    if(feedDays[i]){
      emptyDay = false;
    }
  }
  if(emptyDay){
     writeStr("No feeding days set", 5, 28, 1);
     return;
  }
  
  for(uint8_t i = 0; i < 3; i++){
    if(_hour < FeedInts[i] && timesFeed[i]){
      nextFeed = i;
      break;
    }
  }
  
  char* feedString = (char *) malloc(20);
  
  if(nextFeed != -1){
    strcpy(feedString, "Feeding at ");
    strcat(feedString, FeedStrings[nextFeed]);
    
    writeStr(feedString, 5, 28, 1);
  }else{
    for(uint8_t i = 0; i < 3; i++){
      if(feedTimes[i] != -1){
        nextFeed = i;
        break;
      }
    }
    
    strcpy(feedString, "Feed tomorrow ");
    strcat(feedString, FeedStrings[nextFeed]);
    
    writeStr(feedString, 5, 28, 1);
  }
  free(feedString);
}

void MenuDisplay::writeStr(char* str, int x, int y, uint8_t color) {
  // writes a string to the display at x, y
  display.setTextSize(1);
  display.setTextColor(color);
  display.setCursor(x, y);

  display.write(str);
}

void MenuDisplay::setError(uint8_t error){
  Error = error;
}

void MenuDisplay::enterPressed(void){
  Serial.write("enterPressed, called\n");

  //Main menu action
  if(selectedScreen == 0){
    //if settings menu, fetch the current hour and min
    if(selectedItem == 3){
        //set day
        selectedWDay = dateTime.getWeekDay();
        selectedHour = dateTime.getHour();
        selectedMin = dateTime.getMin();
    }
    selectedScreen = selectedItem + 1;
    Serial.write("TEst\n");
    return;
  }
  
  //Setting menu action
  if(selectedScreen == 4){
    if(selectedSetting == -1){
      selectedSetting = hoveredSetting;
    } else{
      if(selectedSetting == 0){
        dateTime.setDay(selectedWDay);
      }else if(selectedSetting == 1){
        dateTime.setHour(selectedHour);
      }else if(selectedSetting == 2){
        dateTime.setMin(selectedMin);
      }
      selectedSetting = -1;
    }
    return;
  }

  //Day menu action
  if(selectedScreen == 3){
    daysFeed[selectedDay] = !daysFeed[selectedDay];
    return;
  }

  //Time menu action
  if(selectedScreen == 2){
    Serial.write("time menu action, called\n");
    timesFeed[selectedTime] = !timesFeed[selectedTime];
    return;
  }
}

void MenuDisplay::downPressed(void){
  Serial.write("downPressed, called\n");
  //Main menu action
  if(selectedScreen == 0){
    if(selectedItem>0){
        selectedItem -= 1;
      }else{
        selectedItem = 3;
    }
    return;
  }
  //Feeder menu action
  if(selectedScreen == 1){
    if(feedQuantity>0){
        feedQuantity -= 1;
      }else{
        feedQuantity = 4;
    }
    return;
  }

  //Time menu action
  if(selectedScreen == 2){
    if(selectedTime>0){
        selectedTime -= 1;
      }else{
        selectedTime = 2;
    }
    return;
  }

  //Day menu action
  if(selectedScreen == 3){
    if(selectedDay>0){
        selectedDay -= 1;
      }else{
        selectedDay = 6;
    }
    return;
  }

  //Setting menu action
  if(selectedScreen == 4){
    if(selectedSetting == -1){
      if(hoveredSetting>0){
        hoveredSetting --;
      }else{
        hoveredSetting = 2;
      }
    }else if(selectedSetting == 0){
      if(selectedWDay>0){
        selectedWDay --;
      }else{
        selectedWDay = 6;
      }
   }else if(selectedSetting == 1){
      if(selectedHour>0){
        selectedHour --;
      }else{
        selectedHour = 24;
      }
   }else if(selectedSetting == 2){
      if(selectedMin>0){
        selectedMin --;
      }else{
        selectedMin = 60;
      }
   }
    return;
  }
}
void MenuDisplay::upPressed(void){
  Serial.write("upPressed, called\n");
  
  //Main menu action
  if(selectedScreen == 0){
    if(selectedItem<3){
      selectedItem += 1;
    }else{
      selectedItem = 0;
    }
    return;
  }
  
  //Feeder menu action
  if(selectedScreen == 1){
    if(feedQuantity<4){
        feedQuantity += 1;
      }else{
        feedQuantity = 0;
    }
    return;
  }

  //Time menu action
  if(selectedScreen == 2){
    if(selectedTime<2){
        selectedTime += 1;
      }else{
        selectedTime = 0;
    }
    return;
  }

  //Day menu action
  if(selectedScreen == 3){
    if(selectedDay<6){
        selectedDay += 1;
      }else{
        selectedDay = 0;
    }
    return;
  }
  //settings menu action
  if(selectedScreen == 4){
   if(selectedSetting == -1){
    if(hoveredSetting<2){
      hoveredSetting ++;
    }else{
      hoveredSetting = 0;
    }
   }else if(selectedSetting == 0){
    if(selectedWDay<6){
      selectedWDay ++;
    }else{
      selectedWDay = 0;
    }
   }else if(selectedSetting == 1){
      if(selectedHour<24){
        selectedHour ++;
      }else{
        selectedHour = 0;
      }
   }else if(selectedSetting == 2){
      if(selectedMin<60){
        selectedMin ++;
      }else{
        selectedMin = 0;
      }
   }
   return;
  }
}
void MenuDisplay::backPressed(void){
  Serial.write("backpressed, called\n");
  selectedScreen = 0;
  selectedSetting = -1;

  // save all data
  prefs.begin("my-app", false); 
  
  prefs.putUChar("quantity", feedQuantity);
  prefs.putBytes("feedTimes", &timesFeed, 3*sizeof(bool));
  prefs.putBytes("feedDays", &daysFeed, 7*sizeof(bool));
  
  prefs.end();
}
