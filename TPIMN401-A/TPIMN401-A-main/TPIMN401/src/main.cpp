#include <glad/glad.h>
#include <glfw3.h>
#include <iostream>
#include <vector>
#include <glm.hpp>
#include <gtc/constants.hpp>
#include <chrono>
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

		// Cleanup shaders
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		// Get the location of the uniform variable in the fragment shader
		GLuint fragmentColorLocation = glGetUniformLocation(shaderProgram, "triangleColor");
		if (fragmentColorLocation == -1) {
			std::cerr << "Failed to get uniform location for triangleColor" << std::endl;
		}

		// Set the uniform color value
		glUseProgram(shaderProgram);
		GLfloat fragmentColor[] = { 1.0f, 0.0f, 0.0f }; // Red color
		glUniform3fv(fragmentColorLocation, 1, fragmentColor);

		glClearColor(0.3, 0.3, 0.3, 0);
		glEnable(GL_DEPTH_TEST);


		glm::vec3 vArray[]
		{ {-0.5f, -0.5f, 0.f},
			{0, 0.5f, 0.f},
			{0.5f, -0.5f, 0.f}
		};

		//Creation de VBO
		GLuint VBO;
		glCreateBuffers(1, &VBO);
		//Remplir le VBO
		glNamedBufferData(VBO,
			sizeof(GLfloat) * sizeof(vArray),
			&vArray,
			GL_STATIC_DRAW);

		//Creer VAO
		GLuint VA;
		glCreateVertexArrays(1, &VA);

		//Activer chaque attribut de nos données
		glEnableVertexArrayAttrib(VA, 0);
		glEnableVertexArrayAttrib(VA, 1); //removing this doesnt break anything??

		//Definir le formatage des VBOs
		glVertexArrayAttribFormat(VA, 0, 3, GL_FLOAT, GL_FALSE, 0);

		//Specifier le VBO a lire
		glVertexArrayVertexBuffer(VA, 0, VBO, 0, sizeof(float) * 3);

		//Atributes binding
		glVertexArrayAttribBinding(VA, 0, 0);
		glVertexArrayAttribBinding(VA, 1, 1);

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
			// ==================
			// TODO: render here !
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//glBindProgramPipeline(pipeline);
			glBindVertexArray(VA);

			glDrawArrays(GL_TRIANGLES, 0, 3);
			// ==================

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