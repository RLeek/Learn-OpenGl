#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
unsigned int createShaderProgram(const char* fragmentShaderSource);
unsigned int createVAO(float verticies[], int vertexSize, unsigned int indices[], int indexSize);


// Does exercises 1, 2 and 3 together
// Ex3: 
// The reason that the bottom-left side of our triangle is black is because 
// the XYZ values are mapped to RGB, but the XYZ values are either 0, or 
// smaller than 0. Since RGB(0,0,0) is black, the vertex is also black.

// Note: Since we implemented the exercises together, the black vertex is 
// actually the bottom middle vertex, not the bottom-left vertex, as the
// triangle is inverted. Regardless, the same logic applies, the RGB values
// are (0,0,0).
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

	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}


	Shader customShader("shader.vs", "shader.fs");


	float verticies[] = {
		0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
	};

	unsigned int indices1[] = {
		0,1,2,
	};

	// Create virtual array object,
	// virtual buffer object,
	// element buffer object
	unsigned int VAO1 = createVAO(verticies, 24, indices1, 6);


	while (!glfwWindowShouldClose(window))
	{
		//input
		processInput(window);

		// clear the screen
		glClearColor(0.1f, 0.1f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Get the uniform value
		float timeValue = glfwGetTime();
		float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
		customShader.use();
		customShader.setFloat("offset", 0.5f);

		// draw the actual objects
		glBindVertexArray(VAO1);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}

unsigned int createVAO(float verticies[], int vertexSize, unsigned int indices[], int indexSize)
{
	// Create virtual array object,
	// virtual buffer object,
	// element buffer object
	unsigned int VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// VAO stores all the other operations
	// done for VBO and EBO, so bind first
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(&verticies) * vertexSize, verticies, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(&indices) * indexSize, indices, GL_STATIC_DRAW);


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	return VAO;
}