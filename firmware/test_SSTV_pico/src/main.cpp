/*
 BlackImage est un pointeur vers la mémoire qui contient l'image affichée sur le LCD.
 Chaque pixel occupe donc 16 bits (2 octets), au format RGB565.
 uint16_t pixel = cam_ptr[y * WIDTH + x];
 UWORD pixel = BlackImage[y * LCD_2IN.WIDTH + x];
 BlackImage contient les pixels en RGB565 sur 16 bits.
 offset = (Y * largeur + X) * 2;
 bit 15              bit 0
 RRRRR GGGGGG BBBBB
 0xF800 = rouge pur
 0x07E0 = vert pur
 0x001F = bleu pur
 0xFFFF = blanc
 0x0000 = noir
 Adresse +0       Adresse +1
 MSB              LSB
 RRRRRGGG         GGGBBBBB 
 rouge Color = 0xF800
 gpio libres
 GPIO12 i2c IMU
 GPIO13 i2c IMU
 GPIO28 libre
 * 
 */
#include <Arduino.h>
#include "LCD_2in.h"
#include "DEV_Config.h"
#include "cam.h"
#include "CST816D.h"
//#include "Dds.h"
#include "Sstv.h"
//#include "SSTVDisplay.h"

UWORD *BlackImage;
volatile bool touch_flag = false;
volatile bool stateObjMod=false;

SSTVMode_t mode=Scottie1; //defaut mode

//Dds dds;

Sstv *sstv;

extern "C" char *sbrk(int incr);

int freeMemory()
{
    char stack_dummy;
    return &stack_dummy - sbrk(0);
}


void Touch_INT_callback()
{
    touch_flag = true;
}

void setup()
{
    set_sys_clock_khz(CLK_KHZ, true);
    Serial.begin(115200);

     if(DEV_Module_Init()!=0){
         while(1){}
    }
    DEV_SET_PWM(0);

    // Initialisation du tactile
    CST816D_init(CST816D_Point_Mode);

    // Configuration de l'interruption tactile
    DEV_KEY_Config(Touch_INT_PIN);
    attachInterrupt(Touch_INT_PIN, Touch_INT_callback, RISING);
    
    /*
    
    printf("2inch LCD demo...\r\n");
    LCD_2IN_Init(VERTICAL);
    LCD_2IN_Clear(0xffff);
    DEV_SET_PWM(100);
    
    init_cam();          // initialize camera
    config_cam_buffer(); // config buffer
    start_cam();         // start streaming
    */
    sstv = new Sstv();
    //dds.setFreqCW(7100000L);
    stateObjMod = true; //il faut que l'objet mod ait le temps de s'initialiser avant de lancer le dds dans le core 1
    
    
}

void loop() {
    char c;
    if (Serial.available() > 0) {
        c = Serial.read();
        switch (c) {
            case 'm':
                Serial.println("Mode Scottie1");
                sstv->tx(mode);
                sstv->sendMire(RGB);
                break;
            case 'f':
                Serial.println("freq1000");
                sstv->setFreq(1000);
                delay(1000);
                Serial.println("freq1500");
                sstv->setFreq(1500);
                delay(1000);
                break;
            case 'p':
                 sstv->tx(mode);
                 sstv->sendImg(RGB);
                 break;
            case 'o':
                Serial.println("off");
                sstv->off();
                break;
        }
    }
    
    /*
    if (buffer_ready) {
        cam_pause();
        cam_swap_rgb565_bytes(cam_ptr, CAM_FUL_SIZE);
        LCD_2IN_Display((UBYTE *) cam_ptr);
        Serial.print("RAM libre : ");
        Serial.print(freeMemory());
        Serial.println(" octets");
        buffer_ready = false; // Reset the flag
        cam_play();
    }
    
    if (touch_flag)
    {
        touch_flag = false;
        // Lecture des coordonnées
        CST816D_Get_Point();
        Serial.print("X = ");
        Serial.print(Touch_CTS816.x_point);
        Serial.print("  Y = ");
        Serial.println(Touch_CTS816.y_point);
        //dds.sendCarriage(true);
    }
     * */
}


void setup1(void){
    while (!stateObjMod){}
    delay(1); //nécessaire pour que le dds démarre
    sstv->coreUnSetup();
}

