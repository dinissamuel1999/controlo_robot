/*
 * funcoes_encoder_velocidade.h
 *
 *  Created on: 07/12/2022
 *      Author: Dinis
 */

#ifndef FUNCOES_ENCODER_VELOCIDADE_H_
#define FUNCOES_ENCODER_VELOCIDADE_H_

#include "constantes_projeto.h"
#include <Arduino.h>
float Calcula_velocidade_linear_roda(long numero_pulsos);
float Calcula_velocidade_media_linear_carro(float velocidade_roda1,float velocidade_roda2);

#endif /* FUNCOES_ENCODER_VELOCIDADE_H_ */
