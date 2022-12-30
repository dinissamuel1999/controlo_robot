/*
 * funcoes_encoder_velocidade.c
 *
 *  Created on: 07/12/2022
 *      Author: Dinis
 */
#include "funcoes_encoder_velocidade.h"

float Calcula_velocidade_linear_roda(long numero_pulsos) {
	float velocidade=0;
	velocidade = (float) (numero_pulsos * 1000	* DISTANCIA_PERCORRIDA_ENTRE_PULSOS_SINAL_VELOCIDADE_HALL )  / PERIODICIDADE_TASK_ENCODER_VELOCIDADE;
	return velocidade;
}

float Calcula_velocidade_media_linear_carro(float velocidade_roda1,
		float velocidade_roda2) {
	return (velocidade_roda1 + velocidade_roda2) / 2;
}

void configura_pinos_encoder_interrupt(void) {

	//INPUT_PULLUP
}
