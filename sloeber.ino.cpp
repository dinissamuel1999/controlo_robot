#ifdef __IN_ECLIPSE__
//This is a automatic generated file
//Please do not modify this file
//If you touch this file your change will be overwritten during the next build
//This file has been generated on 2022-12-30 11:33:41

#include "Arduino.h"
#include "Arduino.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_task_wdt.h"
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Ultrasonic.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <MFRC522.h>
#include "constantes_projeto.h"
#include "funcoes_webserver.h"
#include "funcoes_brain.h"
#include "funcoes_encoder_velocidade.h"
#include "funcoes_controlar_motores.h"
#include "funcoes_lcd_tft.h"

void setup() ;
void loop() ;
void vTask_controlo_led(void *pvParameters) ;
void vTask_Web_Server(void *pvParameters) ;
void vTask_Rfid(void *pvParameters) ;
void vTask_Lcd_Tft(void *pvParameters) ;
void vTask_obtencao_distancia(void *pvParameters) ;
void vTask_encoder_velocidade(void *pvParameters) ;
void vTask_Brain(void *pvParameters) ;
void vTask_Controlo_Motores(void *pvParameters) ;
void vTask_Kepp_Wifi_Live(void *pvParameters) ;
static void IRAM_ATTR vInterruptHandler_conta_passos_hall_motor1(void) ;
static void IRAM_ATTR vInterruptHandler_conta_passos_hall_motor2(void) ;

#include "task_wifi_connection.ino"


#endif
