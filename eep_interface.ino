
/*
eeprom driver
 */

#include <EEPROM.h>
#define COLOR_MEM_IDX 300
#define BRIGHT_MEM_IDX 301
#define EEP_UPDATE_INTERVAL 20

//#define SERIAL_LOG_EN

#ifdef SERIAL_LOG_EN
#define SERIALBEGIN(x) Serial.begin(9600)
#define SERIALPRNT(x) Serial.print(x)
#define SERIALPRNTLN(x) Serial.println(x)
#else
#define SERIALBEGIN(x)
#define SERIALPRNT(x)
#define SERIALPRNTLN(x)
#endif

typedef void (*reIniCallbk)(uint16_t, uint16_t);
uint8_t colorIdx_g;
uint8_t ledBrightness_g;
long long startime = 0;


void writeToEep(uint8_t colorIdx, uint8_t ledBrightness);
void reInitFromEep(reIniCallbk cbk);
void eepnit();
void requestEepWrite(uint8_t col, uint8_t br, bool isImmediate);
void task_EepHandler();



void writeToEep(uint8_t colorIdx, uint8_t ledBrightness) {
  static uint8_t old_col = 0, old_ledBr = 0;
  bool hasWrite = false;
  if (old_col != colorIdx) {
    EEPROM.write(COLOR_MEM_IDX, (uint8_t)colorIdx);
    hasWrite = true;
  }
  if (old_ledBr != ledBrightness) {
    EEPROM.write(BRIGHT_MEM_IDX, (uint8_t)ledBrightness);
    hasWrite = true;
  }
  if (hasWrite) {
    EEPROM.commit();
    SERIALPRNTLN("Written to eep!");
  } else {
    SERIALPRNTLN("No need to update eep!");
  }
  old_col = colorIdx;
  old_ledBr = ledBrightness;
}



void reInitFromEep(reIniCallbk cbk) {
  SERIALPRNTLN("inside reInitFromEep!!");
  uint8_t col = EEPROM.read(COLOR_MEM_IDX);
  uint8_t bgt = EEPROM.read(BRIGHT_MEM_IDX);
  if (col == 0xFF || bgt == 0xFF) 
  {
    SERIALPRNTLN("Eep never written!!");
    return;
  } 
  else 
  {
    SERIALPRNTLN("EepValsExists..");
    if (cbk != NULL)
     {
      SERIALPRNTLN("valid callback..");
      cbk(col, bgt);
    } 
    else
     {
      SERIALPRNTLN("Invalid cbk..");
      
    }
  }
}



void requestEepWrite(uint8_t col, uint8_t br, bool isImmediate) 
{
  if (isImmediate) 
  {
    writeToEep(col, br);
  } 
  else 
  {
    colorIdx_g = col;
    ledBrightness_g = br;
  }
}

void eepnit()
{
  EEPROM.begin(512);
}

void task_EepHandler() 
{
  static long long startime = millis();
  static uint8_t sectick = 0;
  if (millis() - startime >= 1000) 
  {
    sectick++;
    startime = millis();
  }

  if (sectick % EEP_UPDATE_INTERVAL == 0)
   {
    writeToEep(colorIdx_g, ledBrightness_g);
  }
}
