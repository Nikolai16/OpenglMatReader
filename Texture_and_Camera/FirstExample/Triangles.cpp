using namespace std;

#define _CRT_SECURE_NO_WARNINGS

#include "vgl.h"
#include "LoadShaders.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
//#include "Angel.h"
#include "mat.h"
#include "SOIL.h"
#include <stdio.h>
#include "iostream"
#include <fstream>
#include <string>
#include <vector>

using namespace std;

GLuint program;
const int NumVertices = 900;
const int NumVertices2 = 20;
GLfloat theta = 0.0f;
GLfloat speed = 0.0F;
bool stop = false;
typedef glm::vec4 point4;
typedef glm::vec4 color4;
GLfloat u, v = 0.0f;
GLfloat w = 1.0f;
point4 light_position = point4(u, v, w, 1.0);
glm::vec3 rotation_axis = glm::vec3(0, 0, 1);

GLushort cube_Indices[NumVertices];
GLfloat vertices[NumVertices][4];
GLfloat textureCoordinates[NumVertices][2] = {};
glm::vec3 normals[NumVertices];

GLushort cube_Indices2[NumVertices];
GLfloat vertices2[NumVertices][4];
GLfloat textureCoordinates2[NumVertices][2] = {};
glm::vec3 normals2[NumVertices];

GLuint  ModelView, Projection;

GLuint texture[1];

GLuint buffer[3];

unsigned char* image1;
unsigned char* image2;
GLint width, height;
GLint width2, height2;

//give me dat teapot
void load_first_obj() {
	FILE *file;
	errno_t err = fopen_s(&file, "ConeDerp.obj", "r");
	int vertexCount = 0;
	int indicesCount = 0;
	int textCount = 0;
	int normalCount = 0;
	int number_of_faces = 288*3;

	if (file == nullptr)
		std::cout << "ERROR COULD NOT LOAD FILE" << std::endl;

	GLfloat vertexes[482][4];
	GLfloat textTemp[560][2];
	unsigned int texIndex[960];
	GLfloat normalTemp[482][3];
	unsigned int normalIndex[960];

	while (true)
	{
		char tempText[128];
		int eof = fscanf(file, "%s", tempText);
		if (eof == EOF) {
			break;
		}
		//this line contains vertex coords
		if (strcmp(tempText, "v") == 0) {
			glm::vec3 tempVec;
			fscanf_s(file, "%f %f %f\n", &tempVec.x, &tempVec.y, &tempVec.z);
			vertexes[vertexCount][0] = tempVec.x;
			vertexes[vertexCount][1] = tempVec.y;
			vertexes[vertexCount][2] = tempVec.z;
			vertexes[vertexCount][3] = 1.0f;
			vertexCount++;
		}
		//this line contains texture coords
		else if (strcmp(tempText, "vt") == 0) {
			glm::vec2 tempVec;
			fscanf_s(file, "%f %f\n", &tempVec.x, &tempVec.y);
			textTemp[textCount][0] = tempVec.x;
			textTemp[textCount][1] = tempVec.y;
			textCount++;
		}
		//this line contains normals
		else if (strcmp(tempText, "vn") == 0) {
			glm::vec3 tempVec;
			fscanf_s(file, "%f %f %f\n", &tempVec.x, &tempVec.y, &tempVec.z);
			normalTemp[normalCount][0] = tempVec.x;
			normalTemp[normalCount][1] = tempVec.y;
			normalTemp[normalCount][2] = tempVec.z;
			normalCount++;
		}
		//this line contains faces
		else if (strcmp(tempText, "f") == 0) {
			unsigned int derp[9];
			fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &derp[0], &derp[1], &derp[2], &derp[3], &derp[4], &derp[5], &derp[6], &derp[7], &derp[8]);
			cube_Indices[indicesCount] = derp[0] - 1;
			cube_Indices[indicesCount + 1] = derp[3] - 1;
			cube_Indices[indicesCount + 2] = derp[6] - 1;

			texIndex[indicesCount] = derp[1] - 1;
			texIndex[indicesCount + 1] = derp[4] - 1;
			texIndex[indicesCount + 2] = derp[7] - 1;

			normalIndex[indicesCount] = derp[2] - 1;
			normalIndex[indicesCount + 1] = derp[5] - 1;
			normalIndex[indicesCount + 2] = derp[5] - 1;

			//std::cout << "Part 1 " << texIndex[indicesCount] << " Part 2 " << texIndex[indicesCount + 1] << " Part 3 " << texIndex[indicesCount + 2] << std::endl;

			indicesCount += 3;
		}
		//this line contains useless shit
		else {
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}
	}

	for (int i = 0; i < number_of_faces; i++)
	{
		unsigned int tmp = texIndex[i];

		//std::cout << tmp << std::endl;

		textureCoordinates[i][0] = textTemp[tmp][0];
		textureCoordinates[i][1] = textTemp[tmp][1];
	}

	for (int i = 0; i < number_of_faces; i++)
	{
		unsigned int tmp = normalIndex[i];

		normals[i].x = normalTemp[tmp][0];
		normals[i].y = normalTemp[tmp][1];
		normals[i].z = normalTemp[tmp][2];

	}

	for (int i = 0; i < number_of_faces; i++)
	{
		unsigned int tmp = cube_Indices[i];

		//std::cout << tmp << std::endl;

		vertices[i][0] = vertexes[tmp][0];
		vertices[i][1] = vertexes[tmp][1];
		vertices[i][2] = vertexes[tmp][2];
		vertices[i][3] = vertexes[tmp][3];

		//std::cout << "X: " << vertices[i][0] << " Y: " << vertices[i][1] << " Z: " << vertices[i][2] << std::endl;
	}
}
//load muh diamond
void load_second_obj()
{
	FILE *file;
	errno_t err = fopen_s(&file, "diamond.obj", "r");
	int vertexCount = 0;
	int indicesCount = 0;
	int textCount = 0;
	int normalCount = 0;
	int number_of_faces = 9;

	if (file == nullptr)
	{
		std::cout << "ERROR LOADING SECOND OBJ FILE" << std::endl;
	}

	GLfloat vertexes[122][4];
	GLfloat textTemp[127][2];
	unsigned int texIndex[864];
	GLfloat normalTemp[50][3];
	unsigned int normalIndex[864];

	while (true)
	{
		char tempText[128];
		int eof = fscanf(file, "%s", tempText);
		if (eof == EOF)
		{
			break;
		}

		if (strcmp(tempText, "v") == 0)
		{
			glm::vec3 tempVec;
			fscanf_s(file, "%f %f %f\n", &tempVec.x, &tempVec.y, &tempVec.z);
			vertexes[vertexCount][0] = tempVec.x;
			vertexes[vertexCount][1] = tempVec.y;
			vertexes[vertexCount][2] = tempVec.z;
			vertexes[vertexCount][3] = 1.0f;

			//std::cout << "vertex x " << vertices[vertexCount][0] << " vertex y " << vertices[vertexCount][1] << " vertex z " << vertices[vertexCount][2] << std::endl;
			vertexCount++;
		}

		else if (strcmp(tempText, "vt") == 0)
		{
			glm::vec2 tempVec;
			fscanf_s(file, "%f %f\n", &tempVec.x, &tempVec.y);
			textTemp[textCount][0] = tempVec.x;
			textTemp[textCount][1] = tempVec.y;

			//std::cout << "vertex x " << vertices[vertexCount][0] << " vertex y " << vertices[vertexCount][1] << " vertex z " << vertices[vertexCount][2] << std::endl;
			textCount++;
		}

		else if (strcmp(tempText, "vn") == 0)
		{
			glm::vec3 tempVec;
			fscanf_s(file, "%f %f %f\n", &tempVec.x, &tempVec.y, &tempVec.z);
			normalTemp[normalCount][0] = tempVec.x;
			normalTemp[normalCount][1] = tempVec.y;
			normalTemp[normalCount][2] = tempVec.z;

			//std::cout << "vertex x " << normalTemp[normalCount][0] << " vertex y " << normalTemp[normalCount][1] << " vertex z " << normalTemp[normalCount][2] << std::endl;
			normalCount++;
		}

		else if (strcmp(tempText, "f") == 0)
		{
			unsigned int derp[9];
			fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &derp[0], &derp[1], &derp[2], &derp[3], &derp[4], &derp[5], &derp[6], &derp[7], &derp[8]);
			cube_Indices2[indicesCount] = derp[0] - 1;
			cube_Indices2[indicesCount + 1] = derp[3] - 1;
			cube_Indices2[indicesCount + 2] = derp[6] - 1;

			texIndex[indicesCount] = derp[1] - 1;
			texIndex[indicesCount + 1] = derp[4] - 1;
			texIndex[indicesCount + 2] = derp[7] - 1;

			normalIndex[indicesCount] = derp[2] - 1;
			normalIndex[indicesCount + 1] = derp[5] - 1;
			normalIndex[indicesCount + 2] = derp[5] - 1;

			//std::cout << "Part 1 " << normalIndex[indicesCount] << " Part 2 " << normalIndex[indicesCount + 1] << " Part 3 " << normalIndex[indicesCount + 2] << std::endl;
			//system("pause");
			indicesCount += 3;
		}
		else {
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}
	}

	//Rearrange Textures (Not here now no way to load textures)
	for (int i = 0; i < number_of_faces; i++)
	{
		unsigned int tmp = texIndex[i];
		textureCoordinates2[i][0] = textTemp[tmp][0];
		textureCoordinates2[i][1] = textTemp[tmp][1];
	}

	for (int i = 0; i < number_of_faces; i++)
	{
		unsigned int tmp = normalIndex[i];
		normals2[i].x = normalTemp[tmp][0];
		normals2[i].y = normalTemp[tmp][1];
		normals2[i].z = normalTemp[tmp][2];
	}

	for (int i = 0; i < number_of_faces; i++)
	{
		unsigned int tmp = cube_Indices2[i];

		//std::cout << tmp << std::endl;
		vertices2[i][0] = vertexes[tmp][0];
		vertices2[i][1] = vertexes[tmp][1];
		vertices2[i][2] = vertexes[tmp][2];
		vertices2[i][3] = vertexes[tmp][3];

		//std::cout << "X: " << vertices[i][0] << " Y: " << vertices[i][1] << " Z: " << vertices[i][2] << std::endl;
	}
}

//----------------------------------------------------------------------------

// OpenGL initialization
void init() {
	load_first_obj();
	//load_second_obj();
	float delta = 0.2f;

	image1 = SOIL_load_image("coke.png", &width, &height, 0, SOIL_LOAD_RGB);
	image2 = SOIL_load_image("apple.png", &width2, &height2, 0, SOIL_LOAD_RGB);

	glGenTextures(1, texture);

	ShaderInfo shaders[] = {
		{ GL_VERTEX_SHADER, "triangles.vert" },
	{ GL_FRAGMENT_SHADER, "triangles.frag" },
	{ GL_NONE, NULL }
	};

	program = LoadShaders(shaders);
	glUseProgram(program);

	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(3, buffer);

	point4 light_position(0.0, 0.0, 2.0, 0.0);

	color4 light_diffuse(1.0, 1.0, 0.0, 1.0);

	color4 material_diffuse(1.0, 1.0, 1.0, 1.0);

	float  material_shininess = 5.0;

	color4 diffuse_product = light_diffuse * material_diffuse;

	GLfloat tmp2[4] = { diffuse_product.r, diffuse_product.g, diffuse_product.b, diffuse_product.a };

	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, tmp2);

	GLfloat tmp_p[4] = { light_position.x, light_position.y, light_position.z, light_position.w };

	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, tmp_p);

	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);

	ModelView = glGetUniformLocation(program, "ModelView");
	Projection = glGetUniformLocation(program, "Projection");

	glEnable(GL_DEPTH_TEST);

	glClearColor(0.0, 0.0, 0.0, 1.0);
}

//----------------------------------------------------------------------------

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::vec3 at(0.0, 0.0, 0.0);
	glm::vec3 eye(0.0, 0.0, 0.5);
	glm::vec3 up(0.0, 1.0, 0.0);

	glm::mat4 model_view = glm::lookAt(eye, at, up);

	//this stuff has to be here becase reasons
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_FLOAT, image1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image1);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindAttribLocation(program, 0, "vPosition");
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
	glBindAttribLocation(program, 1, "vNormal");
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoordinates), textureCoordinates, GL_STATIC_DRAW);
	glBindAttribLocation(program, 2, "vTexCoord");
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(2);

	model_view = glm::rotate(model_view, theta, rotation_axis);

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, &model_view[0][0]);

	light_position = point4(u, v, w, 1.0);
	GLfloat tmp_p[4] = { light_position.x, light_position.y, light_position.z, light_position.w };

	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, tmp_p);

	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	/*
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_FLOAT, image2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width2, height2, 0, GL_RGB, GL_UNSIGNED_BYTE, image2);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
	glBindAttribLocation(program, 0, "vPosition");
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals2), normals2, GL_STATIC_DRAW);
	glBindAttribLocation(program, 1, "vNormal");
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoordinates2), textureCoordinates2, GL_STATIC_DRAW);
	glBindAttribLocation(program, 2, "vTexCoord");
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(2);

	model_view = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, 0.0));
	model_view = glm::rotate(model_view, -theta, rotation_axis);

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, &model_view[0][0]);

	light_position = point4(u, v, w, 1.0);
	GLfloat tmp_p2[4] = { light_position.x, light_position.y, light_position.z, light_position.w };

	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, tmp_p2);

	glDrawArrays(GL_TRIANGLES, 0, NumVertices);*/

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	glutSwapBuffers();


}

void idle() {
	if (!stop)
		theta += speed;

	glutPostRedisplay();
}

//----------------------------------------------------------------------------

void
keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'e':
		w += 0.05;
		break;

	case 'q':
		w -= 0.05;
		break;

	case 'd':
		u += 0.05;
		break;

	case 'a':
		u -= 0.05;
		break;

	case 'w':
		v += 0.05;
		break;

	case 's':
		v -= 0.05;
		break;
	case 'x':
		rotation_axis = glm::vec3(1, 0, 0);
		break;
	case 'y':
		rotation_axis = glm::vec3(0, 1, 0);
		break;
	case 'z':
		rotation_axis = glm::vec3(0, 0, 1);
		break;
	case 'c':
		stop = !stop;
		break;
	case '=':
		speed += 0.001;
		break;
	case '-':
		speed -= 0.001;
		if (speed <= 0) speed = 0.01;
		break;
	}
	glutPostRedisplay();
}

//----------------------------------------------------------------------------

void
reshape(int width, int height)
{
	glViewport(0, 0, width, height);

	GLfloat left = -2.0, right = 2.0;
	GLfloat top = 2.0, bottom = -2.0;
	GLfloat zNear = 0.001, zFar = 20.0;

	GLfloat aspect = GLfloat(width) / height;

	if (aspect > 1.0) {
		left *= aspect;
		right *= aspect;
	}
	else {
		top /= aspect;
		bottom /= aspect;
	}

	glm::mat4 projection = glm::ortho(left, right, bottom, top, zNear, zFar);

	glUniformMatrix4fv(Projection, 1, GL_TRUE, &projection[0][0]);
}

//----------------------------------------------------------------------------

int
main(int argc, char **argv)
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("Sphere");
	glewExperimental = GL_TRUE;
	glewInit();

	GLenum err = glGetError();
	std::cout << err;

	init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	glutMainLoop();
	return 0;
}
