#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 Color;

uniform vec3 lightPos;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position=projection*view*model*vec4(aPos, 1.0);
	vec3 FragPos = vec3(view *model * vec4(aPos, 1.0));
	vec3 Normal= mat3(transpose(inverse(view)) * transpose(inverse(model)))*aNormal;
	vec3 LightPos = vec3(view * vec4(lightPos,1.0));



	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor;

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(LightPos- FragPos);
	float diffuseStrength = 1.0;
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * diffuseStrength* lightColor;

	float specularStrength = 1.0;
	vec3 viewDir = normalize(- FragPos);
	vec3 reflectdir = reflect(-lightDir, norm);

	float spec = pow(max(dot(viewDir, reflectdir), 0.0), 32);
	vec3 specular = specularStrength*spec*lightColor;

	vec3 result = (ambient + diffuse + specular) * objectColor;
	Color = result;

};