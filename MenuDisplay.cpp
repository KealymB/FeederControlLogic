#include "MenuDisplay.h"
#include "DateTime.h"
#include <Wire.h>
#include <EEPROM.h>
#include <U8g2lib.h>

//screen def
U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

//global error var
uint8_t Error = 0;
const uint8_t NO_ERR = 0;
const uint8_t RTC_ERR = 1;
const uint8_t HOMING_ERR = 2;

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
uint8_t FeedInts[] = {7, 12, 18};

//Day variables
bool daysFeed[] = {false, false, false, false, false, false, false};
int selectedDay = 0;

//Menu variables
uint8_t selectedScreen = 0;
uint8_t selectedItem = 0;

//Date time object for interacting with RTC
DateTime dateTime;

MenuDisplay::MenuDisplay() {
}

void MenuDisplay::initValues() {
  Serial.write("menu0");
  delay(250); // wait for the OLED to power up
  Serial.write("menu01");
  u8g2.begin();
  Serial.write("menu02");
  
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);

  Serial.write("menu1");
  
  //set up RTC read
  dateTime.init();
    
  //Read values stored in prefrences
  EEPROM.get(20, timesFeed);
  EEPROM.get(40, daysFeed);
  EEPROM.get(60, feedQuantity);
}

void MenuDisplay::drawMenu() {
    u8g2.firstPage();
    do {
      //draws border line
      u8g2.drawRFrame(0, 0, 128, 64, 3);
      
      //clock draw
      u8g2.drawRFrame(87, 4, 37, 13, 3); // clock outline
  
      if(!dateTime.readDateTime()){
        setError(RTC_ERR);
        Serial.println("Failed to read date Time");
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
    
    } while( u8g2.nextPage() );
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
    if(Error == 2){
      writeStr("Homing motor, please wait", 5, 28, 1);
    }
  }
  
  // feed, time and days outlines
  u8g2.drawRFrame(5, 47, 36, 13, 3);
  u8g2.drawRFrame(46, 47, 35, 13, 3);
  u8g2.drawRFrame(86, 47, 36, 13, 3);
  
  if(selectedItem == 0){
    u8g2.drawRFrame(4, 46, 38, 15, 4);
  }
  writeStr("FEED", 12, 50, 1);
  
  if(selectedItem == 1){
    u8g2.drawRFrame(45, 46, 37, 15, 4);
  }
  writeStr("TIME", 53, 50, 1);
  
  if(selectedItem == 2){
    u8g2.drawRFrame(85, 46, 38, 15, 4);
  }
  writeStr("DAYS", 93, 50, 1);

  //hover clock if selected
  if(selectedItem == 3){
    u8g2.drawRFrame(86, 3, 39, 15, 4);
  }
}

//selectedScreen == 1
void MenuDisplay::feedMenu(){
  //clock
  u8g2.drawRBox(29, 47, 70, 13, 3);
  writeStr("Handfuls", 40, 50, 0);
  // feed counter
  u8g2.drawRFrame(18, 19, 92, 26, 3);
  for(uint8_t i = 0; i < 5; i++){
    u8g2.drawRFrame(21+i*18, 22, 14, 20, 3);
  }

  for(uint8_t i = 0; i < feedQuantity+1; i++){
    u8g2.drawRBox(21+i*18, 22, 14, 20, 3);
  }
}

//selectedScreen == 2
void MenuDisplay::timeMenu(){
  //bottom label
  u8g2.drawRBox(29, 47, 70, 13, 3);
  writeStr("Feed Times", 35, 50, 0);

  if(timesFeed[0]){
    u8g2.drawRBox(5, 20, 36, 21, 3);
    writeStr("7 AM", 11, 27, 0);
  }else{
      u8g2.drawRFrame(5, 20, 36, 21, 3);
      writeStr("7 AM", 11, 27, 1);
  }
  if(selectedTime == 0){
    u8g2.drawRFrame(4, 19, 38, 23, 4);
  }

  if(timesFeed[1]){
    u8g2.drawRBox(46, 20, 36, 21, 3);
    writeStr("12 PM", 49, 27, 0);
  }else{
      u8g2.drawRFrame(46, 20, 36, 21, 3);
      writeStr("12 PM", 49, 27, 1);
  }
  if(selectedTime == 1){
    u8g2.drawRFrame(45, 19, 38, 23, 4);
  }

  if(timesFeed[2]){
   u8g2.drawRBox(87, 20, 36, 21, 3);
    writeStr("6 PM", 93, 27, 0);
  }else{
      u8g2.drawRFrame(87, 20, 36, 21, 3);
      writeStr("6 PM", 93, 27, 1);
  }
  if(selectedTime == 2){
    u8g2.drawRFrame(86, 19, 38, 23, 4);
  }
}

//selectedScreen == 3
void MenuDisplay::dayMenu(){
  //clock
  u8g2.drawRBox(29, 47, 70, 13, 3);
  writeStr("Feed Days", 35, 50, 0);

  if(daysFeed[0]){
    u8g2.drawRBox(14, 20, 22, 11, 3);
    writeStr("S", 22, 22, 0);
  }else{
    u8g2.drawRFrame(14, 20, 22, 11, 3);
    writeStr("S", 22, 22, 1);
  }

  if(selectedDay == 0){
    u8g2.drawRFrame(13, 19, 24, 13, 4);
  }

  if(daysFeed[1]){
    u8g2.drawRFrame(40, 20, 22, 11, 3);
    writeStr("M", 48, 22, 0);
  }else{
    u8g2.drawRFrame(40, 20, 22, 11, 3);
    writeStr("M", 48, 22, 1);
  }

  if(selectedDay == 1){
    u8g2.drawRFrame(39, 19, 24, 13, 4);
  }

  if(daysFeed[2]){
    u8g2.drawRBox(66, 20, 22, 11, 3);
    writeStr("T", 74, 22, 0);
  }else{
    u8g2.drawRFrame(66, 20, 22, 11, 3);
    writeStr("T", 74, 22, 1);
  }

  if(selectedDay == 2){
    u8g2.drawRFrame(65, 19, 24, 13, 4);
  }

  if(daysFeed[3]){
    u8g2.drawRBox(92, 20, 22, 11, 3);
    writeStr("W", 100, 22, 0);
  }else{
    u8g2.drawRFrame(92, 20, 22, 11, 3);
    writeStr("W", 100, 22, 1);
  }

  if(selectedDay == 3){
    u8g2.drawRFrame(91, 19, 24, 13, 4);
  }

  if(daysFeed[4]){
    u8g2.drawRBox(28, 33, 22, 11, 3);
    writeStr("T", 36, 35, 0);
  }else{
    u8g2.drawRFrame(28, 33, 22, 11, 3);
    writeStr("T", 36, 35, 1);
  }

  if(selectedDay == 4){
    u8g2.drawRFrame(27, 32, 24, 13, 4);
  }

  if(daysFeed[5]){
    u8g2.drawRBox(54, 33, 22, 11, 3);
    writeStr("F", 62, 35, 0);
  }else{
    u8g2.drawRFrame(54, 33, 22, 11, 3);
    writeStr("F", 62, 35, 1);
  }
  if(selectedDay == 5){
    u8g2.drawRFrame(53, 32, 24, 13, 4);
  }

  if(daysFeed[6]){
    u8g2.drawRBox(80, 33, 22, 11, 3);
    writeStr("S", 88, 35, 0);
  }else{
    u8g2.drawRFrame(80, 33, 22, 11, 3);
    writeStr("S", 88, 35, 1);
  }
  if(selectedDay == 6){
    u8g2.drawRFrame(79, 32, 24, 13, 4);
  }
}

//selectedScreen 4
void MenuDisplay::settingsMenu(){
  //clock
  u8g2.drawRBox(29, 47, 70, 13, 3);
  writeStr("Settings", 42, 50, 0);

  //string of current day of week 
  char* weekDayString = (char*) malloc(4);
  dateTime.getDayString(weekDayString, selectedWDay);

  if(selectedSetting == 0){
    u8g2.drawRBox(5, 20, 36, 21, 3);
    writeStr(weekDayString, 14, 27, 0);
  }else{
    u8g2.drawRFrame(5, 20, 36, 21, 3);
    writeStr(weekDayString, 14, 27, 1);
  }
  free(weekDayString);
  if(hoveredSetting == 0){
    u8g2.drawRFrame(4, 19, 38, 23, 4);
  }

  char hourText [3];
  snprintf(hourText, sizeof(hourText), "%u", selectedHour);

  if(selectedSetting == 1){
    u8g2.drawRFrame(46, 20, 36, 21, 3);
    writeStr(hourText, 60, 27, 0);
  }else{
      u8g2.drawRFrame(46, 20, 36, 21, 3);
      writeStr(hourText, 60, 27, 1);
  }

  if(hoveredSetting == 1){
   u8g2.drawRFrame(45, 19, 38, 23, 4);
  }

  char minText [3];
  snprintf(minText, sizeof(minText), "%u", selectedMin);

  if(selectedSetting == 2){
    u8g2.drawRBox(87, 20, 36, 21, 3);
    writeStr(minText, 100, 27, 0);
  }else{
      u8g2.drawRFrame(87, 20, 36, 21, 3);
      writeStr(minText, 100, 27, 1);
  }

  if(hoveredSetting == 2){
    u8g2.drawRFrame(86, 19, 38, 23, 4);
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

  u8g2.setDrawColor(color);
  u8g2.drawStr(x, y-1, str);
  u8g2.setDrawColor(1);
}

void MenuDisplay::setError(uint8_t error){
  Error = error;
}

void MenuDisplay::enterPressed(void){
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
    timesFeed[selectedTime] = !timesFeed[selectedTime];
    return;
  }
}

void MenuDisplay::downPressed(void){
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
  selectedScreen = 0;
  selectedSetting = -1;

  // save all data

  EEPROM.put(20, timesFeed);
  EEPROM.put(40, daysFeed);
  EEPROM.put(60, feedQuantity);
  
//  prefs.begin("my-app", false); 
//  
//  prefs.putUChar("quantity", feedQuantity);
//  prefs.putBytes("feedTimes", &timesFeed, 3*sizeof(bool));
//  prefs.putBytes("feedDays", &daysFeed, 7*sizeof(bool));
//  
//  prefs.end();
}
bool MenuDisplay::shouldFeed(void){
  if(daysFeed[dateTime.getWeekDay()]){ //check if we are going to feed today
    for(uint8_t i = 0; i < 3; i++){
      if(FeedInts[i] == dateTime.getHour() && timesFeed[i]){//check if we should be feeding now
        return true;
      }
    }  
  }
  return false;
}
uint8_t MenuDisplay::getHandfuls(void){
  return feedQuantity;
}
