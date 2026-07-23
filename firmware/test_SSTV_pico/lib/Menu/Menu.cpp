/* 
 * File:   Menu.cpp
 * 
 * Created on 22 avril 2022, 17:11
 */

#include "Menu.h"

const char* Menu::modeStr[11] = { "scottie1", "scottie2", "scottieDX", "martin1", "martin2", "wrasse", "robot36", "robot72", "pd50", "pd90", "mp73" };

const char* Menu::bandStr[6] = {"80m", "60m", "40m", "30m", "20m", "free frequency"};
 
Menu::Menu() :
exitFlag(false),
con(new Console())      
{
    anchor = this;
    //pinMode(MENU_PIN, INPUT_PULLUP); // GP6 avec résistance interne activée
    tabfreq[0] = {3720000, -150};
    tabfreq[1] = {5363000, -200};
    tabfreq[2] = {7035000, -300};
    tabfreq[3] = {10132000, -400};
    tabfreq[4] = {14227000, -550};
}

Menu::Menu(const Menu& orig) {
}

Menu::~Menu() {
    anchor = NULL;
}


void Menu::run() {
    exitFlag = false;

    unsigned long startTime = millis();
    unsigned long lastDisplay = 0;

    while ((millis() - startTime) < MENU_TIMEOUT_MS) {

        unsigned long elapsed = millis() - startTime;

        // Affichage toutes les secondes
        if ((elapsed - lastDisplay) >= 1000 || lastDisplay == 0) {
            lastDisplay = elapsed;

            unsigned long remaining = (MENU_TIMEOUT_MS - elapsed + 999) / 1000;

            Serial.print(F("Press 'm' within "));
            Serial.print(remaining);
            Serial.println(F(" seconds to enter configuration menu."));
        }

        if (Serial.available()) {
            char c = Serial.read();

            if (c == 'm') {
                EEPROM.begin(sizeof(config));
                EEPROM.get(0, cfg);

                Serial.println(F("Configuration menu"));
                Serial.println(F("Type 'help' for available commands."));

                while (!exitFlag) {
                    con->run();
                }

                Serial.println(F("\nConsole exit"));
                return;
            }
        }

        delay(10);
    }

    Serial.println(F("Menu timeout."));
}

/*
void Menu::run() {
    exitFlag = false;
    if (digitalRead(MENU_PIN) == 0) {
        EEPROM.begin(sizeof (config));
        EEPROM.get(0, cfg);
        Serial.println(F("help command for info"));
        while (exitFlag == false) {
            con->run();
        }
    }
    Serial.println(F("\nConsole exit"));
}
*/
void Menu::setup() {
 
    con->onCmd("call", _call_);
    con->onCmd("freq", _freq_);
    con->onCmd("minute",_minute_);
    con->onCmd("offset", _offset_);
    con->onCmd("show", _show_);
    con->onCmd("raz", _raz_);
    con->onCmd("restart", _restart_);
    con->onCmd("save", _save_);
    con->onCmd("exit", _exit_);
    con->onCmd("help", _help_);
    con->onCmd("scan", _scan_);
    con->onCmd("loc", _loc_);
    con->onCmd("mode", _mode_);
    con->onCmd("band", _band_);
    con->onCmd("posx", _posx_);    
    con->onCmd("posy", _posy_);
    con->onCmd("color", _color_);
    con->onCmd("auto", _manAuto_);
    
    con->onUnknown(_unknown);
    con->start();
    this->run();
}

void Menu::_exit_(ArgList& L, Stream& S) {
    anchor->exitFlag = true;

}


void Menu::_help_(ArgList& L, Stream& S) {
    S.println(F("Available commands"));
    S.println(F("Set transmission frequency              : freq 7040100"));
    S.println(F("Set offset value                        : offset -200"));
    S.println(F("Set callsign                            : call F4XYZ"));
    S.println(F("Set locator                             : loc JN07DV"));
    S.println(F("Set mode scottie1 scottie2 scottieDX    : mode martin1"));
    S.println(F("         martin1 martin2 wrasse robot36 "));
    S.println(F("         robot72 pd50 pd90 mp73"));
    S.println(F("Set band 80m,60m,40m,30m,20m,freq       : band 40m"));    
    S.println(F("Set modulo in minutes                   : minute 5"));
    S.println(F("Set manual 0 /automatic 1               : auto 1"));
    S.println(F("Set text overlay x                      : posx 15"));
    S.println(F("Set text overlay y                      : posx 15"));
    S.println(F("Set text color (0 to 9)                 : color 5"));
    S.println(F("  0: Black     1: White     2: Red       3: Orange"));
    S.println(F("  4: Yellow    5: Green     6: Cyan      7: Blue"));
    S.println(F("  8: Magenta   9: Gray"));
    S.println(F("Scan I2C bus                            : scan"));
    S.println(F("Show nmea frame  0 or 1                 : nmea 1"));
    S.println(F("Save current configuration to EEPROM    : save"));
    S.println(F("Display current configuration           : show"));
    S.println(F("Reset all parameters to default         : raz"));
    S.println(F("Restart RPI pico                        : restart"));
    S.println(F("Show this help message                  : help"));
    S.println(F("Exit menu                               : exit"));
}


bool Menu::acceptCmd(String cmd, int longMin, int longMax) {

    if (cmd.length() >= longMin && cmd.length() <= longMax) {
        return true;
    } else {
        Serial.println("Erreur");
        return false;
    }
}

void Menu::_unknown(String& L, Stream& S) {
    S.print(L);
    S.println(" : command not found");
}

void Menu::_call_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    ret = anchor->acceptCmd(p, 4, 9);
    if (ret == true) {
        p.toUpperCase();
        size_t len = strlen(p.c_str());

        if (len < sizeof (cfg.call)) {
            //memcpy(anchor->cfg.call, p.c_str(), len);
            //anchor->cfg.call[len] = '\0'; // Ajout manuel du terminateur
            strcpy(anchor->cfg.call, p.c_str());
            S.printf("Callsign set to: %s\n\r", anchor->cfg.call);
        } else {
            S.println(F("Callsign too long. Max 9 characters."));
        }
    } else {
        S.println(F("Invalid callsign format."));
    }

}

void Menu::_loc_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    ret = anchor->acceptCmd(p, 6, 6);
    if (ret == true) {
        p.toUpperCase();
            strcpy(anchor->cfg.locator, p.c_str());
            S.printf("Locator set to: %s\n\r", anchor->cfg.locator);
    } else {
        S.println(F("Invalid locator format ex JN07DV"));
    }
}

//anchor->cfg.mode

void Menu::_mode_(ArgList& L, Stream& S) {
    String p = L.getNextArg();
    p.toLowerCase();  // Normalise la casse pour comparaison
    if (p == "scottie1") {
        anchor->cfg.sstv = Scottie1;
        anchor->cfg.mode = 0;
    } else if (p == "scottie2") {
        anchor->cfg.sstv = Scottie2;
        anchor->cfg.mode = 1;
    } else if (p == "scottieDX") {
        anchor->cfg.sstv = ScottieDX;
        anchor->cfg.mode = 2;
    } else if (p == "martin1") {
        anchor->cfg.sstv = Martin1;
        anchor->cfg.mode = 3;
    } else if (p == "martin2") {
        anchor->cfg.sstv = Martin2;
        anchor->cfg.mode = 4;
    } else if (p == "wrasse") {
        anchor->cfg.sstv = Wrasse;
        anchor->cfg.mode = 5;
    } else if (p == "robot36") {
        anchor->cfg.sstv = Robot36;
        anchor->cfg.mode = 6;
    } else if (p == "robot72") {
        anchor->cfg.sstv = Robot72;
        anchor->cfg.mode = 7;
    } else if (p == "pd50") {
        anchor->cfg.sstv = PD50;
        anchor->cfg.mode = 8;
    } else if (p == "pd90") {
        anchor->cfg.sstv = PD90;
        anchor->cfg.mode = 9;
    } else if (p == "mp73") {
        anchor->cfg.sstv = MP73N;
        anchor->cfg.mode = 10;
    } else {
        S.println(F("Invalid mode syntax. Example: mode martin1"));
        return;
    }
    // Affiche le mode sélectionné    
    S.printf("Mode set to: %s\n\r", modeStr[anchor->cfg.mode]);
}

void Menu::_band_(ArgList& L, Stream& S) {
    String p = L.getNextArg();
    p.toLowerCase(); // Normalise la casse pour comparaison
    if (p == "80m") {
        anchor->cfg.band = M80;
        anchor->cfg.freq = anchor->tabfreq[0].freq;
        anchor->cfg.offset = anchor->tabfreq[0].offset;
    } else if (p == "60m") {
        anchor->cfg.band = M60;
        anchor->cfg.freq = anchor->tabfreq[1].freq;
        anchor->cfg.offset = anchor->tabfreq[1].offset;
    } else if (p == "40m") {
        anchor->cfg.band = M40;
        anchor->cfg.freq = anchor->tabfreq[2].freq;
        anchor->cfg.offset = anchor->tabfreq[2].offset;
    } else if (p == "30m") {
        anchor->cfg.band = M30;
        anchor->cfg.freq = anchor->tabfreq[3].freq;
        anchor->cfg.offset = anchor->tabfreq[3].offset;
    } else if (p == "20m") {
        anchor->cfg.band = M20;
        anchor->cfg.freq = anchor->tabfreq[4].freq;
        anchor->cfg.offset = anchor->tabfreq[4].offset;
    } else if (p == "freq") {
        anchor->cfg.band = FREQ;
    } else {
        S.println(F("Invalid mode syntax. Example: band 40m"));
        return;
    }
    // Affiche le mode sélectionné    
    S.printf("Band set to: %s\n\r", bandStr[anchor->cfg.band]);
}



//manque le test de la fréquence max supportée par le RPI
void Menu::_freq_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    ret = anchor->acceptCmd(p, 6, 8);
    if (ret == true) {
        uint32_t freqValue = p.toInt(); // Conversion de la chaîne en entier
        anchor->cfg.freq = freqValue; // Sauvegarde dans la structure
        S.printf("Frequency set to: %lu Hz\n\r", anchor->cfg.freq);
    }
}


void Menu::_minute_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    ret = anchor->acceptCmd(p, 1, 2);
    if (ret == true) {
        uint8_t minuteValue = (uint8_t) p.toInt(); // Conversion en entier 8 bits
        anchor->cfg.minute = minuteValue; // Sauvegarde dans la structure
        S.printf("Minute modulo set to: %u\n\r", anchor->cfg.minute);
    }
}

void Menu::_posx_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    ret = anchor->acceptCmd(p, 1, 3);
    if (ret == true) {
        uint8_t posx = (uint8_t) p.toInt(); // Conversion en entier 8 bits
        anchor->cfg.posx = posx; // Sauvegarde dans la structure
        S.printf("POSX set to: %u\n\r", anchor->cfg.posx);
    }
}

void Menu::_posy_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    ret = anchor->acceptCmd(p, 1, 3);
    if (ret == true) {
        uint8_t posy = (uint8_t) p.toInt(); // Conversion en entier 8 bits
        anchor->cfg.posy = posy; // Sauvegarde dans la structure
        S.printf("POSY set to: %u\n\r", anchor->cfg.posy);
    }
}

void Menu::_color_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    ret = anchor->acceptCmd(p, 1, 1);
    if (ret == true) {
        uint8_t color = (uint8_t) p.toInt(); // Conversion en entier 8 bits
        anchor->cfg.color = color; // Sauvegarde dans la structure
        S.printf("COLOR set to: %u\n\r", anchor->cfg.color);
    }
}

//facteur de correction
void Menu::_offset_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    ret = anchor->acceptCmd(p, 1, 5);
    if (ret == true) {
        int32_t offsetValue = p.toInt(); // Conversion en entier signé
        anchor->cfg.offset = offsetValue; // Sauvegarde dans la structure
        S.printf("Offset set to: %ld\n\r", anchor->cfg.offset);
    }
}


void Menu::_manAuto_(ArgList& L, Stream& S) {
    String p;
    bool ret;
    p = L.getNextArg();
    ret = anchor->acceptCmd(p, 1, 1);
    if (ret == true) {
        int val = p.toInt(); // Conversion en entier
        if (val == 0 || val == 1) {
            anchor->cfg.manAuto = (val == 1); // Affectation booléenne
            S.printf("Txing is %s\n\r", anchor->cfg.manAuto ? "automatic" : "manual");
        } else {
            S.println(F("Invalid value. Use 0 to disable or 1 to enable nmea debug."));
        }
    }
}

const char* Menu::colorToString(uint8_t color)
{
    switch (color)
    {
        case 0: return "Black";
        case 1: return "White";
        case 2: return "Red";
        case 3: return "Green";
        case 4: return "Blue";
        case 5: return "Yellow";
        case 6: return "Cyan";
        case 7: return "Magenta";
        case 8: return "Orange";
        case 9: return "Gray";
        default: return "Unknown";
    }
}


void Menu::_show_(ArgList& L, Stream& S) {
    S.println("Current configuration :");
    S.printf("  freq           : %lu Hz\n\r", anchor->cfg.freq);
    S.printf("  offset         : %ld\n\r", anchor->cfg.offset);
    S.printf("  call           : %s\n\r", anchor->cfg.call);
    S.printf("  locator        : %s\n\r", anchor->cfg.locator);
    S.printf("  mode           : %s\n\r", modeStr[anchor->cfg.mode]);
    S.printf("  band           : %s\n\r", bandStr[anchor->cfg.band]);
    S.printf("  posx           : %u\n\r", anchor->cfg.posx);
    S.printf("  posy           : %u\n\r", anchor->cfg.posy);
    S.printf("  color          : %s\n\r", anchor->colorToString(anchor->cfg.color));
    S.printf("  TXing mode     : %s\n\r", anchor->cfg.manAuto ? "AUTOMATIC" : "MANUAL");
    S.printf("  minute         : %u min\n\r", anchor->cfg.minute);
}

void Menu::_save_(ArgList& L, Stream& S) {
  EEPROM.put(0, anchor->cfg);
  EEPROM.commit();
  Serial.println("Configuration saved.");
}


  
void Menu::_raz_(ArgList& L, Stream& S) {

    anchor->cfg.freq =  anchor->tabfreq[2].freq; // Fréquence par défaut
    anchor->cfg.offset =  anchor->tabfreq[2].offset;
    strcpy(anchor->cfg.call, "NOCALL"); // Indicatif par défaut
    strcpy(anchor->cfg.locator, "JN07"); // Locator par défaut
    anchor->cfg.minute = 5; // Durée par défaut
    anchor->cfg.sstv=Martin1;
    anchor->cfg.mode = 3;
    anchor->cfg.manAuto=false;
    anchor->cfg.posx=10;
    anchor->cfg.posy=50;
    anchor->cfg.color=2;
    anchor->cfg.band = FREQ;
    Serial.println("Configuration has been reset to default values.");    
}

void Menu::_restart_(ArgList& L, Stream& S) {
    watchdog_reboot(0, 0, 0);  // Redémarre immédiatement    
}


void Menu::_scan_(ArgList& L, Stream& S) {
    anchor->scanI2C();
}

void Menu::scanI2C() {
    Serial.println("    00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f");
    for (uint8_t row = 0; row < 8; row++) {
        Serial.printf("%02x: ", row << 4);
        for (uint8_t col = 0; col < 16; col++) {
            uint8_t address = (row << 4) | col;
            Wire.beginTransmission(address);
            uint8_t error = Wire.endTransmission();

            if (error == 0) {
                Serial.printf("%02x ", address);
            } else if (error == 4) {
                Serial.print("UU "); // Bus error
            } else {
                Serial.print("-- ");
            }
        }
        Serial.println();
    }
}


Menu* Menu::anchor = NULL;

