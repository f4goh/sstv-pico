/* 
 * File:   Afficheur.cpp
 * Author: F4GOH
 * 
 */

#include "Afficheur.h"

// Constructeur de Afficheur

Afficheur::Afficheur()
{
    tft = new Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
}

Afficheur::~Afficheur() {
    // Rien à nettoyer
}

void Afficheur::begin() {
    // Initialisation I2C avec adresse 0x3C
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    SPI.setSCK(TFT_SCK);
    SPI.setTX(TFT_MOSI);
    tft->init(320, 320);
    tft->setRotation(1);
    tft->setAddrWindow(34, 0, 240, 320);
    tft->fillScreen(ST77XX_BLACK);
tft->setTextWrap(false);
    
}

const uint16_t Afficheur::lineColors[8] = {
    ST77XX_YELLOW,
    ST77XX_CYAN,
    ST77XX_GREEN,
    ST77XX_MAGENTA,
    ST77XX_ORANGE,
    ST77XX_WHITE,
    ST77XX_RED,
    ST77XX_BLUE
};

void Afficheur::timeDisplay(char* txt) {
    tft->setTextSize(4);
    tft->setTextColor(ST77XX_CYAN, ST77XX_BLACK);

    const int width = 172;
    const int height = 22;

    // Efface uniquement la zone du haut
    tft->fillRect(0, 0, width, height, ST77XX_BLACK);

    // Petit padding
    tft->setCursor(46, 0);
    tft->print(txt);
}

void Afficheur::modeDisplay(const char* txt) {
    int16_t x, y;
    uint16_t w, h;

    tft->setTextSize(4);
    tft->setTextColor(ST77XX_YELLOW, ST77XX_BLACK);

    tft->getTextBounds(txt, 0, 0, &x, &y, &w, &h);

    const int width = 320;
    const int height = 172;

    // Centrage parfait en portrait
    int cx = (width - w) / 2;
    int cy = (height - h) / 2;

    // Efface uniquement la zone centrale (évite de flinguer le reste)
    int clearY = cy - 4;
    int clearH = h + 8;

    tft->fillRect(0, clearY, width, clearH, ST77XX_BLACK);

    tft->setCursor(cx, cy);
    tft->print(txt);
}

void Afficheur::modeEfface() {
    const int width = 172;
    // Zone centrale approximative (même logique que modeDisplay)
    tft->fillRect(0, 120, width, 80, ST77XX_BLACK);
}

/*
void Afficheur::clearDisplay() {
    tft->fillScreen(ST77XX_BLACK);
}
*/
