#ifndef DATETIME_H
#define DATETIME_H
#include <Arduino.h>

class DateTime {
  private:
    uint8_t _hour;
    uint8_t _min;
    uint8_t _sec;
    uint8_t _mday;
    uint8_t _mon;
    uint16_t _year;
    uint8_t _wday;  
    
  public:
    DateTime(void);
    void init();
    bool readDateTime();
    char* getDayString(char *day);
    char* getTimeString(char *_time);
    char* getDayString(char *day, int wDay);
    int getHour();
    int getMin();
    int getWeekDay();
    
    void setMin(uint8_t min);
    void setHour(uint8_t hour);
    void setDay(uint8_t day);
};
#endif
