/*
 * funcoes_brain.c
 *
 *  Created on: 06/12/2022
 *      Author: Dinis
 */

#include "funcoes_brain.h"

struct tipo_dados_queue_brain_controlo_motores_0_255 {
	uint8_t codigo_motor_1;
	uint8_t codigo_motor_2;
};

bool verifica_valor_limite_velocidade(int velocidade) {
	return true;
}

void processa_dados_web(String cmd, int valor) {

	if (cmd == "MoveCar") {
		processa_cmd_web_controlo_carro(valor);
		//Serial.println("mexer no carro");
	}

	else if (cmd == "Light") {
		processa_cmd_web_alterar_intensidade_luz(valor);
		//Serial.println("mexer na intensidade da luz");
	}

}

void processa_cmd_web_controlo_carro(int codigo_controlo) {

	tipo_dados_queue_brain_controlo_motores_0_255 codigos_motores;

	switch (codigo_controlo) {

	case CODIGO_WEB_BRAIN_ANDAR_FRENTE:
		//Serial.println("Andar frente");
		codigos_motores.codigo_motor_1 = valor_default_andar_frente;
		codigos_motores.codigo_motor_2 = valor_default_andar_frente;
		break;

	case CODIGO_WEB_BRAIN_ANDAR_TRAS:
		//Serial.println("Andar tras");
		codigos_motores.codigo_motor_1 = valor_default_andar_tras;
		codigos_motores.codigo_motor_2 = valor_default_andar_tras;
		break;

	case CODIGO_WEB_BRAIN_ANDAR_ESQUERDA:
		//Serial.println("Andar esquerda");
		codigos_motores.codigo_motor_1 =
		valor_default_curvar_parado_roda_dentro;
		codigos_motores.codigo_motor_2 = valor_default_curvar_parado_roda_fora;
		break;

	case CODIGO_WEB_BRAIN_ANDAR_DIREITA:
		//Serial.println("Andar direita");
		codigos_motores.codigo_motor_1 = valor_default_curvar_parado_roda_fora;
		codigos_motores.codigo_motor_2 =
		valor_default_curvar_parado_roda_dentro;
		break;

	case CODIGO_WEB_BRAIN_PARAR:
		//Serial.println("Parar");
		codigos_motores.codigo_motor_1 = valor_default_travar;
		codigos_motores.codigo_motor_2 = valor_default_travar;
		break;

	default:
		//Serial.println("Parar");
		codigos_motores.codigo_motor_1 = valor_default_travar;
		codigos_motores.codigo_motor_2 = valor_default_travar;
		break;
	}
	xQueueOverwrite(xQueue_brain_controlo_motores_0_255,
			(void* ) &codigos_motores);
}

void processa_cmd_web_alterar_intensidade_luz(int valor_intensidade_led) {

	xQueueOverwrite(xQueue_brain_controla_led_0_255,
			(void* ) &valor_intensidade_led);

}

