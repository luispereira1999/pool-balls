/*
 * @descrição	Ficheiro principal da aplicação, com o ponto de entrada e, as variáveis e funções gerais da aplicação.
 * @ficheiro	Source.cpp
 * @autor(s)	Henrique Azevedo a23488, Luís Pereira a18446, Pedro Silva a20721, Vânia Pereira a19264
 * @data		11/06/2023
 *
 * -------------------------------------
 *
 * Proposta de software para a renderização de um cenário 3D, com a parecença de uma mesa de bilhar e suas bolas,
 * aplicando texturas e iluminação, e realizar animação de uma das bolas.
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

#include "Source.h"
#include "Shaders.h"
#include "Pool.h"

#pragma endregion


#pragma region variáveis globais

// mesa
const GLuint _numberOfTableVertices = 36;
GLfloat _tableVertices[_numberOfTableVertices * 8];
GLuint _tableVAO;
GLuint _tableVBO;

// bolas
const int _numberOfBalls = 15;
Pool::RendererBall _rendererBalls[_numberOfBalls];

// câmara
GLfloat _angle = -10.0f;
glm::vec3 _cameraPosition = glm::vec3(0.0f, 0.0f, 5.0f);

// eventos do mouse
float _lastX = 0.0f;
float _lastY = 0.0f;
bool _firstMouse = true;

// animação de uma bola
int _animatedBallIndex = 4;
bool _animationStarted = false;
bool _animationFinished = false;

#pragma endregion


#pragma region ponto de entrada do programa

int main()
{
	// para quando houver algum erro com a glfw
	glfwSetErrorCallback(printErrorCallback);

	// inicializa a glfw
	if (!glfwInit()) {
		return -1;
	}

	// cria janela
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_NAME, NULL, NULL);
	if (window == nullptr) {
		std::cout << "Erro ao inicializar a biblioteca GLFW" << std::endl;
		glfwTerminate();
		return -1;
	}

	// define o contexto atual para a renderização da janela
	glfwMakeContextCurrent(window);

	// inicializa a glew
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cout << "Erro ao inicializar a biblioteca GLEW" << std::endl;
		glfwTerminate();
		return -1;
	}

	// inicializa a cena pela primeira vez
	init();

	// quando o utilizador faz scroll com o mouse
	glfwSetScrollCallback(window, scrollCallback);

	// quando o utilizador muda a visão da câmara
	glfwSetCursorPosCallback(window, mouseCallback);

	// quando é pressionada uma tecla
	glfwSetCharCallback(window, charCallback);

	// mantém a janela aberta e atualizada
	while (!glfwWindowShouldClose(window))
	{
		// renderiza os objetos na cena
		display();

		// troca os buffers de renderização (da frame antiga para a nova)
		glfwSwapBuffers(window);

		// processa todos os eventos ocorridos
		glfwPollEvents();
	}

	// termina todas as instâncias da glfw
	glfwTerminate();

	return 1;
}

#pragma endregion


#pragma region funções do programa

void init(void) {
	// -----------------------------------------------------------
	// Carregar dados da mesa para CPU
	// -----------------------------------------------------------

	// posição da mesa
	float xCoord = 1.25f;
	float yCoord = 0.25f;
	float zCoord = 1.25f;

	// cores da mesa
	std::vector<glm::vec3> colors{
		glm::vec3(1.0f, 0.0f, 0.0f),	   // vermelho
			glm::vec3(1.0f, 1.0f, 0.0f),   // amarelo
			glm::vec3(0.0f, 1.0f, 0.0f),   // verde
			glm::vec3(0.0f, 1.0f, 1.0f),   // ciano
			glm::vec3(0.0f, 0.0f, 1.0f),   // azul
			glm::vec3(1.0f, 0.0f, 1.0f)	   // magenta
	};

	// coordenadas de textura da mesa
	float xTexture = 0.0f;
	float yTexture = 0.0f;

	// cria atributos dos vértices da mesa
	GLfloat _tableVertices[_numberOfTableVertices * 8] = {
		// -----------------------------------------------------------
		// X+ (face 1)
		// -----------------------------------------------------------
		// Primeiro triângulo
		// Posições						Cores											Coordenadas de textura
		xCoord, -yCoord,  zCoord,		colors[0][0], colors[0][1], colors[0][2],		xTexture, yTexture,
		xCoord, -yCoord, -zCoord,		colors[0][0], colors[0][1], colors[0][2],       xTexture, yTexture,
		xCoord,  yCoord,  zCoord,		colors[0][0], colors[0][1], colors[0][2],       xTexture, yTexture,
		// Segundo triângulo
		xCoord,  yCoord,  zCoord,		colors[0][0], colors[0][1], colors[0][2],       xTexture, yTexture,
		xCoord, -yCoord, -zCoord,		colors[0][0], colors[0][1], colors[0][2],       xTexture, yTexture,
		xCoord,  yCoord, -zCoord,		colors[0][0], colors[0][1], colors[0][2],       xTexture, yTexture,

		// -----------------------------------------------------------
		// X- (face 2)
		// -----------------------------------------------------------
		// Primeiro triângulo
		-xCoord, -yCoord, -zCoord,		colors[1][0], colors[1][1], colors[1][2],		xTexture, yTexture,
		-xCoord, -yCoord,  zCoord,		colors[1][0], colors[1][1], colors[1][2],		xTexture, yTexture,
		-xCoord,  yCoord, -zCoord,		colors[1][0], colors[1][1], colors[1][2],		xTexture, yTexture,
		// Segundo triângulo
		-xCoord,  yCoord, -zCoord,		colors[1][0], colors[1][1], colors[1][2],		xTexture, yTexture,
		-xCoord, -yCoord,  zCoord,		colors[1][0], colors[1][1], colors[1][2],		xTexture, yTexture,
		-xCoord,  yCoord,  zCoord,		colors[1][0], colors[1][1], colors[1][2],		xTexture, yTexture,

		// -----------------------------------------------------------
		// Y+ (face 3)
		// -----------------------------------------------------------
		// Primeiro triângulo
		-xCoord, yCoord, zCoord,		colors[2][0], colors[2][1], colors[2][2],      xTexture, yTexture,
		xCoord, yCoord, zCoord,			colors[2][0], colors[2][1], colors[2][2],      xTexture, yTexture,
		-xCoord, yCoord, -zCoord,		colors[2][0], colors[2][1], colors[2][2],      xTexture, yTexture,
		// Segundo triângulo
		-xCoord, yCoord, -zCoord,		colors[2][0], colors[2][1], colors[2][2],      xTexture, yTexture,
		xCoord, yCoord, zCoord,			colors[2][0], colors[2][1], colors[2][2],      xTexture, yTexture,
		xCoord, yCoord, -zCoord,		colors[2][0], colors[2][1], colors[2][2],      xTexture, yTexture,

		// -----------------------------------------------------------
		// Y- (face 4)
		// -----------------------------------------------------------
		// Primeiro triângulo
		-xCoord, -yCoord, -zCoord,		colors[3][0], colors[3][1], colors[3][2],		xTexture, yTexture,
		xCoord, -yCoord, -zCoord,		colors[3][0], colors[3][1], colors[3][2],		xTexture, yTexture,
		-xCoord, -yCoord, zCoord,		colors[3][0], colors[3][1], colors[3][2],		xTexture, yTexture,
		// Segundo triângulo
		-xCoord, -yCoord, zCoord,		colors[3][0], colors[3][1], colors[3][2],		xTexture, yTexture,
		xCoord, -yCoord, -zCoord,		colors[3][0], colors[3][1], colors[3][2],		xTexture, yTexture,
		xCoord, -yCoord, zCoord,		colors[3][0], colors[3][1], colors[3][2],		xTexture, yTexture,

		// -----------------------------------------------------------
		// Z+ (face 5)
		// -----------------------------------------------------------
		// Primeiro triângulo
		-xCoord, -yCoord, zCoord,		colors[4][0], colors[4][1], colors[4][2],      xTexture, yTexture,
		xCoord, -yCoord, zCoord,		colors[4][0], colors[4][1], colors[4][2],      xTexture, yTexture,
		-xCoord, yCoord, zCoord,		colors[4][0], colors[4][1], colors[4][2],      xTexture, yTexture,
		// Segundo triângulo
		-xCoord, yCoord, zCoord,		colors[4][0], colors[4][1], colors[4][2],      xTexture, yTexture,
		xCoord, -yCoord, zCoord,		colors[4][0], colors[4][1], colors[4][2],      xTexture, yTexture,
		xCoord,  yCoord, zCoord,		colors[4][0], colors[4][1], colors[4][2],      xTexture, yTexture,

		// -----------------------------------------------------------
		// Z- (face 6)
		// -----------------------------------------------------------
		// Primeiro triângulo
		xCoord, -yCoord, -zCoord,		colors[5][0], colors[5][1], colors[5][2],      xTexture, yTexture,
		-xCoord, -yCoord, -zCoord,		colors[5][0], colors[5][1], colors[5][2],      xTexture, yTexture,
		xCoord, yCoord, -zCoord,		colors[5][0], colors[5][1], colors[5][2],      xTexture, yTexture,
		// Segundo triângulo
		xCoord, yCoord, -zCoord,		colors[5][0], colors[5][1], colors[5][2],      xTexture, yTexture,
		-xCoord, -yCoord, -zCoord,		colors[5][0], colors[5][1], colors[5][2],      xTexture, yTexture,
		-xCoord, yCoord, -zCoord,		colors[5][0], colors[5][1], colors[5][2],      xTexture, yTexture
	};


	// -----------------------------------------------------------
	// Enviar dados da mesa para GPU
	// -----------------------------------------------------------

	// gera o nome para o VAO da mesa
	glGenVertexArrays(1, &_tableVAO);

	// vincula o VAO ao contexto OpenGL atual
	glBindVertexArray(_tableVAO);

	// gera o nome para o VBO da mesa
	glGenBuffers(1, &_tableVBO);

	// vincula o VBO ao contexto OpenGL atual
	glBindBuffer(GL_ARRAY_BUFFER, _tableVBO);

	// inicializa o VBO atualmente ativo com dados imutáveis
	glBufferStorage(GL_ARRAY_BUFFER, sizeof(_tableVertices), _tableVertices, 0);

	// ativar atributos das posições dos vértices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	// ativar atributos das cores dos vértices
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// ativar atributos das coordenadas de textura dos vértices
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	// desvincula o VAO atual
	glBindVertexArray(_tableVAO);


	// -----------------------------------------------------------
	// Carregar dados das bolas para CPU
	// -----------------------------------------------------------

	// posições das bolas
	std::vector<glm::vec3> _positions = {
		glm::vec3(1.1f, 0.33f, 1.1f),		// bola 1
		glm::vec3(-1.1f, 0.33f, -1.1f),		// bola 2
		glm::vec3(-1.1f, 0.33f, 1.1f),		// bola 3
		glm::vec3(1.1f, 0.33f, -1.1f),		// bola 4
		glm::vec3(0.1f, 0.33f, -0.1f),		// bola 5
		glm::vec3(-0.3f, 0.33f, -0.3f),		// bola 6
		glm::vec3(-0.6f, 0.33f, -0.4f),		// bola 7
		glm::vec3(0.8f, 0.33f, 0.7f),		// bola 8
		glm::vec3(-0.8f, 0.33f, -0.2f),		// bola 9
		glm::vec3(0.3f, 0.33f, 0.7f),		// bola 10
		glm::vec3(-0.2f, 0.33f, -0.8f),		// bola 11
		glm::vec3(0.7f, 0.33f, 0.5f),		// bola 12
		glm::vec3(-0.9f, 0.33f, 0.6f),		// bola 13
		glm::vec3(0.1f, 0.33f, 0.3f),		// bola 14
		glm::vec3(0.4f, 0.33f, -0.6f),		// bola 15
	};

	// rotações das bolas
	std::vector<glm::vec3> _orientations = {
		glm::vec3(0.0f),		// bola 1
		glm::vec3(0.0f),		// bola 2
		glm::vec3(0.0f),		// bola 3
		glm::vec3(0.0f),		// bola 4
		glm::vec3(0.0f),		// bola 5
		glm::vec3(0.0f),		// bola 6
		glm::vec3(0.0f),		// bola 7
		glm::vec3(0.0f),		// bola 8
		glm::vec3(0.0f),		// bola 9
		glm::vec3(0.0f),		// bola 10
		glm::vec3(0.0f),		// bola 11
		glm::vec3(0.0f),		// bola 12
		glm::vec3(0.0f),		// bola 13
		glm::vec3(0.0f),		// bola 14
		glm::vec3(0.0f),		// bola 15
	};

	// para cada bola, define um identificador único, posição e orientação,
	// carrega o modelo, material e textura e envia os dados para a GPU,
	for (int i = 0; i < _numberOfBalls; i++) {
		_rendererBalls[i].setId(i + 1);

		std::string objFilepath = "textures/Ball" + std::to_string(i + 1) + ".obj";
		_rendererBalls[i].Read(objFilepath);

		_rendererBalls[i].setPosition(_positions[i]);
		_rendererBalls[i].setOrientation(_orientations[i]);


		// -----------------------------------------------------------
		// Enviar dados das bolas para GPU
		// -----------------------------------------------------------

		_rendererBalls[i].Send();
	}


	// -----------------------------------------------------------
	// Carregar shaders para CPU
	// -----------------------------------------------------------

	// informações dos shaders
	ShaderInfo shaders[] = {
		{ GL_VERTEX_SHADER,   "shaders/Pool.vert" },
		{ GL_FRAGMENT_SHADER, "shaders/Pool.frag" },
		{ GL_NONE, NULL }
	};

	// carrega shaders
	Pool::_programShader = loadShaders(shaders);

	// se houve erros ao carregar shaders
	if (!Pool::_programShader) {
		std::cout << "Erro ao carregar shaders: " << std::endl;
		exit(EXIT_FAILURE);
	}


	// -----------------------------------------------------------
	// Envia shaders para GPU
	// -----------------------------------------------------------

	// vincula o programa shader
	Pool::bindProgramShader(&Pool::_programShader);

	// atribui os atributos dos vértices ao programa shader
	Pool::sendAttributesToProgramShader(&Pool::_programShader);

	// matrizes de transformação
	Pool::_modelMatrix = glm::rotate(glm::mat4(1.0f), _angle, glm::vec3(0.0f, 1.0f, 0.15f));
	Pool::_viewMatrix = glm::lookAt(
		_cameraPosition,				// posição da câmara
		glm::vec3(0.0f, 0.0f, 0.0f),	// para onde está a "olhar"
		glm::vec3(0.0f, 1.0f, 0.0f)		// orientação vertical na qual a câmera está a "olha"
	);
	glm::mat4 modelViewMatrix = Pool::_viewMatrix * Pool::_modelMatrix;
	Pool::_projectionMatrix = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	Pool::_normalMatrix = glm::inverseTranspose(glm::mat3(modelViewMatrix));

	// atribui as matrizes de transformação ao programa shader
	Pool::sendUniformsToProgramShader(&Pool::_programShader, &Pool::_modelMatrix, &Pool::_viewMatrix, &modelViewMatrix, &Pool::_projectionMatrix, &Pool::_normalMatrix);

	// carrega os diferentes tipos de luzes da cena
	loadSceneLighting();


	// -----------------------------------------------------------
	// Configurar janela de renderização
	// -----------------------------------------------------------

	// define a janela de renderização
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// ativa o teste de profundidade e o descarte de polígonos não observáveis
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void display(void) {
	// -----------------------------------------------------------
	// Limpar buffers
	// -----------------------------------------------------------

	// limpa o buffer de cor e de profundidade
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// -----------------------------------------------------------
	// Desenhar mesa
	// -----------------------------------------------------------

	// translação da mesa
	glm::mat4 translatedModel = glm::translate(Pool::_modelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));

	// modelo de visualização do objeto
	glm::mat4 modelView = Pool::_viewMatrix * translatedModel;

	// obtém a localização do uniform
	GLint modelViewId = glGetProgramResourceLocation(Pool::_programShader, GL_UNIFORM, "ModelView");

	// atribui o valor ao uniform
	glProgramUniformMatrix4fv(Pool::_programShader, modelViewId, 1, GL_FALSE, glm::value_ptr(modelView));

	// define que a mesa não tem textura (valor 0)
	GLint isRenderTexture = glGetProgramResourceLocation(Pool::_programShader, GL_UNIFORM, "isRenderTexture");
	glProgramUniform1i(Pool::_programShader, isRenderTexture, 0);

	GLint viewPositionLoc = glGetUniformLocation(Pool::_programShader, "viewPosition");
	glUniform3f(viewPositionLoc, _cameraPosition.x, _cameraPosition.y, _cameraPosition.z);

	// desenha a mesa na tela
	glBindVertexArray(_tableVAO);
	glDrawArrays(GL_TRIANGLES, 0, _numberOfTableVertices);


	// -----------------------------------------------------------
	// Desenhar bolas
	// -----------------------------------------------------------

	// desenha para cada bola
	for (int i = 0; i < _numberOfBalls; i++) {
		_rendererBalls[i].Draw(_rendererBalls[i].getPosition(), _rendererBalls[i].getOrientation());
	}


	// -----------------------------------------------------------
	// Animação de uma bola
	// -----------------------------------------------------------

	// se animação da bola iniciou
	if (_animationStarted && !_animationFinished) {
		// move a bola em X e Z
		_rendererBalls[_animatedBallIndex].setPosition(glm::vec3(
			_rendererBalls[_animatedBallIndex].getPosition().x + 0.001f,
			_rendererBalls[_animatedBallIndex].getPosition().y,
			_rendererBalls[_animatedBallIndex].getPosition().z + 0.001f
		));

		// roda a bola em X, Y e Z
		_rendererBalls[_animatedBallIndex].setOrientation(_rendererBalls[_animatedBallIndex].getOrientation() + 2.0f);

		// se colidiu com outro objeto
		if (isColliding()) {
			_animationStarted = false;
			_animationFinished = true;
			std::cout << "Colidiu com bola ou mesa." << std::endl;
		}
	}
}

void loadSceneLighting(void) {
	// fonte de luz ambiente
	glProgramUniform3fv(Pool::_programShader, glGetProgramResourceLocation(Pool::_programShader, GL_UNIFORM, "ambientLight.ambient"), 1, glm::value_ptr(glm::vec3(7.0f)));

	// fonte de luz direcional
	glProgramUniform3fv(Pool::_programShader, glGetProgramResourceLocation(Pool::_programShader, GL_UNIFORM, "directionalLight.direction"), 1, glm::value_ptr(glm::vec3(1.0f, 0.0f, 0.0f)));
	glProgramUniform3fv(Pool::_programShader, glGetProgramResourceLocation(Pool::_programShader, GL_UNIFORM, "directionalLight.ambient"), 1, glm::value_ptr(glm::vec3(4.0f)));
	glProgramUniform3fv(Pool::_programShader, glGetProgramResourceLocation(Pool::_programShader, GL_UNIFORM, "directionalLight.diffuse"), 1, glm::value_ptr(glm::vec3(2.0f)));
	glProgramUniform3fv(Pool::_programShader, glGetProgramResourceLocation(Pool::_programShader, GL_UNIFORM, "directionalLight.specular"), 1, glm::value_ptr(glm::vec3(1.0f)));

	// fonte de luz pontual 
	glProgramUniform3fv(Pool::_programShader, glGetProgramResourceLocation(Pool::_programShader, GL_UNIFORM, "pointLight.position"), 1, glm::value_ptr(glm::vec3(1.0f, 0.0f, 0.0f)));
	glProgramUniform3fv(Pool::_programShader, glGetProgramResourceLocation(Pool::_programShader, GL_UNIFORM, "pointLight.ambient"), 1, glm::value_ptr(glm::vec3(6.0f)));
	glProgramUniform3fv(Pool::_programShader, glGetProgramResourceLocation(Pool::_programShader, GL_UNIFORM, "pointLight.diffuse"), 1, glm::value_ptr(glm::vec3(2.0f)));
	glProgramUniform3fv(Pool::_programShader, glGetProgramResourceLocation(Pool::_programShader, GL_UNIFORM, "pointLight.specular"), 1, glm::value_ptr(glm::vec3(1.0f)));
	glProgramUniform1f(Pool::_programShader, glGetProgramResourceLocation(Pool::_programShader, GL_UNIFORM, "pointLight.constant"), 1.0f);
	glProgramUniform1f(Pool::_programShader, glGetProgramResourceLocation(Pool::_programShader, GL_UNIFORM, "pointLight.linear"), 0.06f);
	glProgramUniform1f(Pool::_programShader, glGetProgramResourceLocation(Pool::_programShader, GL_UNIFORM, "pointLight.quadratic"), 0.02f);

	// fonte de luz cónica
	glProgramUniform3fv(Pool::_programShader, glGetProgramResourceLocation(Pool::_programShader, GL_UNIFORM, "spotLight.position"), 1, glm::value_ptr(glm::vec3(0.0f, 2.2f, 0.0f)));
	glProgramUniform3fv(Pool::_programShader, glGetProgramResourceLocation(Pool::_programShader, GL_UNIFORM, "spotLight.direction"), 1, glm::value_ptr(glm::vec3(0.0f, -0.1f, 0.0f)));
	glProgramUniform3fv(Pool::_programShader, glGetProgramResourceLocation(Pool::_programShader, GL_UNIFORM, "spotLight.ambient"), 1, glm::value_ptr(glm::vec3(5.0f)));
	glProgramUniform3fv(Pool::_programShader, glGetProgramResourceLocation(Pool::_programShader, GL_UNIFORM, "spotLight.diffuse"), 1, glm::value_ptr(glm::vec3(1.0f)));
	glProgramUniform3fv(Pool::_programShader, glGetProgramResourceLocation(Pool::_programShader, GL_UNIFORM, "spotLight.specular"), 1, glm::value_ptr(glm::vec3(1.0f)));
	glProgramUniform1f(Pool::_programShader, glGetProgramResourceLocation(Pool::_programShader, GL_UNIFORM, "spotLight.constant"), 1.0f);
	glProgramUniform1f(Pool::_programShader, glGetProgramResourceLocation(Pool::_programShader, GL_UNIFORM, "spotLight.linear"), 0.09f);
	glProgramUniform1f(Pool::_programShader, glGetProgramResourceLocation(Pool::_programShader, GL_UNIFORM, "spotLight.quadratic"), 0.032f);
	glProgramUniform1f(Pool::_programShader, glGetProgramResourceLocation(Pool::_programShader, GL_UNIFORM, "spotLight.cutOff"), glm::cos(glm::radians(20.0f)));
	glProgramUniform1f(Pool::_programShader, glGetProgramResourceLocation(Pool::_programShader, GL_UNIFORM, "spotLight.outerCutOff"), glm::cos(glm::radians(30.0f)));

	// define a luz padrão apresentada
	glProgramUniform1i(Pool::_programShader, glGetProgramResourceLocation(Pool::_programShader, GL_UNIFORM, "lightModel"), 1);
}

bool isColliding(void) {
	float _ballRadius = 0.08f;

	for (int i = 0; i < _numberOfBalls; i++) {
		if (i != _animatedBallIndex) {
			float distance = glm::distance(_rendererBalls[i].getPosition(), _rendererBalls[_animatedBallIndex].getPosition());

			// se colidiu com alguma bola
			if (distance <= 2 * _ballRadius) {
				return true;
			}
		}
	}

	// se colidiu com os limites da mesa
	if (_rendererBalls[_animatedBallIndex].getPosition().x + _ballRadius >= 1.25f || _rendererBalls[_animatedBallIndex].getPosition().x - _ballRadius <= -1.25f ||
		_rendererBalls[_animatedBallIndex].getPosition().z + _ballRadius >= 1.25f || _rendererBalls[_animatedBallIndex].getPosition().z - _ballRadius <= -1.25f) {
		return true;
	}

	return false;
}

#pragma endregion


#pragma region funções de callbacks para a biblioteca glfw

void printErrorCallback(int code, const char* description) {
	std::cout << description << std::endl;
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	float zoomLevel = 1.0f;
	float minZoom = 0.1f;
	float maxZoom = 10.0f;
	float zoomSpeed = 0.1f;

	// converte a posição do mouse em coordenadas normalizadas
	int width, height;
	glfwGetWindowSize(window, &width, &height);

	// calcula o zoom com base no deslocamento do scrol
	float zoomFactor = 1.0f + static_cast<float>(yoffset) * zoomSpeed;

	Pool::_viewMatrix = glm::scale(Pool::_viewMatrix, glm::vec3(zoomFactor, zoomFactor, zoomFactor));

	zoomLevel += yoffset * zoomSpeed;
	zoomLevel = std::max(minZoom, std::min(maxZoom, zoomLevel));
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) != GLFW_PRESS)
	{
		_lastX = xpos;
		_lastY = ypos;
		_firstMouse = false;
		return;		// sai da função se o botão do mouse esquerdo não estiver pressionado
	}

	if (_firstMouse)
	{
		_lastX = xpos;
		_lastY = ypos;
		_firstMouse = false;
	}

	float xOffset = xpos - _lastX;
	float yOffset = ypos - _lastY;

	_lastX = xpos;
	_lastY = ypos;

	float sensitivity = 0.1f;

	// aplica rotação horizontal 
	Pool::_viewMatrix = glm::rotate(Pool::_viewMatrix, glm::radians(xOffset), glm::vec3(0.0f, 1.0f, 0.0f));

	// calcula o vetor "direito" da câmera
	glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(Pool::_viewMatrix[2])));

	// aplica rotação vertical 
	Pool::_viewMatrix = glm::rotate(Pool::_viewMatrix, glm::radians(yOffset), right);
}

void charCallback(GLFWwindow* window, unsigned int codepoint)
{
	int lightModel;

	// deteta as teclas do teclado
	switch (codepoint)
	{
	case '1':
		lightModel = 1;
		glProgramUniform1i(Pool::_programShader, glGetProgramResourceLocation(Pool::_programShader, GL_UNIFORM, "lightModel"), lightModel);
		std::cout << "Luz ambiente ativada." << std::endl;
		break;

	case '2':
		lightModel = 2;
		glProgramUniform1i(Pool::_programShader, glGetProgramResourceLocation(Pool::_programShader, GL_UNIFORM, "lightModel"), lightModel);
		std::cout << "Luz direcional ativada." << std::endl;
		break;

	case '3':
		lightModel = 3;
		glProgramUniform1i(Pool::_programShader, glGetProgramResourceLocation(Pool::_programShader, GL_UNIFORM, "lightModel"), lightModel);
		std::cout << "Luz pontual ativada." << std::endl;
		break;

	case '4':
		lightModel = 4;
		glProgramUniform1i(Pool::_programShader, glGetProgramResourceLocation(Pool::_programShader, GL_UNIFORM, "lightModel"), lightModel);
		std::cout << "Luz conica ativada." << std::endl;
		break;

	case GLFW_KEY_SPACE:
		// não permite voltar a iniciar animação, depois de iniciar uma vez e terminar
		if (!_animationFinished) {
			_animationStarted = true;
			std::cout << "Animacao da bola iniciada." << std::endl;
		}
		else {
			std::cout << "Animacao da bola encerrada." << std::endl;
		}

		break;

	default:
		break;
	}
}

#pragma endregion