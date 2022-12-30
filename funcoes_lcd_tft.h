/*
 * funcoes_lcd_tft.h
 *
 *  Created on: 09/12/2022
 *      Author: Dinis
 */

#ifndef FUNCOES_LCD_TFT_H_
#define FUNCOES_LCD_TFT_H_

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

#include "constantes_projeto.h"

extern Adafruit_ILI9341 tft;

void atualiza_estado_login(char codigo);
void sem_end_ip_atribuido(void);
void atualiza_end_ip(IPAddress end_ip);
void parametros_estaticos_lcd_tft(void);
void atualiza_distancia_TFT(float distancia);
void atualiza_velocidade_TFT(float velocidade);



#endif /* FUNCOES_LCD_TFT_H_ */
