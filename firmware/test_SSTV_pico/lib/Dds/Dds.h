/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Dds.h
 * Author: ale
 *
 * Created on 28 août 2025, 07:55
 */

#ifndef DDS_H
#define DDS_H

#include <Arduino.h>
#include "vfo2.pio.h"
#include "pico/stdlib.h"
#include "pico/multicore.h"


//#define CLK_KHZ  125000L
#define CLK_KHZ  180000L

#define SSTV_TONE_SPACING       100L //1.00 Hz

class Dds {
public:
    Dds();
    Dds(const Dds& orig);
    virtual ~Dds();
    
   
    void coreUnSetup();
    //void __not_in_flash_func(coreUnSetup)();
    void setFreqBase(uint32_t freq);
    
    void setFreq(uint32_t freq);
     //void __not_in_flash_func(setFreq)(uint32_t freq);
     
    void sendCarriage(bool state);
    void off();
    
    uint32_t baseFreq;   
    uint32_t dwsfr;  
     
    volatile uint32_t periods;  //pas de setter en lien avec les tables pour le moment mais il faudrait le mettre en private
    
private:
};

#endif // DDS_H