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
 * ov5640
 * https://fr.aliexpress.com/item/1005003006706291.html
 * 
 * 
 */
#include <Arduino.h>
#include <stdint.h>
#include "LCD_2in.h"
#include "DEV_Config.h"
#include "cam.h"
#include "CST816D.h"
//#include "Dds.h"
#include "Sstv.h"
#include "SSTVDisplay.h"

#define SSTV_WIDTH  320
#define SSTV_HEIGHT 240

uint8_t cam_buffer[SSTV_WIDTH * SSTV_HEIGHT * 2];

UWORD *BlackImage;
volatile bool touch_flag = false;
volatile bool stateObjMod = false;

//SSTVMode_t mode=Scottie1; //defaut mode
SSTVMode_t mode = PD90; //defaut mode

bool sendFlag = false;

//Dds dds;

Sstv *sstv;
SSTVDisplay incrustation; 

extern "C" char *sbrk(int incr);

int freeMemory() {
    char stack_dummy;
    return &stack_dummy - sbrk(0);
}

void Touch_INT_callback() {
    touch_flag = true;
}

void setup() {
    set_sys_clock_khz(CLK_KHZ, true);
    Serial.begin(115200);

    if (DEV_Module_Init() != 0) {
        while (1) {
        }
    }
    DEV_SET_PWM(0);

    // Initialisation du tactile
    CST816D_init(CST816D_Point_Mode);

    // Configuration de l'interruption tactile
    DEV_KEY_Config(Touch_INT_PIN);
    attachInterrupt(Touch_INT_PIN, Touch_INT_callback, RISING);



    printf("2inch LCD demo...\r\n");
    LCD_2IN_Init(VERTICAL);
    LCD_2IN_Clear(0xffff);
    DEV_SET_PWM(100);

    init_cam(); // initialize camera
    config_cam_buffer(); // config buffer
    start_cam(); // start streaming

    sstv = new Sstv();
    //dds.setFreqCW(7100000L);
    stateObjMod = true; //il faut que l'objet mod ait le temps de s'initialiser avant de lancer le dds dans le core 1


}

void loop() {
    char c;
    if (Serial.available() > 0) {
        c = Serial.read();
        switch (c) {
            case '1':
                mode = Scottie1;
                Serial.println("Mode Scottie1");
                break;
            case '2':
                mode = Scottie2;
                Serial.println("Mode Scottie2");
                break;
            case '3':
                mode = ScottieDX;
                Serial.println("Mode ScottieDX");
                break;
            case '4':
                mode = Martin1;
                Serial.println("Mode Martin1");
                break;
            case '5':
                mode = Martin2;
                Serial.println("Mode Martin2");
                break;
            case '6':
                mode = Wrasse;
                Serial.println("Mode Wrasse");
                break;
            case '7':
                mode = Robot36;
                Serial.println("Mode Robot36");
                break;
            case '8':
                mode = Robot72;
                Serial.println("Mode Robot72");
                break;
            case '9':
                mode = PD50;
                Serial.println("Mode PD50");
                break;
            case 'a':
                mode = PD90;
                Serial.println("Mode PD90");
                break;
            case 'b':
                mode = MP73N;
                Serial.println("Mode MP73-N");
                break;
            case 'k':
                sstv->tx(mode);
                Serial.println("camera");
                sendFlag = true;
            case 'h':
                Serial.println();
                Serial.println("========== SSTV HELP ==========");
                Serial.println("1 : Select Scottie1 (RGB)");
                Serial.println("2 : Select Scottie2 (RGB)");
                Serial.println("3 : Select ScottieDX (RGB)");
                Serial.println("4 : Select Martin1 (RGB)");
                Serial.println("5 : Select Martin2 (RGB)");
                Serial.println("6 : Select Wrasse (RGB)");
                Serial.println("7 : Select Robot36 (YUV)");
                Serial.println("8 : Select Robot72 (YUV)");
                Serial.println("9 : Select PD50 (YUV)");
                Serial.println("a : Select PD90 (YUV)");
                Serial.println("b : Select MP73-N (YUV, Narrow)");
                Serial.println();
                Serial.println("k : Transmit camera image");
                Serial.println("m : Transmit test pattern");
                Serial.println("p : Transmit loaded image");
                Serial.println("i : Idle mode");
                Serial.println("s : Standby (RF gain off)");
                Serial.println("h : Show this help");
                Serial.println("===============================");
                Serial.println();
                break;

                break;
            case 'm':
                sstv->tx(mode);
                if (mode.visCode == SSTV_ROBOT_36 || mode.visCode == SSTV_ROBOT_72 || mode.visCode == SSTV_PD_50 || mode.visCode == SSTV_PD_90) {
                    sstv->sendMire(YUV);
                } else if (mode.visCode == SSTV_MP_73_N) {
                    sstv->sendMireNarrow();
                } else {
                    sstv->sendMire(RGB);
                }
                break;
            case 'p':
                sstv->tx(mode);
                if (mode.visCode == SSTV_ROBOT_36 || mode.visCode == SSTV_ROBOT_72 || mode.visCode == SSTV_PD_50 || mode.visCode == SSTV_PD_90) {
                    sstv->sendImg(YUV);
                } else if (mode.visCode == SSTV_MP_73_N) {
                    sstv->sendImgNarrow();
                } else {
                    sstv->sendImg(RGB);
                }
                break;
            case 'i':
                Serial.println("test idle");
                sstv->idle();
                break;
            case 's':
                Serial.println("standby an gain off");
                sstv->standby();
                break;
        }
    }
    if (buffer_ready) {
        cam_swap_rgb565_bytes(cam_ptr, cam_buffer, CAM_FUL_SIZE);
        if (sendFlag || touch_flag) {
            //cam_pause();
            cam_rotate_90_mirror_vertical_rgb565(cam_buffer, cam_ptr, 240, 320);
            if (mode.visCode == SSTV_ROBOT_36 || mode.visCode == SSTV_ROBOT_72 || mode.visCode == SSTV_PD_50 || mode.visCode == SSTV_PD_90 || mode.visCode == SSTV_MP_73_N) {
                Serial.println("YUV565 QVGA");
                sstv->rgb2yuv(cam_ptr, cam_buffer, 320, 240);
                incrustation.drawString(100, 30, "F4GOH", cam_buffer, YUV);
                sstv->tx(mode);
                if (mode.visCode == SSTV_MP_73_N) {
                    sstv->sendCameraYUVNarrow((uint8_t*) cam_buffer);
                } else {
                    sstv->sendCameraYUV((uint8_t*) cam_buffer);
                }
            } else {
                Serial.println("RGB565 QVGA");
                incrustation.drawString(100, 30, "F4GOH", cam_ptr, RGB);                    
                sstv->tx(mode);
                sstv->sendCameraRGB((uint8_t*) cam_ptr);
            }
            sendFlag = false;
            touch_flag = false;
            //cam_play();
        } else {
            LCD_2IN_Display((UBYTE *) cam_buffer);
        }
        //delay(1000);
        //Serial.print("RAM libre : ");
        //Serial.print(freeMemory());
        //Serial.println(" octets");
        buffer_ready = false; // Reset the flag
    }

}


void setup1(void) {
    while (!stateObjMod) {
    }
    delay(1); //nécessaire pour que le dds démarre
    sstv->coreUnSetup();
}

/*
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