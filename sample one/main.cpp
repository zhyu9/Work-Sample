///////////////////////////////////////////
////Type your name and student ID here/////
////Name: Mega Gunawan
////Student ID: 1155072307

#include "Dependencies\glew\glew.h"
#include "Dependencies\glm\glm.hpp"
#include "Dependencies\glm\gtc\matrix_transform.hpp"
#include <iostream>
#include <fstream>
using namespace std;
using glm::vec3;
using glm::mat4;

GLint programID;
float x_delta = 0.1f;
float y_delta = 0.1f;
int x_press_num = 0;
int y_press_num = 0;
float rotate_cube = 0.0f;
float rotate_pyramid = 0.0f;
int scale = 1;
int snow_position = 0;
float snow_delta = 0.1f;

GLuint width = 500;
GLuint height = 500;

GLuint vaoGround;
GLuint vboGround;
GLuint vboGroundColour;
GLuint vaoCube;
GLuint vboCube;
GLuint vboCubeColour;
GLuint vaoPyramid;
GLuint vboPyramid;
GLuint vboPyramidColour;
GLuint vaoSnowPoints;
GLuint vboSnowPoints;
GLuint vboSnowPointsColour;
GLuint vaoGroundLines;
GLuint vboGroundLines;
GLuint vboGroundLinesColour;

bool checkStatus(
	GLuint objectID,
	PFNGLGETSHADERIVPROC objectPropertyGetterFunc,
	PFNGLGETSHADERINFOLOGPROC getInfoLogFunc,
	GLenum statusType)
{
	GLint status;
	objectPropertyGetterFunc(objectID, statusType, &status);
	if (status != GL_TRUE)
	{
		GLint infoLogLength;
		objectPropertyGetterFunc(objectID, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* buffer = new GLchar[infoLogLength];

		GLsizei bufferSize;
		getInfoLogFunc(objectID, infoLogLength, &bufferSize, buffer);
		cout << buffer << endl;

		delete[] buffer;
		return false;
	}
	return true;
}

bool checkShaderStatus(GLuint shaderID)
{
	return checkStatus(shaderID, glGetShaderiv, glGetShaderInfoLog, GL_COMPILE_STATUS);
}

bool checkProgramStatus(GLuint programID)
{
	return checkStatus(programID, glGetProgramiv, glGetProgramInfoLog, GL_LINK_STATUS);
}

string readShaderCode(const char* fileName)
{
	ifstream meInput(fileName);
	if (!meInput.good())
	{
		cout << "File failed to load..." << fileName;
		exit(1);
	}
	return std::string(
		std::istreambuf_iterator<char>(meInput),
		std::istreambuf_iterator<char>()
	);
}

void installShaders()
{
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar* adapter[1];
	//adapter[0] = vertexShaderCode;
	string temp = readShaderCode("VertexShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(vertexShaderID, 1, adapter, 0);
	//adapter[0] = fragmentShaderCode;
	temp = readShaderCode("FragmentShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(fragmentShaderID, 1, adapter, 0);

	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);

	if (!checkShaderStatus(vertexShaderID) || !checkShaderStatus(fragmentShaderID))
		return;

	programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	if (!checkProgramStatus(programID))
		return;

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	glUseProgram(programID);
}

void keyboard(unsigned char key, int x, int y)
{
	//TODO:
	if (key == 'a' && x_press_num > -30)
	{
		x_press_num -= 1;
	}
	if (key == 'd' && x_press_num < 30)
	{
		x_press_num += 1;
	}
	if (key == 'w' && y_press_num < 30)
	{
		y_press_num += 1;

	}
	if (key == 's' && y_press_num > -30)
	{
		y_press_num -= 1;
	}
	if (key == 'r')
	{
		rotate_cube += 0.5f;
	}
	if (key == 'e')
	{
		rotate_pyramid -= 0.5f;
	}
	if (key == 'x')
	{
		scale += 1;
	}
	if (key == 'z' && scale > 1)
	{
		scale -= 1;
	}
	if (key == 27)
	{
		glutDestroyWindow(programID);
		exit(0);
	}
}

void mouse(int button, int state, int x, int y)
{
	if (button == 4) 
	{
		snow_position -= 1;
	}
}

void sendDataToOpenGL()
{
	//TODO:
	//create point, line, 2D object and 3D object here and bind to VAOs & VBOs
	const GLfloat cube[] =
	{
		-1.0f,-1.0f,-1.0f, //red
		-1.0f,-1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f, 
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,

		1.0f, 1.0f,-1.0f, //green
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f, 
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,

		1.0f,-1.0f, 1.0f, //blue
		-1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,

		1.0f, 1.0f, 1.0f, //yellow
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,

		-1.0f, 1.0f, 1.0f, //purple
		-1.0f,-1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,

		1.0f, 1.0f, 1.0f, //cyan
		1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
	};

	const GLfloat cubeColour[] =
	{
		1.0f, 0.0f, 0.0f, //red
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, //green
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,

		0.0f, 0.0f, 1.0f, //blue
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		1.0f, 1.0f, 0.0f, //yellow
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,

		1.0f, 0.0f, 1.0f, //purple
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,

		0.0f, 1.0f, 1.0f, //cyan
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f
	};

	const GLfloat ground[] =
	{
		-4.0f, -4.0f, -1.0f,
		4.0f, -4.0f, -1.0f,
		-4.0f, 4.0f, -1.0f,
		4.0f, 4.0f, -1.0f,
	};

	GLushort groundIndices[] =
	{
		0, 1, 2,
		1, 2, 3,
	};

	const GLfloat groundColour[] =
	{
		0.5f, 0.35f, 0.05f,
		0.5f, 0.35f, 0.05f,
		0.5f, 0.35f, 0.05f,
		0.5f, 0.35f, 0.05f,
	};

	const GLfloat pyramid[] =
	{
		/* -2.0f, -2.0f, 1.0f,
		-1.0f, -2.0f, 1.0f,
		-2.0f, -1.0f, 1.0f,

		-1.0f, -1.0f, 1.0f,
		-1.0f, -2.0f, 1.0f,
		-2.0f, -1.0f, 1.0f,

		-2.0f, -2.0f, 1.0f,
		-2.0f, -1.0f, 1.0f,
		-1.5f, -1.5f, 2.0f,

		-2.0f, -2.0f, 1.0f,
		-1.0f, -2.0f, 1.0f,
		-1.5f, -1.5f, 2.0f,

		-1.0f, -1.0f, 1.0f,
		-1.0f, -2.0f, 1.0f,
		-1.5f, -1.5f, 2.0f,

		-1.0f, -1.0f, 1.0f,
		-2.0f, -1.0f, 1.0f,
		-1.5f, -1.5f, 2.0f, */

		-1.5f, -1.5f, 1.0f,
		1.5f, -1.5f, 1.0f,
		-1.5f, 1.5f, 1.0f,

		1.5f, 1.5f, 1.0f,
		1.5f, -1.5f, 1.0f,
		-1.5f, 1.5f, 1.0f,

		-1.5f, -1.5f, 1.0f,
		-1.5f, 1.5f, 1.0f,
		0.0f, 0.0f, 2.0f,

		-1.5f, -1.5f, 1.0f,
		1.5f, -1.5f, 1.0f,
		0.0f, 0.0f, 2.0f,

		1.5f, 1.5f, 1.0f,
		1.5f, -1.5f, 1.0f,
		0.0f, 0.0f, 2.0f,

		1.5f, 1.5f, 1.0f,
		-1.5f, 1.5f, 1.0f,
		0.0f, 0.0f, 2.0f,

	};

	const GLfloat pyramidColour[] =
	{
		1.0f, 0.5f, 0.5f,
		1.0f, 0.5f, 0.5f,
		1.0f, 0.5f, 0.5f,
		1.0f, 0.5f, 0.5f,
		1.0f, 0.5f, 0.5f,
		1.0f, 0.5f, 0.5f,

		1.0f, 0.5f, 0.0f,
		1.0f, 0.5f, 0.0f,
		1.0f, 0.5f, 0.0f,

		0.5f, 1.0f, 0.0f,
		0.5f, 1.0f, 0.0f,
		0.5f, 1.0f, 0.0f,

		0.5f, 1.0f, 0.5f,
		0.5f, 1.0f, 0.5f,
		0.5f, 1.0f, 0.5f,

		0.0f, 0.5f, 0.0f,
		0.0f, 0.5f, 0.0f,
		0.0f, 0.5f, 0.0f,

	};

	const GLfloat snowPoints[] =
	{
		-3.5f, -3.5f, 3.5f,
		-3.0f, -3.0f, 0.0f,
		-2.5f, -2.5f, 3.0f,
		-2.0f, -2.0f, 2.0f,
		-1.5f, -1.5f, 2.5f,
		-1.0f, -1.0f, 1.0f,
		-0.5f, -0.5f, 1.5f,
		0.0f, 0.0f, 0.5f,
		0.5f, 0.5f, 2.5f,
		1.0f, 1.0f, 1.5f,
		1.5f, 1.5f, 3.0f,
		2.0f, 2.0f, 1.0f,
		2.5f, 2.5f, 2.0f,
		3.0f, 3.0f, 3.0f,
		3.5f, 3.5f, 3.5f,
	};

	const GLfloat snowPointsColour[] =
	{
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
	};

	const GLfloat groundLines[] =
	{
		-4.0f, -3.0f, -1.0f,
		4.0f, -3.0f, -1.0f,

		-4.0f, -1.0f, -1.0f,
		4.0f, -1.0f, -1.0f,

		-4.0f, 1.0f, -1.0f,
		4.0f, 1.0f, -1.0f,

		-4.0f, 3.0f, -1.0f,
		4.0f, 3.0f, -1.0f,

		3.0f, -4.0f, -1.0f,
		3.0f, 4.0f, -1.0f,

		1.0f, -4.0f, -1.0f,
		1.0f, 4.0f, -1.0f,

		-1.0f, -4.0f, -1.0f,
		-1.0f, 4.0f, -1.0f,

		-3.0f, -4.0f, -1.0f,
		-3.0f, 4.0f, -1.0f,
		
	};

	const GLfloat groundLinesColour[] =
	{
		0.5f, 0.5f, 0.05f,
		0.5f, 0.5f, 0.05f,
		0.5f, 0.5f, 0.05f,
		0.5f, 0.5f, 0.05f,
		0.5f, 0.5f, 0.05f,
		0.5f, 0.5f, 0.05f,
		0.5f, 0.5f, 0.05f,
		0.5f, 0.5f, 0.05f,
		0.5f, 0.5f, 0.05f,
		0.5f, 0.5f, 0.05f,
		0.5f, 0.5f, 0.05f,
		0.5f, 0.5f, 0.05f,
		0.5f, 0.5f, 0.05f,
		0.5f, 0.5f, 0.05f,
		0.5f, 0.5f, 0.05f,
		0.5f, 0.5f, 0.05f,
	};

	// VAO AND VBO FOR CUBE
	glGenVertexArrays(1, &vaoCube);
	glBindVertexArray(vaoCube);

	glGenBuffers(1, &vboCube);
	glBindBuffer(GL_ARRAY_BUFFER, vboCube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &vboCubeColour);
	glBindBuffer(GL_ARRAY_BUFFER, vboCubeColour);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeColour), cubeColour, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// VAO AND VBO FOR PYRAMID
	glGenVertexArrays(1, &vaoPyramid);
	glBindVertexArray(vaoPyramid);

	glGenBuffers(1, &vboPyramid);
	glBindBuffer(GL_ARRAY_BUFFER, vboPyramid);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramid), pyramid, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &vboPyramidColour);
	glBindBuffer(GL_ARRAY_BUFFER, vboPyramidColour);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidColour), pyramidColour, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// VAO AND VBO FOR GROUND
	glGenVertexArrays(1, &vaoGround);
	glBindVertexArray(vaoGround);

	glGenBuffers(1, &vboGround);
	glBindBuffer(GL_ARRAY_BUFFER, vboGround);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ground), ground, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &vboGroundColour);
	glBindBuffer(GL_ARRAY_BUFFER, vboGroundColour);
	glBufferData(GL_ARRAY_BUFFER, sizeof(groundColour), groundColour, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	GLuint vboIndexGround;
	glGenBuffers(1, &vboIndexGround);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndexGround);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(groundIndices), groundIndices, GL_STATIC_DRAW);

	// VAO AND VBO FOR SNOW POINTS
	glGenVertexArrays(1, &vaoSnowPoints);
	glBindVertexArray(vaoSnowPoints);

	glGenBuffers(1, &vboSnowPoints);
	glBindBuffer(GL_ARRAY_BUFFER, vboSnowPoints);
	glBufferData(GL_ARRAY_BUFFER, sizeof(snowPoints), snowPoints, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &vboSnowPointsColour);
	glBindBuffer(GL_ARRAY_BUFFER, vboSnowPointsColour);
	glBufferData(GL_ARRAY_BUFFER, sizeof(snowPointsColour), snowPointsColour, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// VAO AND VBO FOR GROUND LINES
	glGenVertexArrays(1, &vaoGroundLines);
	glBindVertexArray(vaoGroundLines);

	glGenBuffers(1, &vboGroundLines);
	glBindBuffer(GL_ARRAY_BUFFER, vboGroundLines);
	glBufferData(GL_ARRAY_BUFFER, sizeof(groundLines), groundLines, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &vboGroundLinesColour);
	glBindBuffer(GL_ARRAY_BUFFER, vboGroundLinesColour);
	glBufferData(GL_ARRAY_BUFFER, sizeof(groundLinesColour), groundLinesColour, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void paintGL(void)
{
	//TODO:
	//render your objects and control the transformation here
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); //specify the background color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 modelTransformMatrix;
	glm::mat4 modelTransformMatrix_rotate;
	glm::mat4 modelTransformMatrix_move;
	glm::mat4 modelTransformMatrix_scale;
	glm::mat4 modelTransformMatrix_translate;
	glm::mat4 modelTransformMatrix_translate_back;
	GLint modelTransformMatrixUniformLocation = glGetUniformLocation(programID, "modelTransformMatrix");
	
	glBindVertexArray(vaoCube);
	modelTransformMatrix_move = glm::translate(glm::mat4(), glm::vec3(x_delta * x_press_num, y_delta * y_press_num, 0.0f));;
	modelTransformMatrix_rotate = glm::rotate(glm::mat4(), rotate_cube, glm::vec3(0, 0, 1));;
	modelTransformMatrix = modelTransformMatrix_move * modelTransformMatrix_rotate;
	glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 12 * 3);

	glBindVertexArray(vaoPyramid);
	modelTransformMatrix_translate = glm::translate(glm::mat4(), glm::vec3(0, 0, -1));;
	modelTransformMatrix_scale = glm::scale(glm::mat4(), glm::vec3(1, 1, scale));
	modelTransformMatrix_translate_back = glm::translate(glm::mat4(), glm::vec3(0, 0, 1));
	modelTransformMatrix_rotate = glm::rotate(glm::mat4(), rotate_pyramid, glm::vec3(0, 0, 1));;
	modelTransformMatrix_move = glm::translate(glm::mat4(), glm::vec3(x_delta * x_press_num, y_delta * y_press_num, 0.0f));;
	modelTransformMatrix = modelTransformMatrix_move * modelTransformMatrix_rotate * modelTransformMatrix_translate_back * modelTransformMatrix_scale * modelTransformMatrix_translate;
	glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 6 * 3);

	glBindVertexArray(vaoGround);
	modelTransformMatrix = glm::mat4(1.0f);
	glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

	glBindVertexArray(vaoSnowPoints);
	modelTransformMatrix = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, snow_position * snow_delta));;
	glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);
	glPointSize(5.0);
	glDrawArrays(GL_POINTS, 0, 15);

	glBindVertexArray(vaoGroundLines);
	modelTransformMatrix = glm::mat4(1.0f);
	glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);
	glLineWidth(10.0);
	glDrawArrays(GL_LINES, 0, 16);

	glFlush();
	
}

void generateMVPMatrix(void)
{
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	glm::mat4 View = glm::lookAt(
		glm::vec3(0, -11, 3),
		glm::vec3(0, 0, 0), 
		glm::vec3(0, 0, 1)  
	);
	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 mvp = Projection * View * Model;

	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
}

void initializedGL(void) //run only once
{
	sendDataToOpenGL();
	installShaders();
	generateMVPMatrix();
}

int main(int argc, char *argv[])
{
	/*Initialization*/
	glutInit(&argc, argv);
	glutInitWindowSize(width, height);
	glutCreateWindow("Assignment 1");
	glewInit();

	/*Register different CALLBACK function for GLUT to response
	with different events, e.g. window sizing, mouse click or
	keyboard stroke */
	initializedGL();
	glutDisplayFunc(paintGL);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);

	/*Enter the GLUT event processing loop which never returns.
	it will call different registered CALLBACK according
	to different events. */
	glutMainLoop();

	return 0;
}