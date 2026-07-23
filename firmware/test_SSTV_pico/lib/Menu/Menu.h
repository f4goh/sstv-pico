/* 
 * File:   Menu.h
 * 
 * Created on 22 avril 2022, 17:11
 */

#ifndef MENU_H
#define MENU_H
#include <Arduino.h>
#include <console.h>
#include "hardware/watchdog.h"
#include <EEPROM.h>
#include <Wire.h>
#include "Sstv.h"

//#define MENU_PIN 6  //time out
#define MENU_TIMEOUT_MS 30000UL   // 30 secondes

 
typedef enum{
    M80,M60,M40,M30,M20,FREQ     
}bands;

typedef struct {
  uint32_t freq;
  int32_t offset;
}freqbands;
    

typedef struct {
  uint32_t freq;
  char call[10];
  int32_t offset;
  uint8_t minute;    // Durée en minutes
  char locator[7];
  SSTVMode_t sstv;
  uint8_t mode;
  uint8_t posx;
  uint8_t posy;
  uint8_t color;
  bool manAuto;
  bands band;
} config;



class Menu {
    
public:
    Menu();
    Menu(const Menu& orig);
    virtual ~Menu();
    
    void run();
    void setup();    

 private:
    // Méthodes associées aux commandes
    static void _call_(ArgList& L, Stream& S);
    static void _freq_(ArgList& L, Stream& S);
    static void _minute_(ArgList& L, Stream& S);
    static void _offset_(ArgList& L, Stream& S);
    static void _mode_(ArgList& L, Stream& S);
    static void _band_(ArgList& L, Stream& S);
    static void _show_(ArgList& L, Stream& S);
    static void _raz_(ArgList& L, Stream& S);
    static void _save_(ArgList& L, Stream& S);
    static void _restart_(ArgList& L, Stream& S);
    static void _exit_(ArgList& L, Stream& S);
    static void _help_(ArgList& L, Stream& S);
    static void _unknown(String& L, Stream& S);
    static void _scan_(ArgList& L, Stream& S);
    static void _loc_(ArgList& L, Stream& S);
    static void _posx_(ArgList& L, Stream& S);
    static void _posy_(ArgList& L, Stream& S);
    static void _color_(ArgList& L, Stream& S);
    static void _manAuto_(ArgList& L, Stream& S);
    //void displayRtc();
    
    bool exitFlag;
    Console *con;
    config cfg;
    static Menu* anchor;        
    bool acceptCmd(String cmd, int longMin, int longMax);
    void scanI2C();
    const char* colorToString(uint8_t color);
    static const char* modeStr[11];
    static const char* bandStr[6];
    freqbands tabfreq[5];
    
};

#endif /* MENU_H */

