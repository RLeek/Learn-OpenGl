#version 330 core
struct Material {
	sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
}; 

// Note: Not really a fan of this representation
// As it seems to imply that light has seperate colors
// for its ambient/diffuse/specular components. This 
// might be good for customization, but really it should
// just be one color + strength, with ambient/diffuse/specular
// being globally set. As it currently stands, the values are 
// doing multiple things at once.
struct Light {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

void main()
{
	// ambient
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
	  
	// diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position- FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * vec3(texture(material.diffuse, TexCoords))* light.diffuse;
	 
	// specular
	vec3 viewDir = normalize(viewPos- FragPos);
	vec3 reflectdir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectdir), 0.0), material.shininess);
	vec3 specular = spec*(vec3(texture(material.specular, TexCoords)))*light.specular;
	
	vec3 result = (ambient + diffuse + specular) + vec3(texture(material.emission, TexCoords));
	FragColor = vec4(result, 1.0);
};