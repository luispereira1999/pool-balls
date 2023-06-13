/*
 * @descrição	Ficheiro com todas as assinaturas relativas ao carregamento de shaders.
 * @ficheiro	Shaders.h
 * @autor(s)	Henrique Azevedo a23488, Luís Pereira a18446, Pedro Silva a20721, Vânia Pereira a19264
 * @data		11/06/2023
*/


#pragma once

#ifndef LOAD_SHADERS_H
#define LOAD_SHADERS_H 1

#pragma region importações

#include <GL\gl.h>

#pragma endregion


#pragma region estruturas

typedef struct {
	GLenum type;
	const char* filename;
	GLuint shader;
} ShaderInfo;

#pragma endregion


#pragma region funções

static const GLchar* readShader(const char* filename);
GLuint loadShaders(ShaderInfo* shaders);

#pragma endregion

#endif