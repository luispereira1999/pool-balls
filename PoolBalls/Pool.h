/*
 * @descrição	Ficheiro com todas as assinaturas relativas à biblioteca Pool.
 * @ficheiro	Pool.h
 * @autor(s)	Henrique Azevedo a23488, Luís Pereira a18446, Pedro Silva a20721, Vânia Pereira a19264
 * @data		11/06/2023
*/


#pragma once

#ifndef POOL_BALLS_H
#define POOL_BALLS_H 1

#pragma region importações

#include <iostream>
#include <vector>

#define GLEW_STATIC
#include <GL\glew.h>

#define GLFW_USE_DWM_SWAP_INTERVAL
#include <GLFW\glfw3.h>

#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\matrix_inverse.hpp>

#pragma endregion


namespace Pool {

#pragma region declarações da biblioteca

	// estrutura para armazenados dados dos ficheiros .mtl
	typedef struct {
		float ns;				// expoente especular (brilho do objeto)
		glm::vec3 ka;			// coeficiente de reflexão da luz ambiente
		glm::vec3 kd;			// coeficiente de reflexão da luz difusa
		glm::vec3 ks;			// coeficiente de reflexão da luz especular 
		std::string map_kd;		// nome do ficheiro da imagem de textura
	} Material;

	// estrutura para armazenados dados das texturas
	typedef struct {
		int width;				// largura da textura
		int height;				// altura da textura
		int nChannels;			// número de canais de cores
		unsigned char* image;	// imagem da textura
	} Texture;

	// variáveis globais
	extern GLuint _programShader;
	extern glm::mat4 _modelMatrix;
	extern glm::mat4 _viewMatrix;
	extern glm::mat4 _projectionMatrix;
	extern glm::mat3 _normalMatrix;

	// funções globais da biblioteca
	void bindProgramShader(GLuint* programShader);
	void sendAttributesToProgramShader(GLuint* programShader);
	void sendUniformsToProgramShader(GLuint* programShader, glm::mat4* modelMatrix, glm::mat4* viewMatrix, glm::mat4* modelViewMatrix, glm::mat4* projectionMatrix, glm::mat3* normalMatrix);

	// classe para renderizar bolas
	class RendererBall {
	private:
		// atributos privados
		int _id;	// identificador único para depois saber qual a unidade de textura que pertence, entre outros dados, que este seja útil
		glm::vec3 _position;
		glm::vec3 _orientation;

		const char* _objFilepath;
		std::vector<float>* _vertices;
		GLuint* _vao;
		GLuint* _vbo;
		Material* _material;
		Texture* _texture;

	public:
		// getters - definir valores de atributos fora da classe
		const std::vector<float>& getVertices() const;
		const Material& getMaterial() const;
		glm::vec3 getPosition() const;
		glm::vec3 getOrientation() const;

		// setters - obter valores de atributos fora da classe
		void setId(int id);
		void setPosition(glm::vec3 position);
		void setOrientation(glm::vec3 orientation);

		// construtor
		RendererBall();

		// destrutor
		~RendererBall();

		// principais
		void Read(const std::string obj_model_filepath);
		void Send(void);
		void Draw(glm::vec3 position, glm::vec3 orientation);

		// secundárias
		std::vector<float>* load3dModel(const char* objFilepath);
		std::string getMtlFromObj(const char* objFilepath);
		Material* loadMaterial(const char* mtlFilename);
		Texture* loadTexture(std::string imageFilename);
		void loadMaterialLighting(GLuint programShader, Material material);
	};

#pragma endregion

}

#endif