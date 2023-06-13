/*
 * @descrição	Ficheiro com todo o código relativo ao carregamento de shaders.
 * @ficheiro	Shaders.cpp
 * @autor(s)	Henrique Azevedo a23488, Luís Pereira a18446, Pedro Silva a20721, Vânia Pereira a19264
 * @data		11/06/2023
*/


#pragma region importações

#include <iostream>
#include <fstream>

#define GLEW_STATIC
#include <GL\glew.h>

#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\matrix_inverse.hpp>

#include "Shaders.h"

#pragma endregion


#pragma region funções

static const GLchar* readShader(const char* filename) {
	// abre o ficheiro em modo binário e coloca-se na última posição do ficheiro
	std::ifstream ficheiro(filename, std::ifstream::ate | std::ifstream::binary);

	// se houve erros ao abrir o ficheiro
	if (!ficheiro.is_open()) {
		std::cerr << "Erro ao abrir o ficheiro '" << filename << "'" << std::endl;
		return nullptr;
	}

	// tamanho do ficheiro (por isso a vantagem de iniciar na última posição)
	std::streampos fileLengthInBytes = ficheiro.tellg();

	// reposiciona a leitura do ficheiro no início
	ficheiro.seekg(0, std::ios::beg);

	// aloca memória para os dados do ficheiro
	GLchar* source = new GLchar[int(fileLengthInBytes) + 1];

	// lê o ficheiro
	ficheiro.read(source, fileLengthInBytes);

	// fecha a string
	source[fileLengthInBytes] = 0;

	// fecha o ficheiro
	ficheiro.close();

	// retorna o endereço de memória da string
	return const_cast<const GLchar*>(source);
}

GLuint loadShaders(ShaderInfo* shaders) {
	// se não forem passados shaders
	if (shaders == nullptr) {
		return -1;
	}

	// cria um objeto programa
	GLuint program = glCreateProgram();

	// para cada objeto shader
	for (GLint i = 0; shaders[i].type != GL_NONE; i++) {
		// cria um objeto shader
		shaders[i].shader = glCreateShader(shaders[i].type);

		// lê o código do shader
		const GLchar* source = readShader(shaders[i].filename);

		// se houve erros com algum shader
		if (source == nullptr) {
			// destroi os shaders criados
			for (int j = 0; shaders[j].type != GL_NONE; j++) {
				// se tem um shader válido
				if (shaders[j].shader != 0) {
					glDeleteShader(shaders[j].shader);
				}

				shaders[j].shader = 0;
			}

			return -1;
		}

		// carrega o código do shader
		glShaderSource(shaders[i].shader, 1, &source, nullptr);
		delete[] source;

		// compila o shader
		glCompileShader(shaders[i].shader);

		// obtém a informação sobre o estado da compilação do shader
		GLint compiled;
		glGetShaderiv(shaders[i].shader, GL_COMPILE_STATUS, &compiled);

		// se houve erros ao compilar
		if (!compiled) {
			// tamanho do bufer do log
			GLsizei length;
			glGetShaderiv(shaders[i].shader, GL_INFO_LOG_LENGTH, &length);

			// texto da mensagem do log
			GLchar* log = new GLchar[length + 1];
			glGetShaderInfoLog(shaders[i].shader, length, &length, log);
			std::cerr << "Erro ao compilar shaders: " << log << std::endl;
			delete[] log;

			// destroi os shaders criados
			for (int j = 0; shaders[j].type != GL_NONE; j++) {
				// se tem um shader válido
				if (shaders[j].shader != 0) {
					glDeleteShader(shaders[j].shader);
				}

				shaders[j].shader = 0;
			}

			return -1;
		}

		// anexa o objeto shader ao programa shader
		glAttachShader(program, shaders[i].shader);
	}

	// linka o programa (cria um "executável")
	glLinkProgram(program);

	// obtém a informação sobre o estado da linkagem do programa
	GLint linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);

	// se houve erros ao linkar
	if (!linked) {
		// tamanho do bufer do log
		GLsizei length;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

		// texto da mensagem do log
		GLchar* log = new GLchar[length + 1];
		glGetProgramInfoLog(program, length, &length, log);
		std::cerr << "Erro ao linkar shaders: " << log << std::endl;
		delete[] log;

		// destroi os shaders criados
		for (int j = 0; shaders[j].type != GL_NONE; j++) {
			// se tem um shader válido
			if (shaders[j].shader != 0) {
				glDeleteShader(shaders[j].shader);
			}

			shaders[j].shader = 0;
		}

		return -1;
	}

	return program;
}

#pragma endregion