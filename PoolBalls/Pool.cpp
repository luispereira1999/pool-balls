/*
 * @descrição	Ficheiro com todo o código relativo à biblioteca Pool.
 * @ficheiro	Pool.cpp
 * @autor(s)	Henrique Azevedo a23488, Luís Pereira a18446, Pedro Silva a20721, Vânia Pereira a19264
 * @data		11/06/2023
*/


#pragma region importações

#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")

#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#define GLEW_STATIC
#include <GL\glew.h>

#define GLFW_USE_DWM_SWAP_INTERVAL
#include <GLFW\glfw3.h>

#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\matrix_inverse.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include "thirdParty/TinyObjLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "thirdParty/StbImage.h"

#include "Shaders.h"
#include "Pool.h"
#include "Source.h"

#pragma endregion


namespace Pool {

#pragma region variáveis globais

	GLuint _programShader;
	glm::mat4 _modelMatrix;
	glm::mat4 _viewMatrix;
	glm::mat4 _projectionMatrix;
	glm::mat3 _normalMatrix;

#pragma endregion


#pragma region funções globais da biblioteca Pool

	void bindProgramShader(GLuint* programShader) {
		// vincula o programa shader ao contexto OpenGL atual
		glUseProgram(*programShader);
	}

	void sendAttributesToProgramShader(GLuint* programShader) {
		// obtém as localizações dos atributos no programa shader
		GLint positionId = glGetProgramResourceLocation(*programShader, GL_PROGRAM_INPUT, "vPosition");
		GLint normalId = glGetProgramResourceLocation(*programShader, GL_PROGRAM_INPUT, "vNormal");
		GLint textCoordId = glGetProgramResourceLocation(*programShader, GL_PROGRAM_INPUT, "vTextureCoord");

		// faz a ligação entre os atributos do programa shader aos VAOs e VBO ativos 
		glVertexAttribPointer(positionId, 3 /*3 elementos por vértice*/, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glVertexAttribPointer(normalId, 3 /*3 elementos por cor*/, GL_FLOAT, GL_TRUE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glVertexAttribPointer(textCoordId, 2 /*3 elementos por coordenadas da textura*/, GL_FLOAT, GL_TRUE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

		// ativa os atributos do programa shader
		glEnableVertexAttribArray(positionId);
		glEnableVertexAttribArray(normalId);
		glEnableVertexAttribArray(textCoordId);
	}

	void sendUniformsToProgramShader(
		GLuint* programShader,
		glm::mat4* modelMatrix,
		glm::mat4* viewMatrix,
		glm::mat4* modelViewMatrix,
		glm::mat4* projectionMatrix,
		glm::mat3* normalMatrix)
	{
		// obtém as localizações dos uniforms no programa shader
		GLint modelId = glGetProgramResourceLocation(*programShader, GL_UNIFORM, "Model");
		GLint viewId = glGetProgramResourceLocation(*programShader, GL_UNIFORM, "View");
		GLint modelViewId = glGetProgramResourceLocation(*programShader, GL_UNIFORM, "ModelView");
		GLint projectionId = glGetProgramResourceLocation(*programShader, GL_UNIFORM, "Projection");
		GLint normalViewId = glGetProgramResourceLocation(*programShader, GL_UNIFORM, "NormalMatrix");

		// atribui o valor aos uniforms do programa shader
		glProgramUniformMatrix4fv(*programShader, modelId, 1, GL_FALSE, glm::value_ptr(*modelMatrix));
		glProgramUniformMatrix4fv(*programShader, viewId, 1, GL_FALSE, glm::value_ptr(*viewMatrix));
		glProgramUniformMatrix4fv(*programShader, modelViewId, 1, GL_FALSE, glm::value_ptr(*modelViewMatrix));
		glProgramUniformMatrix4fv(*programShader, projectionId, 1, GL_FALSE, glm::value_ptr(*projectionMatrix));
		glProgramUniformMatrix3fv(*programShader, normalViewId, 1, GL_FALSE, glm::value_ptr(*normalMatrix));
	}

#pragma endregion


#pragma region funções getters e setters da classe RendererBall

	// getters
	const std::vector<float>& RendererBall::getVertices() const {   // retorna apontador para ser mais eficiente ao renderizar
		return *_vertices;
	}

	const Material& RendererBall::getMaterial() const {
		return *_material;
	}

	glm::vec3 RendererBall::getPosition()const {
		return _position;
	}

	glm::vec3 RendererBall::getOrientation() const {
		return _orientation;
	}

	// setters
	void RendererBall::setId(int id) {
		_id = id;
	}

	void RendererBall::setPosition(glm::vec3 position) {
		_position = position;
	}

	void RendererBall::setOrientation(glm::vec3 orientation) {
		_orientation = orientation;
	}

#pragma endregion


#pragma region construtor e destrutor da classe RendererBall

	// construtor
	RendererBall::RendererBall(void) {
		_id = 0;
		_objFilepath = new char;
		_vertices = new std::vector<float>;
		_vao = new GLuint;
		_vbo = new GLuint;
		_material = new Material;
		_texture = new Texture;
	}

	// destrutor
	RendererBall::~RendererBall(void) {
		// liberta memória
		delete _vao;
		delete _vbo;
		delete _material;
		delete _texture;
	}

#pragma endregion


#pragma region funções principais da classe RendererBall

	void RendererBall::Read(const std::string obj_model_filepath) {
		_objFilepath = obj_model_filepath.c_str();

		// armazena o modelo 3D
		_vertices = load3dModel(_objFilepath);

		// armazena o material
		std::string mtlFilename = getMtlFromObj(_objFilepath);
		_material = loadMaterial(mtlFilename.c_str());

		// armazena a textura
		std::string textureFilename = _material->map_kd;
		_texture = loadTexture(textureFilename);
	}

	void RendererBall::Send(void) {
		// gera o nome para o VAO da bola
		glGenVertexArrays(1, _vao);

		// vincula o VAO da bola ao contexto OpenGL atual
		glBindVertexArray(*_vao);

		// gera o nome para o VBO da bola
		glGenBuffers(1, _vbo);

		// para cada vértice da da bola
		for (int i = 0; i < _vertices->size(); i++) {
			// vincula o VBO ao contexto OpenGL atual
			glBindBuffer(GL_ARRAY_BUFFER, *_vbo);

			// inicializa o VBO atualmente ativo com dados imutáveis
			glBufferStorage(GL_ARRAY_BUFFER, _vertices->size() * sizeof(float), _vertices->data(), 0);

			// ativa atributos das posições dos vértices
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
			glEnableVertexAttribArray(0);

			// ativa atributos das cores dos vértices
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
			glEnableVertexAttribArray(1);

			// ativa atributos das coordenadas de textura dos vértices
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
			glEnableVertexAttribArray(2);
		
			// desvincula o VAO atual
			glBindVertexArray(0);
		}

		// gera o nome para a textura
		GLuint textureName;
		glGenTextures(1, &textureName);

		// ativa a unidade de textura atual (inicia na unidade 0)
		glActiveTexture(GL_TEXTURE0 + (_id - 1));

		// vincula um nome de textura ao target GL_TEXTURE_2D da unidade de textura ativa
		glBindTexture(GL_TEXTURE_2D, textureName);

		// define os parâmetros de filtragem (wrapping e ajuste de tamanho)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// carrega os dados da imagem para o objeto de textura vinculado ao target GL_TEXTURE_2D da unidade de textura ativa
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _texture->width, _texture->height, 0, _texture->nChannels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, _texture->image);

		// gera o mipmap para essa textura
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	void RendererBall::Draw(glm::vec3 position, glm::vec3 orientation) {
		// atualizar valores de iluminação do objeto
		Material* material = _material;
		loadMaterialLighting(_programShader, *material);

		// translação da bola
		glm::mat4 translatedModel = glm::translate(_modelMatrix, position);

		// rotação da bola em torno do eixo Z
		glm::mat4 rotatedModel = glm::rotate(translatedModel, glm::radians(orientation.z), glm::vec3(0.0f, 0.0f, 1.0f));	// rotação no eixo z
		rotatedModel = glm::rotate(rotatedModel, glm::radians(orientation.y), glm::vec3(0.0f, 1.0f, 0.0f));					// rotação no eixo y
		rotatedModel = glm::rotate(rotatedModel, glm::radians(orientation.x), glm::vec3(1.0f, 0.0f, 0.0f));					// rotação no eixo x

		// escala de cada bola
		glm::mat4 scaledModel = glm::scale(rotatedModel, glm::vec3(0.08f));

		// modelo de visualização do objeto
		glm::mat4 modelView = _viewMatrix * scaledModel;

		// obtém a localização do uniform
		GLint modelViewId = glGetProgramResourceLocation(_programShader, GL_UNIFORM, "ModelView");

		// atribui o valor ao uniform
		glProgramUniformMatrix4fv(_programShader, modelViewId, 1, GL_FALSE, glm::value_ptr(modelView));

		// define que a mesa tem textura (valor 1)
		GLint isRenderTexture = glGetProgramResourceLocation(_programShader, GL_UNIFORM, "isRenderTexture");
		glProgramUniform1i(_programShader, isRenderTexture, 1);

		GLint locationTexSampler1 = glGetProgramResourceLocation(_programShader, GL_UNIFORM, "sampler");
		glProgramUniform1i(_programShader, locationTexSampler1, (_id - 1)/*unidade de textura*/);

		// desenha a bola na tela
		glBindVertexArray(0);
		glDrawArrays(GL_TRIANGLES, 0, _vertices->size() / 8);
	}

#pragma endregion


#pragma region funções secundárias da classe RendererBall

	std::vector<float>* RendererBall::load3dModel(const char* objFilepath) {
		std::vector<float>* vertices = new 	std::vector<float>;

		tinyobj::attrib_t attributes;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warning, error;

		// se houve erros ao carregar o ficheiro .obj
		if (!tinyobj::LoadObj(&attributes, &shapes, &materials, &warning, &error, objFilepath)) {
			std::cout << warning << error << '\n';
			return nullptr;
		}

		// lê atributos do modelo 3D
		for (const auto& shape : shapes) {
			for (const auto& index : shape.mesh.indices) {
				glm::vec4 pos = {
					attributes.vertices[3 * index.vertex_index],
					attributes.vertices[3 * index.vertex_index + 1],
					attributes.vertices[3 * index.vertex_index + 2],
					1
				};

				glm::vec3 normal = {
					attributes.normals[3 * index.normal_index],
					attributes.normals[3 * index.normal_index + 1],
					attributes.normals[3 * index.normal_index + 2]
				};

				glm::vec2 textCoord = {
					attributes.texcoords[2 * index.texcoord_index],
					attributes.texcoords[2 * index.texcoord_index + 1]
				};

				vertices->push_back(pos.x);
				vertices->push_back(pos.y);
				vertices->push_back(pos.z);
				vertices->push_back(normal.x);
				vertices->push_back(normal.y);
				vertices->push_back(normal.z);
				vertices->push_back(textCoord.x);
				vertices->push_back(textCoord.y);
			}
		}

		return vertices;
	}

	std::string RendererBall::getMtlFromObj(const char* objFilepath) {
		std::ifstream file(objFilepath);
		std::string line;
		std::string mtlFilename;

		while (std::getline(file, line)) {
			std::istringstream stream(line);
			std::string prefix;
			stream >> prefix;

			if (prefix == "mtllib") {
				stream >> mtlFilename;
				break;
			}
		}

		return mtlFilename;
	}

	Material* RendererBall::loadMaterial(const char* mtlFilename) {
		std::string directory = "textures/";
		std::ifstream mtlFile(directory + mtlFilename);

		// se houve erros ao carregar o ficheiro .mtl
		if (!mtlFile) {
			std::cerr << "Erro ao carregar ficheiro .mtl." << std::endl;
			return {};
		}

		std::string line;
		Pool::Material* material = new Material;

		// lê cada linha do ficheiro .mtl e guarda os respetivos dados
		while (std::getline(mtlFile, line)) {
			// ignora linhas vazias ou comentadas
			if (line.empty() || line[0] == '#') {
				continue;
			}

			std::istringstream stream(line);
			std::string command;
			stream >> command;

			// brilho
			if (command == "Ns") {
				float value;
				stream >> value;

				material->ns = value;
			}
			// cor ambiente
			else if (command == "Ka") {
				float r, g, b;
				stream >> r >> g >> b;

				material->ka = glm::vec3(r, g, b);
			}
			// cor difusa
			else if (command == "Kd") {
				float r, g, b;
				stream >> r >> g >> b;

				material->kd = glm::vec3(r, g, b);
			}
			// cor especular
			else if (command == "Ks") {
				float r, g, b;
				stream >> r >> g >> b;

				material->ks = glm::vec3(r, g, b);
			}
			// textura
			else if (command == "map_Kd") {
				std::string texture;
				stream >> texture;

				material->map_kd = texture;
			}
		}

		return material;
	}

	Texture* RendererBall::loadTexture(std::string imageFilename) {
		Texture* texture = new Texture;
		int width, height, nChannels;

		// lê o ficheiro da imagem
		std::string directory = "textures/";
		std::string fullPath = directory + imageFilename;
		unsigned char* image = stbi_load(fullPath.c_str(), &width, &height, &nChannels, 0);

		// se houve erros ao abrir o ficheiro
		if (!image) {
			std::cerr << "Erro ao abrir o ficheiro '" << fullPath.c_str() << "'." << std::endl;
			return {};
		}

		texture->width = width;
		texture->height = height;
		texture->nChannels = nChannels;
		texture->image = image;

		return texture;
	}

	void RendererBall::loadMaterialLighting(GLuint programShader, Material material) {
		glProgramUniform1f(programShader, glGetProgramResourceLocation(programShader, GL_UNIFORM, "material.shininess"), material.ns);
		glProgramUniform3fv(programShader, glGetProgramResourceLocation(programShader, GL_UNIFORM, "material.ambient"), 1, glm::value_ptr(material.ka));
		glProgramUniform3fv(programShader, glGetProgramResourceLocation(programShader, GL_UNIFORM, "material.diffuse"), 1, glm::value_ptr(material.kd));
		glProgramUniform3fv(programShader, glGetProgramResourceLocation(programShader, GL_UNIFORM, "material.specular"), 1, glm::value_ptr(material.ks));
	}

#pragma endregion

}