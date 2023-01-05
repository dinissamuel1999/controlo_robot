/*
 * constantes_projeto.h
 *
 *  Created on: 12/12/2022
 *      Author: Dinis
 */

#ifndef CONSTANTES_PROJETO_H_
#define CONSTANTES_PROJETO_H_

//+++++++++++++++++++++++++++++++++++++++Configuracao de Pinos+++++++++++++++++++++++++++++++++++++++

//Led
#define LED_PIN 21
//Comunicacao SPI (RFId/Lcd-TFT)
#define RST_PIN   22
#define SS_PIN    5//SDA
#define MISO_PIN  19
#define MOSI_PIN  23
#define SCK_PIN   18
#define TFT_CS 25
#define TFT_DC 26
//Ultra Sons
#define PIN_TRIGGER   32
#define PIN_ECHO      33
//encoder velocidade
#define PINO_CONTAGEM_PASSOS_HALL_MOTOR1 14
#define PINO_CONTAGEM_PASSOS_HALL_MOTOR2 4
//controlo_motores
#define MOTOR_1_PINO_PWM 27
#define MOTOR_2_PINO_PWM 2
#define MOTOR_1_PINO_SENTIDO 12
#define MOTOR_2_PINO_SENTIDO 15
#define MOTOR_1_PINO_TRAVAO 67 //nao utilizado
#define MOTOR_2_PINO_TRAVAO 68 //nao utilizado
#define MOTOR_1_PINO_STOP 77 //nao utilizado
#define MOTOR_2_PINO_STOP 78 //nao utilizado


//+++++++++++++++++++++++++++++++++++++++Outros+++++++++++++++++++++++++++++++++++++++
//canais escrita PWM
#define CHANEL_MOTOR_1_PINO_PWM 0
#define CHANEL_MOTOR_2_PINO_PWM 1
//distancia entre pixeis lcd-TFT
#define DISTC 12
//periodicidade tarefas
#define PERIODICIDADE_TASK_ENCODER_VELOCIDADE 250
#define WIFI_PERIODICIDADE_VERIFICACAO_MS 5000



//+++++++++++++++++++++++++++++++++++++++Constantes definidas+++++++++++++++++++++++++++++++++++++++
//#define DISTANCIA_PERCORRIDA_ENTRE_PULSOS_SINAL_VELOCIDADE_HALL 0.03267256 //15pulsos por rotacao
#define DISTANCIA_PERCORRIDA_ENTRE_PULSOS_SINAL_VELOCIDADE_HALL 0.01089085333 //45pulsos por rotacao


//valores default controlo velocidade
#define valor_default_andar_frente 160
#define valor_default_andar_tras 105
#define valor_default_curvar_parado_roda_dentro 112
#define valor_default_curvar_parado_roda_fora 142
#define valor_default_travar 127



#define BYTE_VALOR_MAXIMO_VELOCIDADE_PWM 200//nao utilizado


//+++++++++++++++++++++++++++++++++++++++Codigos definidos+++++++++++++++++++++++++++++++++++++++

//estado autenticacao na queue_estado_autenticacao (tarefa LCD e Rfid)
#define AUTENTICACAO_APRESENTAR_CARTAO 0
#define AUTENTICACAO_NAO_AUTORIZADO 1
#define AUTENTICACAO_AUTORIZADO 2

//estado Wifi_escritos na queue_estado_wifi (tarefa Wifi)
#define WIFI_CONECTION_OK 0
#define WIFI_CONECTION_FAIL 1

//task controlo_motores
#define BYTE_ESTADO_FASE_DESLIGADA 0
#define BYTE_ESTADO_VELOCIDADE_MAXIMO_TRAS 1
#define	BYTE_ESTADO_VELOCIDADE_MINIMO_TRAS 126
#define BYTE_ESTADO_TRAVAO 127
#define BYTE_ESTADO_VELOCIDADE_MINIMA_FRENTE 128
#define BYTE_ESTADO_VELOCIDADE_MAXIMA_FRENTE 253
#define BYTE_ESTADO_DESTRAVADO 254
#define BYTE_ESTADO_EMERGENCIA 255

//task brain para controlo veiculo(vindos do web-server)
#define CODIGO_WEB_BRAIN_ANDAR_FRENTE 1
#define CODIGO_WEB_BRAIN_ANDAR_TRAS 2
#define CODIGO_WEB_BRAIN_ANDAR_ESQUERDA 3
#define CODIGO_WEB_BRAIN_ANDAR_DIREITA 4
#define CODIGO_WEB_BRAIN_PARAR 0

#endif /* CONSTANTES_PROJETO_H_ */
