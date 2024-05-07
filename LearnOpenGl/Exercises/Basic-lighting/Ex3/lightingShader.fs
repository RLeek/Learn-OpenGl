#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec3 LightPos;

uniform vec3 objectColor;
uniform vec3 lightColor;

uniform vec3 viewPos;

void main()
{
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor;

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(LightPos- FragPos);
	float diffuseStrength = 1.0;
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * diffuseStrength* lightColor;

	float specularStrength = 0.5;
	vec3 viewDir = normalize(- FragPos);
	vec3 reflectdir = reflect(-lightDir, norm);

	float spec = pow(max(dot(viewDir, reflectdir), 0.0), 32);
	vec3 specular = specularStrength*spec*lightColor;


	vec3 result = (ambient + diffuse + specular) * objectColor;
	FragColor = vec4(result, 1.0);
};