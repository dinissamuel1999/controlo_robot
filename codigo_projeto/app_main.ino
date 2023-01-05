#include "Arduino.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_task_wdt.h"

//+++++++++++++++++++++++++++libraries+++++++++++++++++++++++++++
#include <WiFi.h>//Wifi-Web server
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Ultrasonic.h>//Sensor ultra sons
#include <SPI.h>//Comunicação SPI
#include <Adafruit_GFX.h>//Escrita lcd-tft
#include <Adafruit_ILI9341.h>
#include <MFRC522.h>//Sensor Rfid

//+++++++++++++++++++++++++++FILES+++++++++++++++++++++++++++
#include "constantes_projeto.h"
#include "funcoes_webserver.h"
#include "funcoes_brain.h"
#include "funcoes_encoder_velocidade.h"
#include "funcoes_controlar_motores.h"
#include "funcoes_lcd_tft.h"

//---------------------------Configs. rede---------------------------------
#define WIFI_SSID "rede_testes"
#define WIFI_PASSWORD "1234567890"
IPAddress local_IP(192, 168, 1, 20);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8); //opcional
IPAddress secondaryDNS(8, 8, 4, 4); //opcional

//---------------------------Instancias objetos (para se poder utilizar entre ficheiros-variável extern)---------------------------------
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);	//lcd-TFT
MFRC522 mfrc522(SS_PIN, RST_PIN);	//Rfid
Ultrasonic ultrasonic(PIN_TRIGGER, PIN_ECHO);	//Ultra-Sons

//---------------------------Estruturas---------------------------------
struct tipo_dados_queue_brain_controlo_motores_0_255 {
	uint8_t codigo_motor_1;
	uint8_t codigo_motor_2;
};

struct tipo_dados_queue_web_brain_cmds {
	String tipo_controlo;
	int valor;
};

struct tipo_dados_queue_encoder_velocidade_web_lcd {
	float velocidade_linear_roda1;
	float velocidade_linear_roda2;
	float velocidade_media_carro;
};

//+++++++++++++++++++++++++++Cabeçalhos interrupcoes+++++++++++++++++++++++++++
static void IRAM_ATTR vInterruptHandler_conta_passos_hall_motor1(void);
static void IRAM_ATTR vInterruptHandler_conta_passos_hall_motor2(void);

//+++++++++++++++++++++++++++Ponteiros para semaforos+++++++++++++++++++++++++++
SemaphoreHandle_t xCountingSemaphore_hall_motor1;
SemaphoreHandle_t xCountingSemaphore_hall_motor2;

SemaphoreHandle_t xMutex_Acesso_barr_SPI;
SemaphoreHandle_t xMutex_Acesso_barr_UART;

//+++++++++++++++++++++++++++Ponteiros para queues+++++++++++++++++++++++++++
QueueHandle_t xQueue_wifi_brain_lcd_estado_ligacao_wifi = NULL;
QueueHandle_t xQueue_web_brain_cmds_controlo_veiculo = NULL;
QueueHandle_t xQueue_brain_controlo_motores_0_255 = NULL;
QueueHandle_t xQueue_encoder_velocidade_web_lcd = NULL;
QueueHandle_t xQueue_obtencao_distancia_web_lcd = NULL;
QueueHandle_t xQueue_Rfid_lcd_estado_autenticacao = NULL;
QueueHandle_t xQueue_brain_controla_led_0_255 = NULL;

//+++++++++++++++++++++++++++descritores das tarefas+++++++++++++++++++++++++++
TaskHandle_t xHandle_task_Web_Server = NULL;

//+++++++++++++++++++++++++++cabeçalhos funcoes que implementam tarefas+++++++++++++++++++++++++++
void vTask_Kepp_Wifi_Live(void *pvParameters);
void vTask_Web_Server(void *pvParameters);
void vTask_Rfid(void *pvParameters);
void vTask_Lcd_Tft(void *pvParameters);
void vTask_Controlo_Motores(void *pvParameters);
void vTask_Brain(void *pvParameters);
void vTask_encoder_velocidade(void *pvParameters);
void vTask_obtencao_distancia(void *pvParameters);
void vTask_controlo_led(void *pvParameters);

//+++++++++++++++++++++++++++ponteiros para variaveis de inicialização+++++++++++++++++++++++++++
const char *pcTextFor_Kepp_Wifi_Live = "WIFI task is running ";
const char *pcTextFor_Web_Server = "WEB_SERVER task is running";
const char *pcTextFor_Rfid = "RFID task is running ";
const char *pcTextFor_Lcd_Tft = "LCD_TFT task is running";
const char *pcTextFor_Controlo_Motores = "CONTROLO_MOTORES task is running";
const char *pcTextFor_Brain = "BRAIN task is running";
const char *pcTextFor_Encoder_Velocidade = "ENCODER_VELOCIDADE task is running";
const char *pcTextFor_Obtencao_Distancia = "OBTENCAO_DISTANCIA task is running";
const char *pcTextFor_Controlo_led = "CONTROLO_LED task is running";

//+++++++++++++++++++++++++++MAIN+++++++++++++++++++++++++++
void setup() {

	//+++++++++++++++++++++++++++alterar prioridade da tarefa main para garantia de eliminação+++++++++++++++++++++++++++
	vTaskPrioritySet(NULL, configMAX_PRIORITIES - 1);

	//+++++++++++++++++++++++++++Config. comunicação série+++++++++++++++++++++++++++
	Serial.begin(115200);

	//+++++++++++++++++++++++++++configuração das interrupcoes+++++++++++++++++++++++++++
	pinMode(PINO_CONTAGEM_PASSOS_HALL_MOTOR1, INPUT);
	pinMode(PINO_CONTAGEM_PASSOS_HALL_MOTOR2, INPUT);

	attachInterrupt(digitalPinToInterrupt(PINO_CONTAGEM_PASSOS_HALL_MOTOR1),
			&vInterruptHandler_conta_passos_hall_motor1, FALLING);

	attachInterrupt(digitalPinToInterrupt(PINO_CONTAGEM_PASSOS_HALL_MOTOR2),
			&vInterruptHandler_conta_passos_hall_motor2, FALLING);

	//+++++++++++++++++++++++++++Variavel auxiliar para verificacao da correta criacao de objetos+++++++++++++++++++++++++++
	bool var_aux_verificacao_criacao_objetos = true;

	//+++++++++++++++++++++++++++Criação dos semáforos/mutexes+++++++++++++++++++++++++++
	xCountingSemaphore_hall_motor1 = xSemaphoreCreateCounting(1000, 0);
	if (xCountingSemaphore_hall_motor1 == NULL) {
		var_aux_verificacao_criacao_objetos = false;
	}

	xCountingSemaphore_hall_motor2 = xSemaphoreCreateCounting(1000, 0);
	if (xCountingSemaphore_hall_motor2 == NULL) {
		var_aux_verificacao_criacao_objetos = false;
	}

	xMutex_Acesso_barr_SPI = xSemaphoreCreateMutex();
	if (xMutex_Acesso_barr_SPI == NULL) {
		var_aux_verificacao_criacao_objetos = false;
	}

	xMutex_Acesso_barr_UART = xSemaphoreCreateMutex();
	if (xMutex_Acesso_barr_UART == NULL) {
		var_aux_verificacao_criacao_objetos = false;
	}

	//+++++++++++++++++++++++++++Criação de queues+++++++++++++++++++++++++++
	xQueue_wifi_brain_lcd_estado_ligacao_wifi = xQueueCreate(1, sizeof(char));
	if (xQueue_wifi_brain_lcd_estado_ligacao_wifi == NULL) {
		var_aux_verificacao_criacao_objetos = false;
	}

	xQueue_web_brain_cmds_controlo_veiculo = xQueueCreate(1,
			sizeof(struct tipo_dados_queue_web_brain_cmds));
	if (xQueue_web_brain_cmds_controlo_veiculo == NULL) {
		var_aux_verificacao_criacao_objetos = false;
	}

	xQueue_brain_controlo_motores_0_255 = xQueueCreate(1,
			sizeof(struct tipo_dados_queue_brain_controlo_motores_0_255));
	if (xQueue_brain_controlo_motores_0_255 == NULL) {
		var_aux_verificacao_criacao_objetos = false;
	}

	xQueue_encoder_velocidade_web_lcd = xQueueCreate(1,
			sizeof(struct tipo_dados_queue_encoder_velocidade_web_lcd));
	if (xQueue_encoder_velocidade_web_lcd == NULL) {
		var_aux_verificacao_criacao_objetos = false;
	}

	xQueue_obtencao_distancia_web_lcd = xQueueCreate(1, sizeof(float));
	if (xQueue_obtencao_distancia_web_lcd == NULL) {
		var_aux_verificacao_criacao_objetos = false;
	}

	xQueue_Rfid_lcd_estado_autenticacao = xQueueCreate(1, sizeof(int));
	if (xQueue_Rfid_lcd_estado_autenticacao == NULL) {
		var_aux_verificacao_criacao_objetos = false;
	}

	xQueue_brain_controla_led_0_255 = xQueueCreate(1, sizeof(int));
	if (xQueue_brain_controla_led_0_255 == NULL) {
		var_aux_verificacao_criacao_objetos = false;
	}

	//+++++++++++++++++++++++++++Criação de tarefas+++++++++++++++++++++++++++
	if (var_aux_verificacao_criacao_objetos == true) {

		xTaskCreatePinnedToCore(vTask_Rfid, "Rfid", 1024,
				(void*) pcTextFor_Rfid, 5, NULL, 1);

		xTaskCreatePinnedToCore(vTask_Lcd_Tft, "Lcd_Tft", 4096,
				(void*) pcTextFor_Lcd_Tft, 3, NULL, 1);

		xTaskCreatePinnedToCore(vTask_Brain, "Brain", 1024,
				(void*) pcTextFor_Brain, 4, NULL, 1);

		xTaskCreatePinnedToCore(vTask_Controlo_Motores, "Controlo_Motores",
				1024, (void*) pcTextFor_Controlo_Motores, 4, NULL, 1);

		xTaskCreatePinnedToCore(vTask_obtencao_distancia, "Obtencao_Distancia",
				1024, (void*) pcTextFor_Obtencao_Distancia, 2, NULL, 1);

		//xTaskCreatePinnedToCore(vTask_Web_Server, "Web_Server",4096, (void*) pcTextFor_Web_Server, 4,&xHandle_task_Web_Server, 1);

		//xTaskCreatePinnedToCore(vTask_Kepp_Wifi_Live,"Kepp_Wifi_Live", 4096,(void*) pcTextFor_Kepp_Wifi_Live, 4, NULL,1);

		xTaskCreatePinnedToCore(vTask_encoder_velocidade, "Encoder_Velocidade",
				1024, (void*) pcTextFor_Encoder_Velocidade, 3, NULL, 1);

		xTaskCreatePinnedToCore(vTask_controlo_led, "Controlo_Led", 1024,
				(void*) pcTextFor_Controlo_led, 2, NULL, 1);

	}interrupts();

}

void loop() {
	vTaskDelete(NULL);
}

//+++++++++++++++++++++++++++TAREFAS+++++++++++++++++++++++++++

void vTask_controlo_led(void *pvParameters) {
	char *pcTaskName;
	TickType_t xLastWakeTime;
	pcTaskName = (char*) pvParameters;
	xLastWakeTime = xTaskGetTickCount();

	BaseType_t xStatus_peek_brain_controla_led_0_255_queue = NULL;

	int freq = 5000;
	int ledChannel = 5;
	int resolution = 8;

	ledcSetup(ledChannel, freq, resolution);
	ledcAttachPin(LED_PIN, ledChannel);

	int valor_intensidade_led_ant = 0;
	int valor_intensidade_led_atual = 0;

	for (;;) {
		xSemaphoreTake(xMutex_Acesso_barr_UART, portMAX_DELAY);
		{
			Serial.print("**************************");
			Serial.print(pcTaskName);
			Serial.println("**************************\n\r");

			xStatus_peek_brain_controla_led_0_255_queue = xQueuePeek(
					xQueue_brain_controla_led_0_255,
					&valor_intensidade_led_atual, 0);

			if (xStatus_peek_brain_controla_led_0_255_queue == pdTRUE) {
				if (valor_intensidade_led_atual != valor_intensidade_led_ant) {

					ledcWrite(ledChannel, valor_intensidade_led_atual);
					Serial.print("PWM: Received = ");
					Serial.println(valor_intensidade_led_atual);
					valor_intensidade_led_ant = valor_intensidade_led_atual;
				}
			} else {
				Serial.print("PWM: Could not receive from the queue.\r\n");
			}
		}
		xSemaphoreGive(xMutex_Acesso_barr_UART);

		vTaskDelayUntil(&xLastWakeTime, (250 / portTICK_PERIOD_MS));
	}
}

void vTask_Web_Server(void *pvParameters) {
	char *pcTaskName;
	TickType_t xLastWakeTime;
	pcTaskName = (char*) pvParameters;
	xLastWakeTime = xTaskGetTickCount();

	BaseType_t xStatus_peek_encoder_velocidade_lcd_queue = NULL;
	BaseType_t xStatus_peek_obtencao_distancia_lcd_queue = NULL;

	float distancia_atual = 0;
	float distancia_ant = 0;
	float velocidade_atual = 0;
	float velocidade_ant = 0;
	tipo_dados_queue_encoder_velocidade_web_lcd dados_velocidades = { NULL,
	NULL, NULL };

	char buffer[50];

//Create AsyncWebServer object on port 80
	AsyncWebServer server(80);
	AsyncWebSocket wsCarInput("/CarInput");

	server.on("/", HTTP_GET, handleRoot);
	server.onNotFound(handleNotFound);
	wsCarInput.onEvent(onCarInputWebSocketEvent);
	server.addHandler(&wsCarInput);
	server.begin();

	for (;;) {
		xSemaphoreTake(xMutex_Acesso_barr_UART, portMAX_DELAY);
		{
			Serial.print("**************************");
			Serial.print(pcTaskName);
			Serial.println("**************************\n\r");
		}
		xSemaphoreGive(xMutex_Acesso_barr_UART);

		xStatus_peek_encoder_velocidade_lcd_queue = xQueuePeek(
				xQueue_encoder_velocidade_web_lcd, &dados_velocidades, 0);
		xStatus_peek_obtencao_distancia_lcd_queue = xQueuePeek(
				xQueue_obtencao_distancia_web_lcd, &distancia_atual, 0);

		if (xStatus_peek_encoder_velocidade_lcd_queue == pdTRUE) {
			if (velocidade_ant != dados_velocidades.velocidade_media_carro) {
				velocidade_atual = dados_velocidades.velocidade_media_carro;
				velocidade_ant = dados_velocidades.velocidade_media_carro;
			}
		}

		if (xStatus_peek_obtencao_distancia_lcd_queue == pdTRUE) {
			if (distancia_ant != distancia_atual) {
				distancia_ant = distancia_atual;
			}
		}

		sprintf(buffer, "{\"velocidade\":\"%.02f\",\"distancia\":\"%.02f\"}",
				velocidade_atual, distancia_atual);
		wsCarInput.textAll(buffer);
		wsCarInput.cleanupClients();
		vTaskDelayUntil(&xLastWakeTime, (100 / portTICK_PERIOD_MS));
	}
}

void vTask_Rfid(void *pvParameters) {
	char *pcTaskName;
	TickType_t xLastWakeTime;
	pcTaskName = (char*) pvParameters;
	xLastWakeTime = xTaskGetTickCount();

	SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN);
	mfrc522.PCD_Init();	// Init MFRC522
	mfrc522.PCD_DumpVersionToSerial();// Show details of PCD - MFRC522 Card Reader details

	int estado_autenticacao = 0;

	for (;;) {

		xSemaphoreTake(xMutex_Acesso_barr_UART, portMAX_DELAY);
		{
			Serial.print("**************************");
			Serial.print(pcTaskName);
			Serial.println("**************************\n\r");

			xSemaphoreTake(xMutex_Acesso_barr_SPI, portMAX_DELAY);
			{
				// Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
				if (mfrc522.PICC_IsNewCardPresent()) {

					// Select one of the cards
					if (mfrc522.PICC_ReadCardSerial()) {

						mfrc522.PICC_ReadCardSerial();
						//Show UID on serial monitor
						Serial.print("UID tag :");
						String content = "";
						byte letter;
						for (byte i = 0; i < mfrc522.uid.size; i++) {
							Serial.print(
									mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
							Serial.print(mfrc522.uid.uidByte[i], HEX);
							content.concat(
									String(
											mfrc522.uid.uidByte[i] < 0x10 ?
													" 0" : " "));
							content.concat(String(mfrc522.uid.uidByte[i], HEX));
						}
						Serial.println();
						Serial.print("Message : ");
						content.toUpperCase();
						if (content.substring(1) == "90 54 A6 20") //change here the UID of the card/cards that you want to give access
								{
							Serial.println("Authorized access");
							estado_autenticacao = AUTENTICACAO_AUTORIZADO;
							xQueueOverwrite(xQueue_Rfid_lcd_estado_autenticacao,
									&estado_autenticacao);
							xSemaphoreGive(xMutex_Acesso_barr_SPI);

							xSemaphoreGive(xMutex_Acesso_barr_UART);
							xTaskCreatePinnedToCore(vTask_Kepp_Wifi_Live,
									"Kepp_Wifi_Live", 4096,
									(void*) pcTextFor_Kepp_Wifi_Live, 4, NULL,
									1);

							vTaskDelete(NULL);
							//delay(250);
						}

						else {
							Serial.println(" Access denied");
							estado_autenticacao = AUTENTICACAO_NAO_AUTORIZADO;
							xQueueOverwrite(xQueue_Rfid_lcd_estado_autenticacao,
									&estado_autenticacao);
							//delay(250);
						}
					}

				} else {
					estado_autenticacao = AUTENTICACAO_APRESENTAR_CARTAO;
					xQueueOverwrite(xQueue_Rfid_lcd_estado_autenticacao,
							&estado_autenticacao);
				}
			}
			xSemaphoreGive(xMutex_Acesso_barr_SPI);
		}
		xSemaphoreGive(xMutex_Acesso_barr_UART);
		vTaskDelayUntil(&xLastWakeTime, (500 / portTICK_PERIOD_MS));
	}
}
void vTask_Lcd_Tft(void *pvParameters) {
	char *pcTaskName;
	TickType_t xLastWakeTime;
	pcTaskName = (char*) pvParameters;
	xLastWakeTime = xTaskGetTickCount();

	tipo_dados_queue_encoder_velocidade_web_lcd dados_velocidades = { NULL,
	NULL, NULL };

	BaseType_t xStatus_peek_encoder_velocidade_lcd_queue = NULL;
	BaseType_t xStatus_peek_obtencao_distancia_lcd_queue = NULL;
	BaseType_t xStatus_peek_estado_autenticacao_queue = NULL;
	BaseType_t xStatus_peek_estado_ligacao_wifi_queue = NULL;

	char estado_ligacao_wifi_atual = 0;
	char estado_ligacao_wifi_ant = 0;
	float distancia_atual = 0;
	float distancia_ant = 0;
	float velocidade_ant = 0;
	int estado_autenticacao = 0;
//+++++++++++++++++ Inicializar o tft+++++++++++++++++++++++++++++++++++
	parametros_estaticos_lcd_tft();
	atualiza_estado_login(AUTENTICACAO_APRESENTAR_CARTAO);

	for (;;) {
		xSemaphoreTake(xMutex_Acesso_barr_UART, portMAX_DELAY);
		{
			Serial.print("**************************");
			Serial.print(pcTaskName);
			Serial.println("**************************\n\r");
		}
		xSemaphoreGive(xMutex_Acesso_barr_UART);

		xStatus_peek_encoder_velocidade_lcd_queue = xQueuePeek(
				xQueue_encoder_velocidade_web_lcd, &dados_velocidades, 0);
		xStatus_peek_obtencao_distancia_lcd_queue = xQueuePeek(
				xQueue_obtencao_distancia_web_lcd, &distancia_atual, 0);
		xStatus_peek_estado_autenticacao_queue = xQueuePeek(
				xQueue_Rfid_lcd_estado_autenticacao, &estado_autenticacao, 0);
		xStatus_peek_estado_ligacao_wifi_queue = xQueuePeek(
				xQueue_wifi_brain_lcd_estado_ligacao_wifi,
				&estado_ligacao_wifi_atual, 0);

		xSemaphoreTake(xMutex_Acesso_barr_SPI, portMAX_DELAY);
		{
			if (xStatus_peek_encoder_velocidade_lcd_queue == pdTRUE) {
				if (velocidade_ant
						!= dados_velocidades.velocidade_media_carro) {
					atualiza_velocidade_TFT(
							dados_velocidades.velocidade_media_carro);
					velocidade_ant = dados_velocidades.velocidade_media_carro;
				}
			}

			if (xStatus_peek_obtencao_distancia_lcd_queue == pdTRUE) {
				if (distancia_ant != distancia_atual) {
					atualiza_distancia_TFT(distancia_atual);
					distancia_ant = distancia_atual;
				}
			}

			if (xStatus_peek_estado_autenticacao_queue == pdTRUE) {
				if (estado_autenticacao == AUTENTICACAO_NAO_AUTORIZADO) {
					atualiza_estado_login(AUTENTICACAO_NAO_AUTORIZADO);
				}
				if (estado_autenticacao == AUTENTICACAO_APRESENTAR_CARTAO) {
					atualiza_estado_login(AUTENTICACAO_APRESENTAR_CARTAO);
				}
				if (estado_autenticacao == AUTENTICACAO_AUTORIZADO) {
					atualiza_estado_login(AUTENTICACAO_AUTORIZADO);
				}
			}

			if (xStatus_peek_estado_ligacao_wifi_queue == pdTRUE) {
				if (estado_ligacao_wifi_atual == WIFI_CONECTION_OK) {
					atualiza_end_ip(WiFi.localIP());
				} else {
					sem_end_ip_atribuido();
				}
			} else {
				sem_end_ip_atribuido();
			}

		}
		xSemaphoreGive(xMutex_Acesso_barr_SPI);

		vTaskDelayUntil(&xLastWakeTime, (500 / portTICK_PERIOD_MS));

	}
}

void vTask_obtencao_distancia(void *pvParameters) {
	char *pcTaskName;
	TickType_t xLastWakeTime;
	pcTaskName = (char*) pvParameters;
	xLastWakeTime = xTaskGetTickCount();

	float distancia = 0.0;
	long duration; // variable for the duration of sound wave travel

	for (;;) {
		xSemaphoreTake(xMutex_Acesso_barr_UART, portMAX_DELAY);
		{
			Serial.print("**************************");
			Serial.print(pcTaskName);
			Serial.println("**************************\n\r");

// Clears the trigPin condition
			digitalWrite(PIN_TRIGGER, LOW);
			delayMicroseconds(2);
// Sets the trigPin HIGH (ACTIVE) for 10 microseconds
			digitalWrite(PIN_TRIGGER, HIGH);
			delayMicroseconds(10);
			digitalWrite(PIN_TRIGGER, LOW);
// Reads the echoPin, returns the sound wave travel time in microseconds
			duration = pulseIn(PIN_ECHO, HIGH);
// Calculating the distance
			distancia = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
// Displays the distance on the Serial Monitor
			Serial.print("Distance: ");
			Serial.print(distancia);
			Serial.println(" cm");
		}
		xSemaphoreGive(xMutex_Acesso_barr_UART);

		xQueueOverwrite(xQueue_obtencao_distancia_web_lcd, &distancia);

//vTaskDelete( NULL);
		vTaskDelayUntil(&xLastWakeTime, (100 / portTICK_PERIOD_MS));

	}
}

void vTask_encoder_velocidade(void *pvParameters) {
	char *pcTaskName;
	TickType_t xLastWakeTime;
	pcTaskName = (char*) pvParameters;
	xLastWakeTime = xTaskGetTickCount();

	int numero_pulsos_motor1 = NULL;
	int numero_pulsos_motor2 = NULL;

	tipo_dados_queue_encoder_velocidade_web_lcd dados_velocidades = { 0, 0, 0 };

	for (;;) {
		xSemaphoreTake(xMutex_Acesso_barr_UART, portMAX_DELAY);
		{
			Serial.print("**************************");
			Serial.print(pcTaskName);
			Serial.println("**************************\n\r");
		}
		xSemaphoreGive(xMutex_Acesso_barr_UART);

		numero_pulsos_motor1 = uxSemaphoreGetCount(
				xCountingSemaphore_hall_motor1);
		numero_pulsos_motor2 = uxSemaphoreGetCount(
				xCountingSemaphore_hall_motor2);

		dados_velocidades.velocidade_linear_roda1 =
				Calcula_velocidade_linear_roda(numero_pulsos_motor1);
		dados_velocidades.velocidade_linear_roda2 =
				Calcula_velocidade_linear_roda(numero_pulsos_motor2);

		dados_velocidades.velocidade_media_carro =
				Calcula_velocidade_media_linear_carro(
						dados_velocidades.velocidade_linear_roda1,
						dados_velocidades.velocidade_linear_roda2);

		xQueueOverwrite(xQueue_encoder_velocidade_web_lcd, &dados_velocidades);

		xQueueReset(xCountingSemaphore_hall_motor1);
		xQueueReset(xCountingSemaphore_hall_motor2);

		vTaskDelayUntil(&xLastWakeTime,
				(PERIODICIDADE_TASK_ENCODER_VELOCIDADE / portTICK_PERIOD_MS));
	}
}

void vTask_Brain(void *pvParameters) {
	char *pcTaskName;
	TickType_t xLastWakeTime;
	pcTaskName = (char*) pvParameters;
	xLastWakeTime = xTaskGetTickCount();

	tipo_dados_queue_web_brain_cmds cmds_recebidos_via_web = { "\0", NULL };
	tipo_dados_queue_brain_controlo_motores_0_255 codigos_enviados_controlo_motores =
			{ 0, 0 };
	char estado_ligacao_wifi = 0;

	BaseType_t xStatus_peek_wifi_brain_estado_ligacao_wifi_queue = NULL;
	BaseType_t xStatus_peek_web_brain_controlo_veiculo_queue = NULL;
	BaseType_t xStatus_overwrite_brain_controlo_motores_0_255_queue = NULL;

	for (;;) {
		xSemaphoreTake(xMutex_Acesso_barr_UART, portMAX_DELAY);
		{
			Serial.print("**************************");
			Serial.print(pcTaskName);
			Serial.println("**************************\n\r");
		}
		xSemaphoreGive(xMutex_Acesso_barr_UART);
//Verificar se há conexao Wifi
		xStatus_peek_wifi_brain_estado_ligacao_wifi_queue = xQueuePeek(
				xQueue_wifi_brain_lcd_estado_ligacao_wifi, &estado_ligacao_wifi,
				0);
		if (xStatus_peek_wifi_brain_estado_ligacao_wifi_queue != pdTRUE) {
			codigos_enviados_controlo_motores.codigo_motor_1 =
			BYTE_ESTADO_TRAVAO;
			codigos_enviados_controlo_motores.codigo_motor_2 =
			BYTE_ESTADO_TRAVAO;
			xStatus_overwrite_brain_controlo_motores_0_255_queue =
					xQueueOverwrite(xQueue_brain_controlo_motores_0_255,
							&codigos_enviados_controlo_motores);
			vTaskDelay(500);
		}

//Caso haja conexão Wifi verificar valores da queue que vem da Web
		if (estado_ligacao_wifi != WIFI_CONECTION_OK) {
			codigos_enviados_controlo_motores.codigo_motor_1 =
			BYTE_ESTADO_TRAVAO;
			codigos_enviados_controlo_motores.codigo_motor_2 =
			BYTE_ESTADO_TRAVAO;
			xStatus_overwrite_brain_controlo_motores_0_255_queue =
					xQueueOverwrite(xQueue_brain_controlo_motores_0_255,
							&codigos_enviados_controlo_motores);
			vTaskDelay(500);
		}

		xStatus_peek_web_brain_controlo_veiculo_queue = xQueuePeek(
				xQueue_web_brain_cmds_controlo_veiculo, &cmds_recebidos_via_web,
				0);

		processa_dados_web(cmds_recebidos_via_web.tipo_controlo,
				cmds_recebidos_via_web.valor);

		vTaskDelayUntil(&xLastWakeTime, (100 / portTICK_PERIOD_MS));
	}
}

void vTask_Controlo_Motores(void *pvParameters) {
	char *pcTaskName;
	TickType_t xLastWakeTime;
	pcTaskName = (char*) pvParameters;
	xLastWakeTime = xTaskGetTickCount();

	configura_pinos_INPUT_OUTPUT_Controlo_motores();
	controlar_motor_1(BYTE_ESTADO_DESTRAVADO);
	controlar_motor_2(BYTE_ESTADO_DESTRAVADO);
	tipo_dados_queue_brain_controlo_motores_0_255 codigos_motores =
			{ NULL, NULL };
	BaseType_t xStatus_peek_brain_contolo_motores_queue;

	for (;;) {
		xSemaphoreTake(xMutex_Acesso_barr_UART, portMAX_DELAY);
		{
			Serial.print("**************************");
			Serial.print(pcTaskName);
			Serial.println("**************************\n\r");
		}
		xSemaphoreGive(xMutex_Acesso_barr_UART);
		xStatus_peek_brain_contolo_motores_queue = xQueuePeek(
				xQueue_brain_controlo_motores_0_255, &codigos_motores, 0);

		if (xStatus_peek_brain_contolo_motores_queue == pdTRUE) {
			Serial.println(codigos_motores.codigo_motor_1);
			Serial.println(codigos_motores.codigo_motor_2);

			controlar_motor_1(codigos_motores.codigo_motor_1);
			controlar_motor_2(codigos_motores.codigo_motor_2);

		} else {
			controlar_motor_1(BYTE_ESTADO_TRAVAO);
			controlar_motor_2(BYTE_ESTADO_TRAVAO);

		}

		vTaskDelayUntil(&xLastWakeTime, (100 / portTICK_PERIOD_MS));
	}
}

void vTask_Kepp_Wifi_Live(void *pvParameters) {
	char *pcTaskName;
	TickType_t xLastWakeTime;
	pcTaskName = (char*) pvParameters;
	xLastWakeTime = xTaskGetTickCount();

	char estado_ligacao_wifi = 0;
	portBASE_TYPE xStatus_estado_ligacao_wifi;

	for (;;) {
		xSemaphoreTake(xMutex_Acesso_barr_UART, portMAX_DELAY);
		{
			Serial.print("**************************");
			Serial.print(pcTaskName);
			Serial.println("**************************\n\r");

			if (WiFi.status() == WL_CONNECTED) {
				//Ligado ao wifi
				Serial.print("WIFI:Ligacao Wifi mantem-se ligada: ");
				Serial.println(WiFi.localIP());
				estado_ligacao_wifi = WIFI_CONECTION_OK;
				xStatus_estado_ligacao_wifi = xQueueOverwrite(
						xQueue_wifi_brain_lcd_estado_ligacao_wifi,
						&estado_ligacao_wifi);
				if (xStatus_estado_ligacao_wifi != pdPASS) {
					//Serial.print("WIFI: Nao deu para enviar o valor para a queue de wifi.\r\n");
				} else {

					//Serial.print("WIFI: Deu para enviar o valor para a queue de wifi.\r\n");
				}

				if (xHandle_task_Web_Server == NULL) {
					xTaskCreatePinnedToCore(vTask_Web_Server, "Web_Server",
							4096, (void*) pcTextFor_Web_Server, 4,
							&xHandle_task_Web_Server, 1);
				}

			} else {
				//Nao ligado ao Wifi
				if (!WiFi.config(local_IP, gateway, subnet, primaryDNS,
						secondaryDNS)) {
					Serial.println("STA Failed to configure");
				}

				Serial.println("WIFI:Connecting to WiFi..");
				WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
				estado_ligacao_wifi = WIFI_CONECTION_FAIL;
				xStatus_estado_ligacao_wifi = xQueueOverwrite(
						xQueue_wifi_brain_lcd_estado_ligacao_wifi,
						&estado_ligacao_wifi);
				if (xStatus_estado_ligacao_wifi != pdPASS) {
					//Serial.print("WIFI: Nao deu para enviar o valor para a queue de wifi.\r\n");
				} else {
					//Serial.print(	"WIFI: Deu para enviar o valor para a queue de wifi.\r\n");
				}

				if (xHandle_task_Web_Server != NULL) {
					vTaskDelete(xHandle_task_Web_Server);
					xHandle_task_Web_Server = NULL;
				}
			}
		}
		xSemaphoreGive(xMutex_Acesso_barr_UART);
		vTaskDelay(WIFI_PERIODICIDADE_VERIFICACAO_MS);
	}
}

static void IRAM_ATTR vInterruptHandler_conta_passos_hall_motor1(void) {
	static portBASE_TYPE xHigherPriorityTaskWoken;

	xHigherPriorityTaskWoken = pdFALSE;

//incrementamos o contador do semaforo
	xSemaphoreGiveFromISR(xCountingSemaphore_hall_motor1,
			(BaseType_t* )&xHigherPriorityTaskWoken);
	if (xHigherPriorityTaskWoken == pdTRUE) {
		portYIELD_FROM_ISR();
//vPortYield();
	}
}

static void IRAM_ATTR vInterruptHandler_conta_passos_hall_motor2(void) {
	static portBASE_TYPE xHigherPriorityTaskWoken;

	xHigherPriorityTaskWoken = pdFALSE;

//incrementamos o contador do semaforo
	xSemaphoreGiveFromISR(xCountingSemaphore_hall_motor2,
			(BaseType_t* )&xHigherPriorityTaskWoken);

	if (xHigherPriorityTaskWoken == pdTRUE) {
		portYIELD_FROM_ISR();
//vPortYield();
	}

}
