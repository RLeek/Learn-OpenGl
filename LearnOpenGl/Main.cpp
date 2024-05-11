#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "camera.h"
#include <vector>
#include "Model.h"
using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
unsigned int createShaderProgram(const char* fragmentShaderSource);
unsigned int createVAO(float verticies[], int vertexSize);
void mouse_callback(GLFWwindow* window, double xPos, double yPos);
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);

// Camera values
Camera camera = Camera();

// Mouse values
float lastX = 400;
float lastY = 300;
bool firstMouse = true;

// Frame values
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// If the angle for the inner cutoff is larger than the outer cutoff, this inverts
// the light such that the centre is dark whilst the outside is bright. This is
// because the epsilon value is now positive instead of negative, resulting 
// in the value of the intensity being negative before being clamped to 0.
// Values outside of that range become positive rather than negative, 
// resulting in the rest of the cone being bright. 

// Note: That this occurs in cycles, that is, if the value of the 
// inner cutoff is large enough it will corresond with a cosine
// value within a narrower range. E.g. if the inner cutoff is 
// 360, this is equivalent to being 0 again.

// Similarly, this occurs with negative angles as the cosine(negative value)
// forms a wave value that constantly repeats

// If the inner and outer cutoff are the same, then this is the equivalent
// of only having an inner cutoff, which results in a sharp edge to the light.
int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGl", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}


	Shader lightingShader("lightingShader.vs", "lightingShader.fs");
	Shader lightCubeShader("lightingCubeShader.vs", "lightingCubeShader.fs");

	Shader modelShader("modelShader.vs", "modelShader.fs");


	float verticies[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,	1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,	1.0f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,	1.0f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,	1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,	1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,	1.0f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};


	// Create virtual array object,
	// virtual buffer object,
	// element buffer object
	unsigned int VAO1 = createVAO(verticies, 36 * 8);
	unsigned int VAO2 = createVAO(verticies, 36 * 8);

	// Enable depth testing
	glEnable(GL_DEPTH_TEST);

	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(2.0f, 5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f, 3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f, 2.0f, -2.5f),
		glm::vec3(1.5f, 0.2f, -1.5f),
		glm::vec3(-1.3f, 1.0f, -1.5f)
	};


	unsigned int diffuseMap;
	glGenTextures(1, &diffuseMap);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height, nrChannels;

	unsigned char* data = stbi_load("container2.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else 
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);


	unsigned int specularMap;
	glGenTextures(1, &specularMap);
	glBindTexture(GL_TEXTURE_2D, specularMap);
	data = stbi_load("container2_specular.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);


	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};

	glm::vec3 pointLightColor[] = {
		glm::vec3(0.0f,  1.0f,  0.5f),
		glm::vec3(1.0f, 0.0f, 1.0f),
		glm::vec3(0.7f,  0.5f, 1.0f),
		glm::vec3(1.0f,  0.2f, 0.1f)
	};

	Model guitarModel = Model(string("backpack/backpack.obj"));

	while (!glfwWindowShouldClose(window))
	{

		// Get time
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//input
		processInput(window);

		// clear the screen
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);


		lightingShader.use();
		lightingShader.setFloat("material.shininess", 16.0f);
		lightingShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setVec3("dirLight.direction", 4.0f, -7.0f, 2.0f);

		lightingShader.setInt("material.diffuse", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);

		lightingShader.setInt("material.specular", 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);

		glm::vec3 lightColor;
		lightColor.x =  1.0;
		lightColor.y =  1.0;
		lightColor.z =  1.0;

		glm::vec3 diffuseColor = lightColor * glm::vec3(0.3f);
		glm::vec3 ambientColor = lightColor * glm::vec3(0.1f);

		lightingShader.setVec3("dirLight.ambient", ambientColor.x, ambientColor.y, ambientColor.z);
		lightingShader.setVec3("dirLight.diffuse", diffuseColor.x, diffuseColor.y, diffuseColor.z);

		glm::vec3 viewPos = camera.Position;
		lightingShader.setVec3("viewPos", viewPos.x, viewPos.y, viewPos.z);


		lightColor = pointLightColor[0];

		diffuseColor = lightColor * glm::vec3(0.5f);
		ambientColor = lightColor * glm::vec3(0.1f);

		// Set pointLight values
		lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setVec3("pointLights[0].ambient", ambientColor.x, ambientColor.y, ambientColor.z);
		lightingShader.setVec3("pointLights[0].diffuse", diffuseColor.x, diffuseColor.y, diffuseColor.z);
		lightingShader.setVec3("pointLights[0].position", pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		lightingShader.setFloat("pointLights[0].constant", 1.0f);
		lightingShader.setFloat("pointLights[0].linear", 0.001f);
		lightingShader.setFloat("pointLights[0].quadratic", 0.0001f);

		lightColor = pointLightColor[1];

		diffuseColor = lightColor * glm::vec3(0.5f);
		ambientColor = lightColor * glm::vec3(0.1f);


		lightingShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setVec3("pointLights[1].ambient", ambientColor.x, ambientColor.y, ambientColor.z);
		lightingShader.setVec3("pointLights[1].diffuse", diffuseColor.x, diffuseColor.y, diffuseColor.z);
		lightingShader.setVec3("pointLights[1].position", pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
		lightingShader.setFloat("pointLights[1].constant", 1.0f);
		lightingShader.setFloat("pointLights[1].linear", 0.01f);
		lightingShader.setFloat("pointLights[1].quadratic", 0.001f);

		lightColor = pointLightColor[2];

		diffuseColor = lightColor * glm::vec3(0.5f);
		ambientColor = lightColor * glm::vec3(0.1f);


		lightingShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setVec3("pointLights[2].ambient", ambientColor.x, ambientColor.y, ambientColor.z);
		lightingShader.setVec3("pointLights[2].diffuse", diffuseColor.x, diffuseColor.y, diffuseColor.z);
		lightingShader.setVec3("pointLights[2].position", pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
		lightingShader.setFloat("pointLights[2].constant", 1.0f);
		lightingShader.setFloat("pointLights[2].linear", 0.01f);
		lightingShader.setFloat("pointLights[2].quadratic", 0.001f);

		lightColor = pointLightColor[3];

		diffuseColor = lightColor * glm::vec3(0.5f);
		ambientColor = lightColor * glm::vec3(0.1f);

		lightingShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setVec3("pointLights[3].ambient", ambientColor.x, ambientColor.y, ambientColor.z);
		lightingShader.setVec3("pointLights[3].diffuse", diffuseColor.x, diffuseColor.y, diffuseColor.z);
		lightingShader.setVec3("pointLights[3].position", pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
		lightingShader.setFloat("pointLights[3].constant", 1.0f);
		lightingShader.setFloat("pointLights[3].linear", 0.01f);
		lightingShader.setFloat("pointLights[3].quadratic", 0.001f);

		// spot light
		lightColor = glm::vec3(1.0);

		diffuseColor = lightColor * glm::vec3(0.6f);
		ambientColor = lightColor * glm::vec3(0.1f);

		lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setVec3("spotLight.ambient", ambientColor.x, ambientColor.y, ambientColor.z);
		lightingShader.setVec3("spotLight.diffuse", diffuseColor.x, diffuseColor.y, diffuseColor.z);
		lightingShader.setVec3("spotLight.position", camera.Position.x, camera.Position.y, camera.Position.z);
		lightingShader.setVec3("spotLight.direction", camera.Front.x, camera.Front.y, camera.Front.z);
		lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(10.0f)));
		lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(20.0f)));


		glBindVertexArray(VAO1);

		// Set Projection
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(camera.Zoom), 800.0f/600.0f, 0.1f, 100.0f);

		unsigned int projectionLoc = glGetUniformLocation(lightingShader.ID, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// Set view
		glm::mat4 view = camera.GetViewMatrix();

		unsigned int viewLoc = glGetUniformLocation(lightingShader.ID, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));


		for (unsigned int i = 0; i < 10; i++)
		{
			// Sets models and draws boxes
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));


			unsigned int modelLoc = glGetUniformLocation(lightingShader.ID, "model");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}












		modelShader.use();
		modelShader.setFloat("material.shininess", 8.0f);
		modelShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);
		modelShader.setVec3("dirLight.direction", 4.0f, -7.0f, 2.0f);

		lightColor.x = 1.0;
		lightColor.y = 1.0;
		lightColor.z = 1.0;

		diffuseColor = lightColor * glm::vec3(0.3f);
		ambientColor = lightColor * glm::vec3(0.1f);

		modelShader.setVec3("dirLight.ambient", ambientColor.x, ambientColor.y, ambientColor.z);
		modelShader.setVec3("dirLight.diffuse", diffuseColor.x, diffuseColor.y, diffuseColor.z);

		viewPos = camera.Position;
		modelShader.setVec3("viewPos", viewPos.x, viewPos.y, viewPos.z);


		lightColor = pointLightColor[0];

		diffuseColor = lightColor * glm::vec3(0.0f);
		ambientColor = lightColor * glm::vec3(0.0f);

		// Set pointLight values
		modelShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
		modelShader.setVec3("pointLights[0].ambient", ambientColor.x, ambientColor.y, ambientColor.z);
		modelShader.setVec3("pointLights[0].diffuse", diffuseColor.x, diffuseColor.y, diffuseColor.z);
		modelShader.setVec3("pointLights[0].position", pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		modelShader.setFloat("pointLights[0].constant", 1.0f);
		modelShader.setFloat("pointLights[0].linear", 0.001f);
		modelShader.setFloat("pointLights[0].quadratic", 0.0001f);

		lightColor = pointLightColor[1];

		diffuseColor = lightColor * glm::vec3(0.5f);
		ambientColor = lightColor * glm::vec3(0.1f);


		modelShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
		modelShader.setVec3("pointLights[1].ambient", ambientColor.x, ambientColor.y, ambientColor.z);
		modelShader.setVec3("pointLights[1].diffuse", diffuseColor.x, diffuseColor.y, diffuseColor.z);
		modelShader.setVec3("pointLights[1].position", pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
		modelShader.setFloat("pointLights[1].constant", 1.0f);
		modelShader.setFloat("pointLights[1].linear", 0.01f);
		modelShader.setFloat("pointLights[1].quadratic", 0.001f);

		lightColor = pointLightColor[2];

		diffuseColor = lightColor * glm::vec3(0.5f);
		ambientColor = lightColor * glm::vec3(0.1f);


		modelShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
		modelShader.setVec3("pointLights[2].ambient", ambientColor.x, ambientColor.y, ambientColor.z);
		modelShader.setVec3("pointLights[2].diffuse", diffuseColor.x, diffuseColor.y, diffuseColor.z);
		modelShader.setVec3("pointLights[2].position", pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
		modelShader.setFloat("pointLights[2].constant", 1.0f);
		modelShader.setFloat("pointLights[2].linear", 0.01f);
		modelShader.setFloat("pointLights[2].quadratic", 0.001f);

		lightColor = pointLightColor[3];

		diffuseColor = lightColor * glm::vec3(0.5f);
		ambientColor = lightColor * glm::vec3(0.1f);

		modelShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
		modelShader.setVec3("pointLights[3].ambient", ambientColor.x, ambientColor.y, ambientColor.z);
		modelShader.setVec3("pointLights[3].diffuse", diffuseColor.x, diffuseColor.y, diffuseColor.z);
		modelShader.setVec3("pointLights[3].position", pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
		modelShader.setFloat("pointLights[3].constant", 1.0f);
		modelShader.setFloat("pointLights[3].linear", 0.01f);
		modelShader.setFloat("pointLights[3].quadratic", 0.001f);

		// spot light
		lightColor = glm::vec3(1.0);

		diffuseColor = lightColor * glm::vec3(0.6f);
		ambientColor = lightColor * glm::vec3(0.1f);

		modelShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		modelShader.setVec3("spotLight.ambient", ambientColor.x, ambientColor.y, ambientColor.z);
		modelShader.setVec3("spotLight.diffuse", diffuseColor.x, diffuseColor.y, diffuseColor.z);
		modelShader.setVec3("spotLight.position", camera.Position.x, camera.Position.y, camera.Position.z);
		modelShader.setVec3("spotLight.direction", camera.Front.x, camera.Front.y, camera.Front.z);
		modelShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(10.0f)));
		modelShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(20.0f)));

		projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(camera.Zoom), 800.0f / 600.0f, 0.1f, 100.0f);

		projectionLoc = glGetUniformLocation(modelShader.ID, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// Set view
		view = camera.GetViewMatrix();

		viewLoc = glGetUniformLocation(modelShader.ID, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));


		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(1.0,5.0, -10.0));

		unsigned int modelLoc = glGetUniformLocation(modelShader.ID, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		guitarModel.Draw(modelShader);















		glBindVertexArray(VAO2);
		lightCubeShader.use();

		view = camera.GetViewMatrix();
		viewLoc = glGetUniformLocation(lightCubeShader.ID, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(camera.Zoom), 800.0f / 600.0f, 0.1f, 100.0f);
		projectionLoc = glGetUniformLocation(lightCubeShader.ID, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));



		for (unsigned int i = 0; i < 4; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.2f));
			lightCubeShader.setVec3("objectColor", pointLightColor[i].x, pointLightColor[i].y, pointLightColor[i].z);

			unsigned int modelLoc = glGetUniformLocation(lightCubeShader.ID, "model");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

			glDrawArrays(GL_TRIANGLES, 0, 36);

		}



		//check and call events and swap buffers
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwTerminate();

	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}

unsigned int createVAO(float verticies[], int vertexSize)
{
	// Create virtual array object,
	// virtual buffer object,
	// element buffer object
	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// VAO stores all the other operations
	// done for VBO and EBO, so bind first
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(&verticies) * vertexSize, verticies, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3* sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	return VAO;
}

void mouse_callback(GLFWwindow* window, double xPos, double yPos)
{

	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}


	float xOffset = xPos - lastX;
	float yOffset = yPos - lastY;
	lastX = xPos;
	lastY = yPos;
	 
	camera.ProcessMouseMovement(xOffset, -yOffset, true);
}


void scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
{
	camera.ProcessMouseScroll(yOffset);
}
