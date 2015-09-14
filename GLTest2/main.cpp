#include <stdio.h>

#include <stdlib.h>
//#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

int main() {
	if (glfwInit() == false){
		// did not succeed
		fprintf(stderr, "GLFW failed to initialise.");
		return -1;
	}

	// 4 AA
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);




	GLFWwindow* window;
	window = glfwCreateWindow(640, 480, "Mike's C++ OpenGL Demo", NULL, NULL);

	if (!window) {
		fprintf(stderr, "Window failed to create");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glewExperimental = true;

	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "GLEW failed to initialize.");
		glfwTerminate();
		return -1;
	}

	// generate VAO
	GLuint vaoID;
	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);

	static const GLfloat verts[] = {
		// X, Y, Z
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f, 0.5f, 0.0f
	};

	// shaders
	GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	const char* vertShaderSource = R"(
		#version 330 core
		layout(location = 0) in vec3 in_pos;
		
		void main(){
		  gl_Position.xyz = in_pos;
		  gl_Position.w = 1;
		}
		)";
	const char* fragShaderSource = R"(
		#version 330 core
		out vec3 color;
		
		void main(){
		  color = vec3(1,0,0);
		}
		)";
	
	glShaderSource(vertShader, 1, &vertShaderSource, NULL);
	glShaderSource(fragShader, 1, &fragShaderSource, NULL);
	glCompileShader(vertShader);
	glCompileShader(fragShader);
	GLuint program = glCreateProgram();
	glAttachShader(program, vertShader);
	glAttachShader(program, fragShader);
	glLinkProgram(program);

	// generate vbo
	GLuint vboID;
	glGenBuffers(1, &vboID);
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	do {
		glClear(GL_COLOR_BUFFER_BIT);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, vboID);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glUseProgram(program);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glDisableVertexAttribArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();

	} while (glfwWindowShouldClose(window) == false);

	return 0;
}