/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Dds.cpp
 * Author: ale
 * 
 * Created on 28 août 2025, 07:55
 * 
 * Adaptation SSTV pico 2 en cours
 */

#include "Dds.h"

Dds::Dds() {
}

Dds::Dds(const Dds& orig) {
}

Dds::~Dds() {
}

void __not_in_flash_func(Dds::coreUnSetup)(){
//void Dds::coreUnSetup() {
  uint16_t t;
  uint32_t delta;
  PIO pio = pio0;
  const uint sm = 1;

  pio_gpio_init(pio, 28);

  uint offset = pio_add_program(pio, &vfo_program);

  pio_sm_set_consecutive_pindirs(pio, sm, 28, 1, true);

  pio_sm_config c = vfo_program_get_default_config(offset);
  sm_config_set_set_pins(&c, 28, 1);

  pio_sm_init(pio, sm, offset, &c);
  pio_sm_set_enabled(pio, sm, true);

  periods = 200 << 24;  // DDS off

  
  for (;;) {
    uint32_t p = periods;

    t = (p + delta) >> 24;
    pio_sm_put_blocking(pio, sm, t);
    delta += p - (t << 24);
}
  /*
  for (;;) {
    t = (periods + delta) >> 24;
    pio_sm_put_blocking(pio, sm, t);
    delta += periods - (t << 24);
  }
   * */
}



//A simplifier pour le calcul des fréquences beaucoup de répétitions

void Dds::setFreqBase(uint32_t freq) {
    /*
    uint64_t ratio = (uint64_t) CLK_KHZ * 1000LL * (1 << 24) / (uint64_t) freq;
    periods = (uint32_t) ratio;
    dwsfr = SSTV_TONE_SPACING * (periods / freq) / 100L;
    baseFreq = periods - (10 << 24); // + 3 x Abstand 1,46 Hz
    //wsfr[1] = wsfr[0] - dwsfr;
     * */
    baseFreq=freq;
}

void __not_in_flash_func(Dds::setFreq)(uint32_t freq){
//void Dds::setFreq(uint32_t freq) {
    static uint32_t IF_FREQ = baseFreq;
    static constexpr uint32_t DDS_OFFSET = (10UL << 24);
    static constexpr uint64_t DDS_NUM =
            (uint64_t) CLK_KHZ * 1000ULL * (1ULL << 24);
    periods = (uint32_t) (DDS_NUM / (freq + IF_FREQ)) - DDS_OFFSET;
}

/*
void Dds::setFreq(uint32_t freq) {
    uint64_t ratio = (uint64_t) CLK_KHZ * 1000LL * (1 << 24) / (uint64_t) (freq+7100000L);
    uint32_t period = (uint32_t) ratio;
    periods = period - (10 << 24);
    //periods=baseFreq-(dwsfr*freq);
}
*/


void Dds::off(){
  periods = 200 << 24;  
}

void Dds::sendCarriage(bool state) {
    if (state) {
        periods = baseFreq;
    } else {
        periods = 200 << 24;
    }
}

