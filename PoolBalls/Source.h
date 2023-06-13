/*
 * @descri��o	Ficheiro com todas as assinaturas relativas � aplica��o em si.
 * @ficheiro	Source.h
 * @autor(s)	Henrique Azevedo a23488, Lu�s Pereira a18446, Pedro Silva a20721, V�nia Pereira a19264
 * @data		11/06/2023
*/


#pragma once

#ifndef SOURCE_H
#define SOURCE_H 1

#pragma region importa��es

#include <GLFW\glfw3.h>

#pragma endregion


#pragma region constantes

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define SCREEN_NAME "Bilhar"

#pragma endregion


#pragma region fun��es do programa

	void init(void);
	void display(void);
	void loadSceneLighting(void);
	bool isColliding(void);

#pragma endregion


#pragma region fun��es de callbacks para a biblioteca glfw

	void printErrorCallback(int code, const char* description);
	void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	void mouseCallback(GLFWwindow* window, double xpos, double ypos);
	void charCallback(GLFWwindow* window, unsigned int codepoint);

#pragma endregion

#endif