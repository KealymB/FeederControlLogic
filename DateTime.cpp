#include "DateTime.h"
#include <Wire.h>
#include <ErriezDS3231.h>
#include <SPI.h>

// Create RTC object
ErriezDS3231 rtc;

#define DATE_STRING_SHORT 3
// month names in flash
const char monthNames_P[] PROGMEM = "JanFebMarAprMayJunJulAugSepOctNovDec";
// Day of the week names in flash
const char dayNames_P[] PROGMEM= "SunMonTueWedThuFriSat";

int _hour;
int _min;
int _sec;
int _mday;
int _mon;
int _year;
int _wday;

DateTime::DateTime() {
}

void DateTime::init() {
  // Initialize I2C for RTC
  Wire.begin();
  Wire.setClock(100000);

  delay(200);
  
  // Initialize RTC
  while (!rtc.begin()) {
      Serial.println(F("RTC not found"));
      delay(3000);
  }

  // Set square wave out pin
  rtc.setSquareWave(SquareWaveDisable);
}
bool DateTime::readDateTime() {
    // Read date/time
    if (!rtc.getDateTime(&_hour, &_min, &_sec, &_mday, &_mon, &_year, &_wday)) {
        return false;
    }
    return true;
}
char* DateTime::getDayString(char *day) {
    // Print day week
    strncpy_P(day, &(dayNames_P[_wday * DATE_STRING_SHORT]), DATE_STRING_SHORT);
    day[DATE_STRING_SHORT] = '\0';

    return day;
}
char* DateTime::getDayString(char *day, int wDay) {
    // Print day week
    strncpy_P(day, &(dayNames_P[wDay * DATE_STRING_SHORT]), DATE_STRING_SHORT);
    day[DATE_STRING_SHORT] = '\0';

    return day;
}
char* DateTime::getTimeString(char *_time) {
    char hourString [4];
    char minString [4];

    if(_hour < 10){
      sprintf (hourString, "0%i:", _hour) ;
    }else{
      sprintf (hourString, "%i:", _hour) ;
    }

    if(_min < 10){
      sprintf (minString, "0%i", _min) ;
    }else{
      sprintf (minString, "%i", _min) ;
    }
       
    strcpy(_time, &hourString[0]);
    strcat(_time, &minString[0]);
    
    return _time;
}

int DateTime::getHour() {
    return _hour;
}

int DateTime::getMin() {
    return _min;
}

int DateTime::getWeekDay() {
    return _wday;
}

void DateTime::setHour(uint8_t hour) {
    rtc.setTime(hour, _min, _sec);
}

void DateTime::setMin(uint8_t min) {
    rtc.setTime(_hour, min, _sec);
}

void DateTime::setDay(uint8_t day) {
  rtc.setDateTime(_hour, _min, _sec,  _mday, _mon, _year,  day);
}
