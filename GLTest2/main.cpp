#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

std::string LoadFileToString(const char* filepath) {
	std::string out = "";
	std::ifstream stream(filepath, std::ios::in);

	if (stream.is_open()) {
		std::string line = "";

		while (getline(stream, line)) {
			out += "\n" + line;
		}

		stream.close();
	}

	return out;
}

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}

int main() {
	if (glfwInit() == false)
	{
		// did not succeed
		fprintf(stderr, "GLFW failed to initialize.");
		return -1;
	}

	// 4 AA
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window;
	window = glfwCreateWindow(640, 480, "Mike's C++ OpenGL Demo", NULL, NULL);

	if (!window)
	{
		fprintf(stderr, "Window failed to create");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glewExperimental = true;

	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "GLEW failed to initialize.");
		glfwTerminate();
		return -1;
	}

	// generate VAO
	GLuint vaoID;
	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);

	std::string manRaw = LoadFileToString("man.raw");
	std::vector<std::string> lines = split(manRaw, '\n');
	std::vector<GLfloat> vertices;
	for (auto &line : lines)
	{
		if (line.empty()) continue;
		std::vector<std::string> coords = split(line, ' ');
		for (auto &coord : coords)
		{
			if (coord.empty()) continue;			
			double d = atof(coord.c_str());
			vertices.push_back(d);
			fprintf(stdout, "%s = %f\n", coord.c_str(), d);
		}
		fprintf(stdout, "EOL\n");
	}

	// shaders
	GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	const char* vertShaderSource = R"(
        varying vec3 vertex_color;
		
		void main(){
		  gl_Position = ftransform();
          vertex_color = gl_Vertex.xyz;
		}
		)";
	const char* fragShaderSource = R"(
        varying vec3 vertex_color;
		
		void main(){
          gl_FragColor = vec4(vertex_color,1.0);
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
	GLfloat* verts = vertices.data();
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(verts), verts, GL_STATIC_DRAW);
	fprintf(stdout, "size %d %d", vertices.size(), sizeof(verts));


	int width, height;
	glfwGetWindowSize(window, &width, &height);
	glViewport(0, 0, width, height);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	double x = 1.0f;
	double y = 1.0f;
	double rx = 1.0f;
	double ry = 1.0f;

	do
	{
		x = x + 0.01f;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// projection matrix as describing the attributes of your camera, such as field of view, focal length, fish eye lens, etc. 
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(65.0, width / height, 1.0f, 64.0f);

		// ModelView matrix as where you stand with the camera and the direction you point it.
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glPushMatrix();
		//gluLookAt(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);

		glTranslated(x, y, 0.0f);
		//glRotated(180.0f*rx, 0.0f, 1.0f, 0.0f);
		//glRotated(180.0f*ry, 1.0f, 0.0f, 0.0f);

		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, vboID);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glUseProgram(program);

		glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 3);

		glDisableVertexAttribArray(0);
		glPopMatrix();

		glfwSwapBuffers(window);
		glfwPollEvents();

	} while (glfwWindowShouldClose(window) == false);

	return 0;
}