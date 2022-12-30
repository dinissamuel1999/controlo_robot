/*
 * funcoes_brain.h
 *
 *  Created on: 06/12/2022
 *      Author: Dinis
 */

#ifndef FUNCOES_BRAIN_H_
#define FUNCOES_BRAIN_H_

#include <Arduino.h>
#include <string.h>
#include "constantes_projeto.h"

extern QueueHandle_t xQueue_brain_controlo_motores_0_255;
extern QueueHandle_t xQueue_brain_controla_led_0_255;

void processa_dados_web(String cmd, int valor);
void processa_cmd_web_controlo_carro(int inputValue);
void processa_cmd_web_alterar_intensidade_luz(int valor_velocidade);

bool verifica_valor_limite_velocidade(int velocidade);

#endif /* FUNCOES_BRAIN_H_ */
