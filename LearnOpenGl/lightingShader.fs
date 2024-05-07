#version 330 core
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 

uniform Material material;
uniform vec3 lightColor;

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec3 LightPos;

void main()
{
	// ambient
	vec3 ambient = lightColor * material.ambient;
	
	// diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(LightPos- FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = (diff * material.diffuse)* lightColor;
	
	// specular
	vec3 viewDir = normalize(- FragPos);
	vec3 reflectdir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectdir), 0.0), material.shininess);
	vec3 specular = (spec*material.specular)*lightColor;

	vec3 result = (ambient + diffuse + specular);
	FragColor = vec4(result, 1.0);
};