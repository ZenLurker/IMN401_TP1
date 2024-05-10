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


		// shaders
		std::string strVS = readFile("shaders/triangle-vs.glsl");
		const GLchar* vsCode = strVS.c_str();
		std::string strFS = readFile("shaders/triangle-fs.glsl");
		const GLchar* fsCode = strFS.c_str();



		// Initialization
		// =====================
		// TODO: init buffers, shaders, etc.
		// cf. https://www.khronos.org/files/opengl46-quick-reference-card.pdf

		glClearColor(0.3, 0.3, 0.3, 0);

		glEnable(GL_DEPTH_TEST);

		GLfloat vArray[]
		{ -0.5f, -0.5f, 0.f,
			0, 0.5f, 0.f,
			0.5f, -0.5f, 0.f
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

		//Activer chaque attribut de nos donn�es
		glEnableVertexArrayAttrib(VA, 0);
		//Definir le formatage des VBOs
		glVertexArrayAttribFormat(VA, 0, 3, GL_FLOAT, GL_FALSE, 0);

		//Specifier le VBO a lire
		glVertexArrayVertexBuffer(VA, 0, VBO, 0, sizeof(float) * 3);

		glVertexArrayAttribBinding(VA, 0, 0);

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
			glClear(GL_COLOR_BUFFER_BIT);
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