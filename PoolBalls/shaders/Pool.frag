/*
 * @descrição	Ficheiro relativo ao fragment shader.
 * @ficheiro	Pool.frag
 * @autor(s)	Henrique Azevedo a23488, Luís Pereira a18446, Pedro Silva a20721, Vânia Pereira a19264
 * @data		11/06/2023
*/


#version 440 core

uniform mat4 Model;
uniform mat4 View;
uniform mat4 ModelView;
uniform mat3 NormalMatrix;
uniform int lightModel;
uniform sampler2D sampler;
uniform int isRenderTexture;
uniform vec3 viewPosition;

layout(location = 0) in vec3 color;
layout(location = 1) in vec2 textureCoord;

layout(location = 0) out vec4 fColor;	// cor final do fragmento
layout(location = 2) in vec3 vPositionEyeSpace;
layout(location = 3) in vec3 vNormalEyeSpace;
layout(location = 4) in vec3 textureVector;
layout(location = 5) in vec3 fPosition;

// estrutura da fonte de luz ambiente
struct AmbientLight {
	vec3 ambient;		// componente de luz ambiente
};

// estrutura de uma fonte de luz direcional
struct DirectionalLight	{
	vec3 direction;		// direção da luz
	
	vec3 ambient;		// componente de luz ambiente
	vec3 diffuse;		// componente de luz difusa
	vec3 specular;		// componente de luz especular
};

// estrutura de uma fonte de luz pontual
struct PointLight {
	vec3 position;		// posição do ponto de luz
	
	vec3 ambient;		// componente de luz ambiente
	vec3 diffuse;		// componente de luz difusa
	vec3 specular;		// componente de luz especular
	
	float constant;		// atenuação constante
	float linear;		// atenuação linear
	float quadratic;	// atenuação quadrática
};

// estrutura de uma fonte de luz cónica
struct SpotLight {
	vec3 position;		// posição do ponto de luz
	vec3 direction;		// direção da luz, espaço do mundo

	vec3 ambient;		// componente de luz ambiente
	vec3 diffuse;		// componente de luz difusa
	vec3 specular;		// componente de luz especular
	
	float constant;		// atenuação constante
	float linear;		// atenuação linear
	float quadratic;	// atenuação quadrática

	float cutOff, outerCutOff;	// ângulo de abertura
};

// estrutura do material do objeto
struct Material {
	float shininess;	// ns
	vec3 ambient;		// Ka
	vec3 diffuse;		// kd
	vec3 specular;		// ks
};

uniform AmbientLight ambientLight;			// fonte de luz ambiente
uniform DirectionalLight directionalLight;	// fonte de luz direcional
uniform PointLight pointLight;				// fonte de luz pontual
uniform SpotLight spotLight;				// fonte de luz cónica
uniform Material material;					// material do objeto

vec4 calcAmbientLight(AmbientLight light);
vec4 calcDirectionalLight(DirectionalLight light);
vec4 calcPointLight(PointLight light);
vec4 calcSpotLight(SpotLight light);
vec4 calcSpotLight2(SpotLight light);

void main() {
	vec4 lightToUse;

	// verifica qual a luz atual
	if (lightModel == 2) {
		lightToUse = calcDirectionalLight(directionalLight);
	} else if (lightModel == 3) {
		lightToUse = calcPointLight(pointLight);
	} else if (lightModel == 4) {
		lightToUse = calcSpotLight(spotLight);
	} else {
		lightToUse = calcAmbientLight(ambientLight);
	}

	// se tem textura (bola)
	if (isRenderTexture == 1) {
		vec4 texColor = texture(sampler, textureCoord);
		fColor = lightToUse * texColor;
	} else {   // se não tem textura (mesa)
		fColor = lightToUse * vec4(color, 1.0f);
	}
}

vec4 calcAmbientLight(AmbientLight light) {
	// cálculo da contribuição da luz ambiente
	vec4 ambient = vec4(material.ambient * light.ambient, 1.0);

	// retorna a fonte de luz ambiente
	return ambient;
}

vec4 calcDirectionalLight(DirectionalLight light) {
	// cálculo da contribuição da luz ambiente
	vec4 ambient = vec4(material.ambient * light.ambient, 1.0);

	// cálculo da contribuição da luz difusa
	vec3 lightDirectionEyeSpace = (View * vec4(light.direction, 0.0)).xyz;
	vec3 L = normalize(-lightDirectionEyeSpace);
	vec3 N = normalize(vNormalEyeSpace);
	float NdotL = max(dot(N, L), 0.0);
	vec4 diffuse = vec4(material.diffuse * light.diffuse, 1.0) * NdotL;

	// cálculo da contribuição da luz especular
	vec3 V = normalize(-vPositionEyeSpace);
	vec3 R = reflect(-L, N);
	float RdotV = max(dot(R, V), 0.0);
	vec4 specular = pow(RdotV, material.shininess) * vec4(light.specular * material.specular, 1.0);

	// retorna a fonte de luz direcional
	return ambient + diffuse + specular;
}

vec4 calcPointLight(PointLight light) {
	// cálculo da contribuição da luz ambiente
	vec4 ambient = vec4(material.ambient * light.ambient, 1.0);

	// cálculo da contribuição da luz difusa
	vec3 lightPositionEyeSpace = (View * vec4(light.position, 1.0)).xyz;
	vec3 L = normalize(lightPositionEyeSpace - vPositionEyeSpace);
	vec3 N = normalize(vNormalEyeSpace);
	float NdotL = max(dot(N, L), 0.0);
	vec4 diffuse = vec4(material.diffuse * light.diffuse, 1.0) * NdotL;

	// cálculo da contribuição da luz especular
	vec3 V = normalize(-vPositionEyeSpace);
	vec3 R = reflect(-L, N);
	float RdotV = max(dot(R, V), 0.0);
	vec4 specular = pow(RdotV, material.shininess) * vec4(light.specular * material.specular, 1.0);
	
	// atenuação
	float distance = length(mat3(View) * light.position - vPositionEyeSpace);	// cálculo da distância entre o ponto de luz e o vértice
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	// retorna a fonte de luz pontual
	return (attenuation * (ambient + diffuse + specular));
}

vec4 calcSpotLight(SpotLight light) {
	// cálculo da contribuição da luz ambiente
    vec3 ambient =  material.ambient * light.ambient;

	// cálculo da contribuição da luz difusa
    vec3 norm = normalize(color);
    vec3 lightDir = normalize(light.position - fPosition);
	float diffuseIntensity = max(dot(norm, lightDir), 0.0);
	float smoothDiffuse = smoothstep(light.outerCutOff, light.cutOff, diffuseIntensity);
	vec3 diffuse = light.diffuse * smoothDiffuse;

	// cálculo da contribuição da luz especular
	vec3 viewDir = normalize(viewPosition - fPosition);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	float smoothSpecular = smoothstep(light.outerCutOff, light.cutOff, diffuseIntensity);
	vec3 specular = light.specular * spec * smoothSpecular;

	// atenuação
	float distance = length(light.position - fPosition);
	float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	ambient  *= attenuation;
	diffuse  *= attenuation;
	specular *= attenuation;

	// retorna a fonte de luz cónica
    return vec4(ambient + diffuse + specular, 1.0f);
}