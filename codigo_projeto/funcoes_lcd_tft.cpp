/*
 * funcoes_lcd_tft.cpp
 *
 *  Created on: 09/12/2022
 *      Author: Dinis
 */

#include "funcoes_lcd_tft.h"

void atualiza_end_ip(IPAddress end_ip) {
	tft.setTextSize(2);
	tft.setCursor(47, 187);
	tft.setTextColor(ILI9341_PURPLE, ILI9341_CYAN);
	tft.print("               ");
	tft.setCursor(47, 187);
	tft.print(end_ip);

}


void sem_end_ip_atribuido(void) {
	tft.setTextSize(2);
	tft.setCursor(47, 187);
	tft.setTextColor(ILI9341_PURPLE, ILI9341_CYAN);
	tft.print("               ");
	tft.setCursor(47, 187);
	tft.print("Sem conexao");

}
void atualiza_estado_login(char codigo) {

	switch (codigo) {

	case AUTENTICACAO_APRESENTAR_CARTAO:
		tft.setTextSize(2);
		tft.setCursor(15, 127);
		tft.setTextColor(ILI9341_BLUE, ILI9341_DARKGREY);
		tft.print("                  ");
		tft.setCursor(15, 127);
		tft.print("Apresentar cartao");

		break;

	case AUTENTICACAO_NAO_AUTORIZADO:
		tft.setTextSize(2);
		tft.setCursor(15, 127);
		tft.setTextColor(ILI9341_RED, ILI9341_DARKGREY);
		tft.print("                  ");
		tft.setCursor(15, 127);
		tft.print("  Nao autorizado");
		break;

	case AUTENTICACAO_AUTORIZADO:
		tft.setTextSize(2);
		tft.setCursor(15, 127);
		tft.setTextColor(ILI9341_GREEN, ILI9341_DARKGREY);
		tft.print("                  ");
		tft.setCursor(15, 127);
		tft.print("    Autorizado");
		break;

	default:

		break;
	}
}

void parametros_estaticos_lcd_tft(void) {

	tft.begin();
	tft.fillScreen(ILI9341_BLACK);
	tft.setRotation(2);

	tft.setCursor(20, 10);
	tft.setTextColor(ILI9341_GREENYELLOW);
	tft.setTextSize(2);
	tft.println("  Projeto SEEV");

	tft.setTextColor(ILI9341_WHITE);
	tft.setTextSize(2);
	tft.println("	    2022/2023 ");

	tft.drawRect(5, 5, 230, 70, ILI9341_RED);
	tft.drawRect(6, 6, 228, 68, ILI9341_RED); //Aumentar espessura

	tft.fillRoundRect(5, 100, 230, 56, 10, ILI9341_DARKGREY);
	tft.setTextColor(ILI9341_LIGHTGREY);
	tft.setCursor(50, 107);
	tft.print("Estado Login:");

	tft.fillRoundRect(5, 160, 230, 56, 10, ILI9341_CYAN);
	tft.setTextColor(ILI9341_PURPLE);
	tft.setCursor(50, 167);
	tft.print("Endereco IP:");

	tft.setTextColor(ILI9341_BLACK);
	tft.fillRoundRect(5, 260, 155, 28, 10, ILI9341_GREEN);
	tft.setCursor(15, 267);
	tft.print("VELOCIDADE");
	tft.setTextSize(1);
	tft.println(" m/s");

	tft.fillRoundRect(5, 290, 155, 28, 10, ILI9341_GREEN);
	tft.setCursor(15, 297);
	tft.setTextSize(2);
	tft.print("DISTANCIA");
	tft.setTextSize(1);
	tft.println("   cm");

}

void atualiza_velocidade_TFT(float velocidade) {

	tft.setCursor(179, 267);
	tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
	tft.print("     ");
	tft.setTextSize(2);

	if (velocidade > 100) {
		tft.setCursor(179, 267);
		tft.print(" --- ");

	} else {
		tft.setCursor(180, 267);
		tft.print(velocidade);
	}

}

void atualiza_distancia_TFT(float distancia) {

	tft.setCursor(179, 297);
	tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
	tft.print("     "); // Limpa área de escrita
	tft.setTextSize(2);

	if (distancia > 100) {
		tft.setCursor(179, 297);
		tft.print(" --- ");

	} else {
		tft.setCursor(180, 297);
		tft.print(distancia);
	}

}
