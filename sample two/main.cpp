/*
Student Information
Student ID:
Student Name:
*/

#include "Dependencies/glew/glew.h"
#include "Dependencies/GLFW/glfw3.h"
#include "Dependencies/glm/glm.hpp"
#include "Dependencies/glm/gtc/matrix_transform.hpp"

#include "Shader.h"
#include "Texture.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <map>



// screen setting
const int SCR_WIDTH = 800;
const int SCR_HEIGHT = 600;

// struct for storing the obj file
struct Vertex {
	glm::vec3 position;
	glm::vec2 uv;
	glm::vec3 normal;
};

struct Model {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
};

Model loadOBJ(const char* objPath)
{
	// function to load the obj file
	// Note: this simple function cannot load all obj files.

	struct V {
		// struct for identify if a vertex has showed up
		unsigned int index_position, index_uv, index_normal;
		bool operator == (const V& v) const {
			return index_position == v.index_position && index_uv == v.index_uv && index_normal == v.index_normal;
		}
		bool operator < (const V& v) const {
			return (index_position < v.index_position) ||
				(index_position == v.index_position && index_uv < v.index_uv) ||
				(index_position == v.index_position && index_uv == v.index_uv && index_normal < v.index_normal);
		}
	};

	std::vector<glm::vec3> temp_positions;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;

	std::map<V, unsigned int> temp_vertices;

	Model model;
	unsigned int num_vertices = 0;

	std::cout << "\nLoading OBJ file " << objPath << "..." << std::endl;

	std::ifstream file;
	file.open(objPath);

	// Check for Error
	if (file.fail()) {
		std::cerr << "Impossible to open the file! Do you use the right path? See Tutorial 6 for details" << std::endl;
		exit(1);
	}

	while (!file.eof()) {
		// process the object file
		char lineHeader[128];
		file >> lineHeader;

		if (strcmp(lineHeader, "v") == 0) {
			// geometric vertices
			glm::vec3 position;
			file >> position.x >> position.y >> position.z;
			temp_positions.push_back(position);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			// texture coordinates
			glm::vec2 uv;
			file >> uv.x >> uv.y;
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			// vertex normals
			glm::vec3 normal;
			file >> normal.x >> normal.y >> normal.z;
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			// Face elements
			V vertices[3];
			for (int i = 0; i < 3; i++) {
				char ch;
				file >> vertices[i].index_position >> ch >> vertices[i].index_uv >> ch >> vertices[i].index_normal;
			}

			// Check if there are more than three vertices in one face.
			std::string redundency;
			std::getline(file, redundency);
			if (redundency.length() >= 5) {
				std::cerr << "There may exist some errors while load the obj file. Error content: [" << redundency << " ]" << std::endl;
				std::cerr << "Please note that we only support the faces drawing with triangles. There are more than three vertices in one face." << std::endl;
				std::cerr << "Your obj file can't be read properly by our simple parser :-( Try exporting with other options." << std::endl;
				exit(1);
			}

			for (int i = 0; i < 3; i++) {
				if (temp_vertices.find(vertices[i]) == temp_vertices.end()) {
					// the vertex never shows before
					Vertex vertex;
					vertex.position = temp_positions[vertices[i].index_position - 1];
					vertex.uv = temp_uvs[vertices[i].index_uv - 1];
					vertex.normal = temp_normals[vertices[i].index_normal - 1];

					model.vertices.push_back(vertex);
					model.indices.push_back(num_vertices);
					temp_vertices[vertices[i]] = num_vertices;
					num_vertices += 1;
				}
				else {
					// reuse the existing vertex
					unsigned int index = temp_vertices[vertices[i]];
					model.indices.push_back(index);
				}
			} // for
		} // else if
		else {
			// it's not a vertex, texture coordinate, normal or face
			char stupidBuffer[1024];
			file.getline(stupidBuffer, 1024);
		}
	}
	file.close();

	std::cout << "There are " << num_vertices << " vertices in the obj file.\n" << std::endl;
	return model;
}

void get_OpenGL_info()
{
	// OpenGL information
	const GLubyte* name = glGetString(GL_VENDOR);
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* glversion = glGetString(GL_VERSION);
	std::cout << "OpenGL company: " << name << std::endl;
	std::cout << "Renderer name: " << renderer << std::endl;
	std::cout << "OpenGL version: " << glversion << std::endl;
}

Model Dol_obj;
GLuint Dol_vao, Dol_vbo, Dol_ebo;
Texture Dol_texture[2];
Model Sea_obj;
GLuint Sea_vao, Sea_vbo, Sea_ebo;
Texture Sea_texture[2];
Model Penguin_obj;
GLuint Penguin_vao, Penguin_vbo, Penguin_ebo;
Texture Penguin_texture[2];
Shader shader;

int DolTexture = 1;
int SeaTexture = 1;
glm::vec3 DolMoveDirection;
float DolRotateDirection;

void Dol_bind()
{
	Dol_texture[0].setupTexture("resources/dolphin/dolphin_01.jpg");
	Dol_texture[1].setupTexture("resources/dolphin/dolphin_02.jpg");
	Dol_obj = loadOBJ("resources/dolphin/dolphin.obj");

	glGenVertexArrays(1, &Dol_vao);
	glBindVertexArray(Dol_vao);

	glGenBuffers(1, &Dol_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, Dol_vbo);
	glBufferData(GL_ARRAY_BUFFER, Dol_obj.vertices.size() * sizeof(Vertex), &Dol_obj.vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &Dol_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Dol_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Dol_obj.indices.size() * sizeof(unsigned int), &Dol_obj.indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
}

void Sea_bind()
{
	Sea_texture[0].setupTexture("resources/sea/sea_01.jpg");
	Sea_texture[1].setupTexture("resources/sea/sea_02.jpg");
	Sea_obj = loadOBJ("resources/sea/sea.obj");

	glGenVertexArrays(1, &Sea_vao);
	glBindVertexArray(Sea_vao);

	glGenBuffers(1, &Sea_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, Sea_vbo);
	glBufferData(GL_ARRAY_BUFFER, Sea_obj.vertices.size() * sizeof(Vertex), &Sea_obj.vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &Sea_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Sea_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Sea_obj.indices.size() * sizeof(unsigned int), &Sea_obj.indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
}

void Penguin_bind()
{
	Sea_texture[0].setupTexture("resources/Penguin/Penguin.jpg");
	Sea_obj = loadOBJ("resources/Penguin/Penguin.obj");

	glGenVertexArrays(1, &Penguin_vao);
	glBindVertexArray(Penguin_vao);

	glGenBuffers(1, &Penguin_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, Penguin_vbo);
	glBufferData(GL_ARRAY_BUFFER, Penguin_obj.vertices.size() * sizeof(Vertex), &Penguin_obj.vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &Sea_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Penguin_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Penguin_obj.indices.size() * sizeof(unsigned int), &Penguin_obj.indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
}

void sendDataToOpenGL()
{
	//TODO
	//Load objects and bind to VAO and VBO
	Dol_bind();
	Sea_bind();
	//Penguin_bind();
	//Load textures
}

void initializedGL(void) //run only once
{
	if (glewInit() != GLEW_OK) {
		std::cout << "GLEW not OK." << std::endl;
	}

	get_OpenGL_info();
	sendDataToOpenGL();


	//TODO: set up the camera parameters	

	//TODO: set up the vertex shader and fragment shader
	shader.setupShader("VertexShaderCode.glsl", "FragmentShaderCode.glsl");
	shader.use();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void paintDol()
{
	glm::mat4 modelTransformMatrix;
	glm::mat4 modelTransformMatrix_rotate1;
	glm::mat4 modelTransformMatrix_rotate2;
	glm::mat4 modelTransformMatrix_rotate3;
	glm::mat4 modelTransformMatrix_scale;
	glm::mat4 modelTransformMatrix_translate1;

	glm::mat4 modelTransformMatrix_translate;
	glm::mat4 modelTransformMatrix_rotate;

	GLint modelTransformMatrixUniformLocation;
	GLuint TextureID;

	TextureID = glGetUniformLocation(shader.Return_ID(), "myTextureSampler");
	glActiveTexture(GL_TEXTURE0);
	if (DolTexture == 1) {
		Dol_texture[0].bind(0);
	}
	else {
		Dol_texture[1].bind(0);
	}
	glUniform1i(TextureID, 0);

	glBindVertexArray(Dol_vao);
	modelTransformMatrix_translate1 = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0.0f, 0.0f));
	modelTransformMatrix_rotate1 = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1, 0, 0));
	modelTransformMatrix_rotate2 = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0, 1, 0));
	modelTransformMatrix_rotate3 = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 0, 1));
	modelTransformMatrix_scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.05f, 0.05f, 0.05f));
	modelTransformMatrix = modelTransformMatrix_translate1 * modelTransformMatrix_rotate1 * modelTransformMatrix_rotate2 * modelTransformMatrix_rotate3 * modelTransformMatrix_scale;

	modelTransformMatrix = glm::translate(modelTransformMatrix, DolMoveDirection);
	modelTransformMatrix = glm::rotate(modelTransformMatrix, glm::radians(DolRotateDirection), glm::vec3(0.0f, 0.0f, 1.0f));

	modelTransformMatrixUniformLocation = glGetUniformLocation(shader.Return_ID(), "modelTransformMatrix");

	glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, Dol_obj.indices.size(), GL_UNSIGNED_INT, 0);
}

void paintSea()
{

	GLint modelTransformMatrixUniformLocation;
	glm::mat4 modelTransformMatrix;
	GLuint TextureID;
	TextureID = glGetUniformLocation(shader.Return_ID(), "myTextureSampler");
	glActiveTexture(GL_TEXTURE0);
	if (SeaTexture == 1) {
		Sea_texture[0].bind(0);
	}
	else {
		Sea_texture[1].bind(0);
	}
	glUniform1i(TextureID, 0);

	glBindVertexArray(Sea_vao);
	modelTransformMatrix = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	modelTransformMatrix = glm::scale(modelTransformMatrix, glm::vec3(5.0f, 1.0f, 5.0f));
	modelTransformMatrixUniformLocation = glGetUniformLocation(shader.Return_ID(), "modelTransformMatrix");
	glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, Sea_obj.indices.size(), GL_UNSIGNED_INT, 0);

}

void paintPen()
{
	glm::mat4 modelTransformMatrix;
	glm::mat4 modelTransformMatrix_rotate1;
	glm::mat4 modelTransformMatrix_rotate2;
	glm::mat4 modelTransformMatrix_rotate3;
	glm::mat4 modelTransformMatrix_scale;
	glm::mat4 modelTransformMatrix_translate1;

	glm::mat4 modelTransformMatrix_translate;
	glm::mat4 modelTransformMatrix_rotate;

	GLint modelTransformMatrixUniformLocation;
	GLuint TextureID;

	TextureID = glGetUniformLocation(shader.Return_ID(), "myTextureSampler");
	glActiveTexture(GL_TEXTURE0);
	Penguin_texture[0].bind(0);
	glUniform1i(TextureID, 0);

	glBindVertexArray(Penguin_vao);
	modelTransformMatrix_translate1 = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0.0f, 0.0f));
	modelTransformMatrix_rotate1 = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1, 0, 0));
	modelTransformMatrix_rotate2 = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0, 1, 0));
	modelTransformMatrix_rotate3 = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 0, 1));
	modelTransformMatrix_scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.05f, 0.05f, 0.05f));
	modelTransformMatrix = modelTransformMatrix_translate1 * modelTransformMatrix_rotate1 * modelTransformMatrix_rotate2 * modelTransformMatrix_rotate3 * modelTransformMatrix_scale;

	modelTransformMatrix = glm::translate(modelTransformMatrix, DolMoveDirection);
	modelTransformMatrix = glm::rotate(modelTransformMatrix, glm::radians(DolRotateDirection), glm::vec3(0.0f, 0.0f, 1.0f));

	modelTransformMatrixUniformLocation = glGetUniformLocation(shader.Return_ID(), "modelTransformMatrix");

	glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, Dol_obj.indices.size(), GL_UNSIGNED_INT, 0);
}

glm::vec3 cameraPos;
glm::vec3 cameraFocus(0.0f);
float cameraMoveX;
float cameraMoveY;
float zoom = 45.0f;
float diffuseBrightness = 0.5f;
double xpos, ypos;
double mouseDeltaX, mouseDeltaY;
float Yradian;

void generateMVPMatrix(void)
{
	GLint projectionMatrixUniformLocation;
	GLint cameraMatrixUniformLocation;
	glm::mat4 cameraMatrix;
	glm::mat4 projectionMatrix;

	cameraPos = glm::vec3(0.0f, 10.0f, -10.0f);
	cameraMatrix = glm::lookAt(cameraPos, cameraFocus, glm::vec3(0.0f, 0.0f, 1.0f));
	cameraMatrix = glm::translate(cameraMatrix, glm::vec3(0.0f, cameraMoveY, -cameraMoveX));
	cameraMatrix = glm::rotate(cameraMatrix, glm::radians(Yradian), glm::vec3(0, 1, 0));

	cameraMatrixUniformLocation = glGetUniformLocation(shader.Return_ID(), "cameraMatrix");
	glUniformMatrix4fv(cameraMatrixUniformLocation, 1, GL_FALSE, &cameraMatrix[0][0]);

	projectionMatrix = glm::perspective(glm::radians(45.0f + zoom), 1.3f, 0.5f, 100.0f);
	projectionMatrixUniformLocation = glGetUniformLocation(shader.Return_ID(), "projectionMatrix");
	glUniformMatrix4fv(projectionMatrixUniformLocation, 1, GL_FALSE, &projectionMatrix[0][0]);


}

bool runningLight = true;
float c_time;
float timecolor;
bool increaseState = true;
void ambientLight()
{
	GLint ambientLightingUniformLocation = glGetUniformLocation(shader.Return_ID(), "ambientLight");
	float ambientLight = 0.5f;
	glUniform1f(ambientLightingUniformLocation, ambientLight);
}
void diffuseLight()
{
	float delta_time = 0.1f;
	if (runningLight) {
		if (increaseState == true)
			c_time += delta_time;
		else
			c_time -= delta_time;
		timecolor = c_time * 0.1f;
		if (c_time * 0.1f > 4.0f)
			increaseState = false;
		if (c_time * 0.1f < 0.0f)
			increaseState = true;
	}
	float speed = 0.001f;
	glm::vec3 lightPosition1, lightPosition2, lightPosition3;
	GLint directionalLightBrightness = glGetUniformLocation(shader.Return_ID(), "directionalLightBrightness");
	glUniform1f(directionalLightBrightness, diffuseBrightness);

	glm::vec3 color1(1.0f + timecolor, 1.0f, 1.0f);
	GLint lightColor1 = glGetUniformLocation(shader.Return_ID(), "diffuseLightColor1");
	glUniform3fv(lightColor1, 1, &color1[0]);

	GLint lightPositionUniformLocation1 = glGetUniformLocation(shader.Return_ID(), "lightPositionWorld1");
	lightPosition1 = glm::vec3(20.0f * sin(c_time * speed), 20.0f, 20.0f * cos(c_time * speed));
	glUniform3fv(lightPositionUniformLocation1, 1, &lightPosition1[0]);

	glm::vec3 color2(1.0f, 1.0f + timecolor, 1.0f);
	GLint lightColor2 = glGetUniformLocation(shader.Return_ID(), "diffuseLightColor2");
	glUniform3fv(lightColor2, 1, &color2[0]);

	GLint lightPositionUniformLocation2 = glGetUniformLocation(shader.Return_ID(), "lightPositionWorld2");
	lightPosition2 = glm::vec3(20.0f * sin(c_time * speed + glm::radians(120.0f)), 20.0f, 20.0f * cos(c_time * speed + glm::radians(120.0f)));
	glUniform3fv(lightPositionUniformLocation2, 1, &lightPosition2[0]);

	glm::vec3 color3(1.0f, 1.0f, 1.0f + timecolor);
	GLint lightColor3 = glGetUniformLocation(shader.Return_ID(), "diffuseLightColor3");
	glUniform3fv(lightColor3, 1, &color3[0]);

	GLint lightPositionUniformLocation3 = glGetUniformLocation(shader.Return_ID(), "lightPositionWorld3");
	lightPosition3 = glm::vec3(20.0f * sin(c_time * speed + glm::radians(240.0f)), 20.0f, 20.0f * cos(c_time * speed + glm::radians(240.0f)));
	glUniform3fv(lightPositionUniformLocation3, 1, &lightPosition3[0]);
}
void specularLight()
{
	GLint eyePositionUniformLocation = glGetUniformLocation(shader.Return_ID(), "eyePositionWorld");
	glUniform3fv(eyePositionUniformLocation, 1, &cameraPos[0]);
}
void paintGL(void)  //always run
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //specify the background color, this is just an example
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	generateMVPMatrix();

	glLineWidth(2.0f);
	ambientLight();
	diffuseLight();
	specularLight();
	paintDol();
	paintSea();
	//paintPen();
	//TODO:
	//Set lighting information, such as position and color of lighting source
	//Set transformation matrix
	//Bind different textures

	glFlush();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

float move = 3.0f;
bool Drag = false;

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	// Sets the mouse-button callback for the current window.
	glfwGetCursorPos(window, &xpos, &ypos);
	if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS)
	{
		Drag = true;
	}
	if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE)
	{
		Drag = false;
	}
}

void cursor_position_callback(GLFWwindow* window, double x, double y)
{
	// Sets the cursor position callback for the current window
	if (Drag == true)
	{
		glfwGetCursorPos(window, &x, &y);
		mouseDeltaX = (x - xpos) * 0.1f;

		mouseDeltaY = (y - ypos) * 0.1f;
		xpos = x;
		ypos = y;
		Yradian += mouseDeltaX;
		cameraMoveY += mouseDeltaY;
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	// Sets the scoll callback for the current window.
	if (yoffset > 0)
		cameraMoveX -= 0.3f;
	else
		cameraMoveX += 0.3f;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Sets the Keyboard callback for the current window.
	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		DolTexture = 1;
	}
	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
	{
		DolTexture = 2;
	}
	if (key == GLFW_KEY_3 && action == GLFW_PRESS)
	{
		SeaTexture = 1;
	}
	if (key == GLFW_KEY_4 && action == GLFW_PRESS)
	{
		SeaTexture = 2;
	}
	if (key == GLFW_KEY_W && action == GLFW_PRESS)
	{
		diffuseBrightness = diffuseBrightness + 0.1f;
		if (diffuseBrightness > 1)
			diffuseBrightness = 1;
	}
	if (key == GLFW_KEY_S && action == GLFW_PRESS)
	{
		diffuseBrightness = diffuseBrightness - 0.1f;
		if (diffuseBrightness < 0)
			diffuseBrightness = 0;
	}

	if (key == GLFW_KEY_Q && action == GLFW_PRESS)
	{
		diffuseBrightness = diffuseBrightness + 0.1f;
		if (diffuseBrightness > 1)
			diffuseBrightness = 1;
	}
	if (key == GLFW_KEY_E && action == GLFW_PRESS)
	{
		diffuseBrightness = diffuseBrightness + 0.1f;
		if (diffuseBrightness > 1)
			diffuseBrightness = 1;
	}

	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
	{
		DolMoveDirection = DolMoveDirection + glm::vec3(move * cos(glm::radians(DolRotateDirection - 90.0f)), move * sin(glm::radians(DolRotateDirection - 90.0f)), 0.0f);
	}
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
	{
		DolMoveDirection = DolMoveDirection - glm::vec3(move * cos(glm::radians(DolRotateDirection - 90.0f)), move * sin(glm::radians(DolRotateDirection - 90.0f)), 0.0f);
	}
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
	{
		DolRotateDirection = DolRotateDirection + 2.0f;
	}
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
	{
		DolRotateDirection = DolRotateDirection - 2.0f;
	}
}


int main(int argc, char* argv[])
{
	GLFWwindow* window;

	/* Initialize the glfw */
	if (!glfwInit()) {
		std::cout << "Failed to initialize GLFW" << std::endl;
		return -1;
	}
	/* glfw: configure; necessary for MAC */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Assignment 2", NULL, NULL);
	if (!window) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	/*register callback functions*/
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);                                                                  //    
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	initializedGL();

	while (!glfwWindowShouldClose(window)) {
		/* Render here */
		paintGL();

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}






