/*
 * funcoes_webserver.h
 *
 *  Created on: 18/11/2022
 *      Author: Dinis
 */

#ifndef FUNCOES_WEBSERVER_H_
#define FUNCOES_WEBSERVER_H_

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <stdio.h>
#include "constantes_projeto.h"
#include "funcoes_controlar_motores.h"

extern QueueHandle_t xQueue_web_brain_cmds_controlo_veiculo;


void handleRoot(AsyncWebServerRequest *request);
void handleNotFound(AsyncWebServerRequest *request);
void onCarInputWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);



#endif /* FUNCOES_WEBSERVER_H_ */
