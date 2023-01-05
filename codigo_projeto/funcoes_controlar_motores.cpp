#include "funcoes_controlar_motores.h"

void ligar_travao_motor(uint8_t pino_travao){
	digitalWrite(pino_travao, LOW);
}
void desligar_travao_motor(uint8_t pino_travao){
	digitalWrite(pino_travao, HIGH);
}
void desligar_sinal_pwm(uint8_t pino_pwm){
	ledcWrite(pino_pwm, 0);
}

void controlar_motor_geral(uint8_t pino_sinal_pwm, uint8_t pino_sinal_travao, uint8_t pino_sinal_stop, uint8_t pino_sinal_sentido, uint8_t byte_estado_motor, uint8_t sentido_rotacao){
	if(byte_estado_motor==BYTE_ESTADO_FASE_DESLIGADA){
		//desligar a fase do motor (rele)

	}else if(byte_estado_motor==BYTE_ESTADO_TRAVAO){
		//ligar a fase do motor (rele)
		//travar
		desligar_sinal_pwm(pino_sinal_pwm);
		ligar_travao_motor(pino_sinal_travao);

	}else if(byte_estado_motor==BYTE_ESTADO_DESTRAVADO){
		//destravado
		desligar_sinal_pwm(pino_sinal_pwm);
		desligar_travao_motor(pino_sinal_travao);

	}else if(byte_estado_motor==BYTE_ESTADO_EMERGENCIA){
		//stop-emergencia

	}else if(byte_estado_motor>=BYTE_ESTADO_VELOCIDADE_MAXIMO_TRAS && byte_estado_motor<=BYTE_ESTADO_VELOCIDADE_MINIMO_TRAS){
		desligar_travao_motor(pino_sinal_travao);
		//para tras
		digitalWrite(pino_sinal_sentido, !sentido_rotacao);
		Serial.println (digitalRead (pino_sinal_sentido), DEC);
		uint8_t valor_pwm=-2.032*byte_estado_motor+257.032;
		ledcWrite(pino_sinal_pwm, valor_pwm);
	}else if(byte_estado_motor>=BYTE_ESTADO_VELOCIDADE_MINIMA_FRENTE && byte_estado_motor<=BYTE_ESTADO_VELOCIDADE_MAXIMA_FRENTE){
		desligar_travao_motor(pino_sinal_travao);
		//para frente
		uint8_t valor_pwm= (((BYTE_VALOR_MAXIMO_VELOCIDADE_PWM-1)*(byte_estado_motor-BYTE_ESTADO_VELOCIDADE_MINIMA_FRENTE))/(BYTE_ESTADO_VELOCIDADE_MAXIMA_FRENTE-BYTE_ESTADO_VELOCIDADE_MINIMA_FRENTE))+1;
		if(sentido_rotacao!=digitalRead (pino_sinal_sentido)){
			//Serial.println (digitalRead (pino_sinal_sentido), DEC);
			ligar_travao_motor(pino_sinal_travao);
			desligar_travao_motor(pino_sinal_travao);
		}
		digitalWrite(pino_sinal_sentido, sentido_rotacao);
		ledcWrite(pino_sinal_pwm, valor_pwm);

	}
}


void controlar_motor_1(uint8_t byte_controlo_motor){
	controlar_motor_geral(CHANEL_MOTOR_1_PINO_PWM, MOTOR_1_PINO_TRAVAO, MOTOR_1_PINO_STOP, MOTOR_1_PINO_SENTIDO, byte_controlo_motor, true);
}

void controlar_motor_2(uint8_t byte_controlo_motor){
	controlar_motor_geral(CHANEL_MOTOR_2_PINO_PWM, MOTOR_2_PINO_TRAVAO, MOTOR_2_PINO_STOP, MOTOR_2_PINO_SENTIDO, byte_controlo_motor, false);
}

void configura_pinos_INPUT_OUTPUT_Controlo_motores(void){

	ledcAttachPin(MOTOR_1_PINO_PWM, CHANEL_MOTOR_1_PINO_PWM);
	ledcSetup(CHANEL_MOTOR_1_PINO_PWM, 5000, 8);

	ledcAttachPin(MOTOR_2_PINO_PWM, CHANEL_MOTOR_2_PINO_PWM);
	ledcSetup(CHANEL_MOTOR_2_PINO_PWM, 5000, 8);

	//pinMode(MOTOR_1_PINO_PWM, OUTPUT);
	pinMode(MOTOR_1_PINO_TRAVAO, OUTPUT);
	//pinMode(MOTOR_1_PINO_STOP, OUTPUT);
	pinMode(MOTOR_1_PINO_SENTIDO, OUTPUT);

	//pinMode(MOTOR_2_PINO_PWM, OUTPUT);
	pinMode(MOTOR_2_PINO_TRAVAO, OUTPUT);
	//pinMode(MOTOR_2_PINO_STOP, OUTPUT);
	pinMode(MOTOR_2_PINO_SENTIDO, OUTPUT);
}

