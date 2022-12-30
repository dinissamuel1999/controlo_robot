/*
 * funcoes_controlar_motores.h
 *
 *  Created on: 18/11/2022
 *      Author: Dinis
 */

#ifndef FUNCOES_CONTROLAR_MOTORES_H_
#define FUNCOES_CONTROLAR_MOTORES_H_

#include <Arduino.h>
#include "constantes_projeto.h"



void controlar_motor_geral(uint8_t pino_sinal_pwm, uint8_t pino_sinal_travao, uint8_t pino_sinal_stop, uint8_t pino_sinal_sentido, uint8_t byte_estado_motor, uint8_t sentido_rotacao);
void controlar_motor_1(uint8_t byte_controlo_motor);
void controlar_motor_2(uint8_t byte_controlo_motor);

void ligar_travao_motor(uint8_t pino_travao);
void desligar_travao_motor(uint8_t pino_travao);
void desligar_sinal_pwm(uint8_t pino_pwm);



void configura_pinos_INPUT_OUTPUT_Controlo_motores(void);


#endif /* FUNCOES_CONTROLAR_MOTORES_H_ */
