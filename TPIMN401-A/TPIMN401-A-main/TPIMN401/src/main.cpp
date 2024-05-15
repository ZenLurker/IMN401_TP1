#include <glad/glad.h>
#include <glfw3.h>

#define _USE_MATH_DEFINES
#include "cmath" 

#include <iostream>
#include <vector>
#include <glm.hpp>
#include <gtc/constants.hpp>
//#include "time.h"
#include "utils.hpp"

namespace IMN401 {
	void printProgramError(GLuint program)
	{
		GLuint isLinked;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			std::cout << "Error : " + std::string(infoLog.begin(), infoLog.end()) + "\n";

			// We don't need the program anymore.
			glDeleteProgram(program);
			return;
		}
		else
			std::cout << "Shader compilation : OK" << std::endl;
	}

	void printPipelineError(GLuint pipeline)
	{
		GLuint isValid;
		glGetProgramPipelineiv(pipeline, GL_VALIDATE_STATUS, (int*)&isValid);
		if (isValid == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramPipelineiv(pipeline, GL_INFO_LOG_LENGTH, &maxLength);
			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramPipelineInfoLog(pipeline, maxLength, &maxLength, &infoLog[0]);

			std::cout << "Error : " + std::string(infoLog.begin(), infoLog.end()) + "\n";
			// We don't need the program anymore.
			glDeleteProgram(pipeline);
			return;
		}
		else
			std::cout << "Pipeline : OK" << std::endl;
	}

	//Force dGPU usage on my AMD hybrid setup
#ifdef _WIN32
#ifdef __cplusplus
	extern "C" {
#endif

		__declspec(dllexport) uint32_t NvOptimusEnablement = 1;
		__declspec(dllexport) int  AmdPowerXpressRequestHighPerformance = 1;

#ifdef __cplusplus
	}
#endif
#endif


	int main()
	{
		// Init GLFW
		glfwInit();

		if (!glfwInit()) {
			std::cerr << "Failed to initialize GLFW" << std::endl;
			return EXIT_FAILURE;
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		const int width = 800;
		const int height = 800;

		GLFWwindow* window = glfwCreateWindow(width, height, "TP - From Scratch", NULL, NULL);
		glfwMakeContextCurrent(window);
		glfwSetWindowUserPointer(window, NULL);


		// Load all OpenGL functions using the glfw loader function
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			std::cerr << "Failed to initialize OpenGL context" << std::endl;
			glfwTerminate();
			return EXIT_FAILURE;
		}

		// Initialization
		// =====================
		// TODO: init buffers, shaders, etc.
		// cf. https://www.khronos.org/files/opengl46-quick-reference-card.pdf

		// shaders
		std::string strVS = readFile("shaders/triangle-vs.glsl");
		const GLchar* vsCode = strVS.c_str();
		std::string strFS = readFile("shaders/triangle-fs.glsl");
		const GLchar* fsCode = strFS.c_str();

		// Create shader program
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vsCode, nullptr);
		glCompileShader(vertexShader);

		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fsCode, nullptr);
		glCompileShader(fragmentShader);

		GLuint shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		glLinkProgram(shaderProgram);

		// Check for shader program linking errors
		printProgramError(shaderProgram);

		glUseProgram(shaderProgram);

		// Cleanup shaders
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		glClearColor(0.3, 0.3, 0.3, 0);
		glEnable(GL_DEPTH_TEST);

		//---------------- TABLEAUX----------------
		// Vertices for the circle
		std::vector<glm::vec3> vertices;
		std::vector<GLuint> indices;

		const int n = 10; // Number of vertices on the circle
		const float radius = 0.5f; // Radius of the circle

		// Center vertex
		vertices.push_back(glm::vec3(0.0f, 0.0f, 0.0f));

		// Generate vertices on the circle
		for (int i = 0; i < n; ++i) {
			float angle = 2.0f * M_PI * i / n;
			float x = radius * std::cos(angle);
			float y = radius * std::sin(angle);
			vertices.push_back(glm::vec3{ x, y, 0.f });
		}

		// Generate indices for triangles (each triangle originates from the center)
		for (int i = 0; i < n; ++i) {
			indices.push_back(0); // Center vertex
			indices.push_back(i + 1); // Current vertex on the circle
			indices.push_back((i + 1) % n + 1); // Next vertex on the circle (wrapping around)
		}

		// Create and bind the VAO
		GLuint VAO;
		glCreateVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		// Create and bind the vertex buffer for vertices
		GLuint VBO_vertices;
		glCreateBuffers(1, &VBO_vertices);
		glNamedBufferData(VBO_vertices, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

		// Create and bind the vertex buffer for the indices
		GLuint EBO_indices;
		glCreateBuffers(1, &EBO_indices);
		glNamedBufferData(EBO_indices, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

		//Atributes binding
		glVertexArrayAttribBinding(VAO, 0, 0);
		//Definir le formatage des VBOs
		glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
		//Activer chaque attribut de nos données
		glEnableVertexArrayAttrib(VAO, 0);

		// Bind the vertex buffer to VAO
		glVertexArrayVertexBuffer(VAO, 0, VBO_vertices, 0, sizeof(glm::vec3));
		//Specifier le VBO a lire pour le tableau d'indices indices
		glVertexArrayElementBuffer(VAO, EBO_indices);


		// Specify the input attribute for the vertex position in the vertex shader
		GLuint vertexPositionLocation = glGetAttribLocation(shaderProgram, "vertexPosition");
		if (vertexPositionLocation == -1) {
			std::cerr << "Failed to get location for vertexPosition" << std::endl;
		}
		// Enable the attribute array
		glEnableVertexAttribArray(vertexPositionLocation);

		// Define the format of the VBO data
		glVertexArrayAttribFormat(VAO, vertexPositionLocation, 3, GL_FLOAT, GL_FALSE, 0);

		//Init clock and locate uniform time variable
		GLuint elapsedTimeMSLocation = glGetAttribLocation(shaderProgram, "elapsedTimeMS");
		if (vertexPositionLocation == -1) {
			std::cerr << "Failed to get location for elapsedTimeMS" << std::endl;
		}
		clock_t startTime;
		startTime = clock();

		if (glGetError() != GL_NO_ERROR) {
			std::cerr << "OpenGL error" << std::endl;
			return EXIT_FAILURE;
		}
		// ====================
		// 
		// Rendering loop
		while (!glfwWindowShouldClose(window))
		{
			// Handle events
			glfwPollEvents();

			// Calculate time elapsed since start
			GLfloat timeElapsedMS = (clock() - (GLfloat)startTime);
			glGetError();

			// Set uniform value for elapsedTimeMS
			glUniform1f(elapsedTimeMSLocation, 0);
			glGetError();

			// Clear the screen
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glGetError();

			// Draw elements
			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
			glGetError();

			// Swap buffers
			glfwSwapBuffers(window);
		}


		// Clean up
		glfwTerminate();

		return EXIT_SUCCESS;
	}
}

int main()
{
	return IMN401::main();
}