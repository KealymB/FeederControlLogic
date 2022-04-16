#ifndef MENUDISPLAY_H
#define MENUDISPLAY_H
#include <Arduino.h>

class MenuDisplay {
  private:
    int selectedSetting;
    uint8_t hoveredSetting;
    char *Days[];
    uint8_t feedQuantity;
    bool feedTimes[];
    uint8_t selectedTime;
    bool feedDays[];
    uint8_t selectedDay;
    uint8_t selectedScreen;
    uint8_t selectedItem;
    uint8_t Error;

    void mainMenu(void);
    void feedMenu(void);
    void timeMenu(void);
    void dayMenu(void);
    void settingsMenu(void);
    void nextFeed(void);
    
  public:
    MenuDisplay(void);
    void initValues(void);
    void clearDisplay(void);
    void drawMenu(void);
    void writeStr(char* str, int x, int y, uint8_t color = 1 );
    void setError(uint8_t error);
    
    void enterPressed(void);
    void backPressed(void);
    void upPressed(void);
    void downPressed(void);

    bool shouldFeed(void);

    uint8_t getHandfuls(void);
};
#endif
