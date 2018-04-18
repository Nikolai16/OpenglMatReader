
///////////////////////////////////////////////////////////////////////
//
// triangles.cpp
//
///////////////////////////////////////////////////////////////////////

#define _CRT_SECURE_NO_WARNINGS

using namespace std;

#include "vgl.h"
#include "LoadShaders.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "Angel.h"
#include "mat.h"
#include <stdio.h>
#include "iostream"
#include <fstream>
#include <string>
#include <vector>

using namespace std;

const float bounds = 1.0f;  //Size of the grid
const int subdivisions = 66; //Number of divisions in each direction

GLuint program;
//const int NumVertices = subdivisions * subdivisions * 6;
const int NumVertices = 100;
GLfloat theta = 0.0f;
GLfloat speed = 0.0F;
float z_scale = 0.1;
bool stop = false;
typedef glm::vec4 point4;
typedef glm::vec4 color4;
GLfloat u, v = 0.0f;
GLfloat w = 1.0f;
point4 light_position = point4(u, v, w, 1.0);
glm::vec3 normals[NumVertices];
glm::vec3 rotation_axis = glm::vec3(0, 0, 1);

GLushort cube_Indices[NumVertices];
GLfloat vertices[NumVertices][4];

// Model-view and projection matrices uniform location
GLuint  ModelView, Projection;

int derpsize;


//----------------------------------------------------------------------------

void loadFile()
{
	FILE *file;
	errno_t err = fopen_s(&file, "plzwork.obj", "r");
	int vertexCount = 0;
	int indicesCount = 0;
	int textCount = 0;
	int normalCount = 0;

	if (file == nullptr)
	{
		std::cout << "derp" << std::endl;
	}

	GLfloat vertexes[125][4];
	GLfloat textTemp[125][2];
	unsigned int texIndex[125];
	GLfloat normalTemp[125][3];
	unsigned int normalIndex[125];

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
			cube_Indices[indicesCount] = derp[0] - 1;
			cube_Indices[indicesCount + 1] = derp[3] - 1;
			cube_Indices[indicesCount + 2] = derp[6] - 1;

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
	/*for (int i = 0; i < 36; i++)
	{
		unsigned int tmp = texIndex[i];

		//std::cout << tmp << std::endl;
		textCorordenats[i][0] = textTemp[tmp][0];
		textCorordenats[i][1] = textTemp[tmp][1];

		//std::cout << "X: " << textCorordenats[i][0] << " Y: " << textCorordenats[i][1] << std::endl;
	}*/

	//Rearrange Normals
	for (int i = 0; i < 36; i++)
	{
		unsigned int tmp = normalIndex[i];

		//std::cout << tmp << std::endl;
		normals[i].x = normalTemp[tmp][0];
		normals[i].y = normalTemp[tmp][1];
		normals[i].z = normalTemp[tmp][2];

		std::cout << "X: " << normals[i].x << " Y: " << normals[i].y << " Z: " << normals[i].z << std::endl;
	}

	for (int i = 0; i < 36; i++)
	{
		unsigned int tmp = cube_Indices[i];

		//std::cout << tmp << std::endl;
		vertices[i][0] = vertexes[tmp][0];
		vertices[i][1] = vertexes[tmp][1];
		vertices[i][2] = vertexes[tmp][2];
		vertices[i][3] = vertexes[tmp][3];

		std::cout << "X: " << vertices[i][0] << " Y: " << vertices[i][1] << " Z: " << vertices[i][2] << std::endl;
	}
}

// OpenGL initialization
void
init()
{
	int counter = 0;

	//GLfloat vertices[NumVertices][4];

	loadFile();
	
	float delta = (bounds * 2) / (subdivisions - 1);
	/*for (float left = -bounds; left <= bounds; left += delta) {
		for (float bottom = -bounds; bottom <= bounds; bottom += delta) {
			//First Triangle
			vertices[counter][0] = left;
			vertices[counter][1] = bottom;
			vertices[counter][2] = z_scale*sin((left+bounds)*M_PI);
			vertices[counter][3] = 1.0;
			//normals[counter] = {0, 0, 1};

			vertices[counter+1][0] = left + delta;
			vertices[counter+1][1] = bottom;
			vertices[counter+1][2] = z_scale*sin((left + delta + bounds)*M_PI);
			vertices[counter+1][3] = 1.0;
			//normals[counter+1] = { 0, 0, 1 };
	
			vertices[counter + 2][0] = left;
			vertices[counter + 2][1] = bottom + delta;
			vertices[counter + 2][2] = z_scale*sin((left + bounds)*M_PI);
			vertices[counter + 2][3] = 1.0;
			//normals[counter+2] = { 0, 0, 1 };

			glm::vec3 v0 = glm::vec3(vertices[counter][0], vertices[counter][1], vertices[counter][2]);
			glm::vec3 v1 = glm::vec3(vertices[counter+1][0], vertices[counter+1][1], vertices[counter+1][2]);
			glm::vec3 v2 = glm::vec3(vertices[counter+2][0], vertices[counter+2][1], vertices[counter+2][2]);

			glm::vec3 u = v1 - v0; glm::vec3 v = v2 - v0;
			glm::vec3 temp = glm::cross(u, v);
			glm::vec3 n = glm::normalize(temp);

			normals[counter] = n; normals[counter+1] = n; normals[counter+2] = n;
			//Draw the second triangle

			vertices[counter + 3][0] = left + delta;
			vertices[counter + 3][1] = bottom;
			vertices[counter + 3][2] = z_scale*sin((left + delta + bounds)*M_PI);
			vertices[counter + 3][3] = 1.0;
			//normals[counter + 3] = { 0, 0, 1 };

			vertices[counter + 4][0] = left;
			vertices[counter + 4][1] = bottom + delta;
			vertices[counter + 4][2] = z_scale*sin((left + bounds)*M_PI);
			vertices[counter + 4][3] = 1.0;
			//normals[counter + 4] = { 0, 0, 1 };

			vertices[counter + 5][0] = left + delta;
			vertices[counter + 5][1] = bottom + delta;
			vertices[counter + 5][2] = z_scale*sin((left + delta + bounds)*M_PI);
			vertices[counter + 5][3] = 1.0;
			//normals[counter + 5] = { 0, 0, 1 };

			v0 = glm::vec3(vertices[counter +3][0], vertices[counter + 3][1], vertices[counter + 3][2]);
			v1 = glm::vec3(vertices[counter + 4][0], vertices[counter + 4][1], vertices[counter + 4][2]);
			v2 = glm::vec3(vertices[counter + 5][0], vertices[counter + 5][1], vertices[counter + 5][2]);

			u = v1 - v0; v = v2 - v0;
			temp = glm::cross(v, u);
			n = glm::normalize(temp);

			normals[counter + 3] = n; normals[counter + 4] = n; normals[counter + 5] = n;

			counter += 6;

		}

	}*/



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

	// Create and initialize a buffer object
	GLuint buffer[2];
	glGenBuffers(2, buffer);
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

	/*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_Indices), cube_Indices, GL_STATIC_DRAW);
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &derpsize);
	glEnableVertexAttribArray(2);*/

	// Initialize shader lighting parameters
	point4 light_position(0.0, 0.0, 2.0, 0.0); //Position of diffuse light
											   //color4 light_ambient(0.2, 0.2, 0.2, 1.0);
	color4 light_diffuse(1.0, 1.0, 0.0, 1.0); //Intensity or Lumination
											  //	color4 light_specular(1.0, 1.0, 1.0, 1.0);

											  //	color4 material_ambient(1.0, 0.0, 1.0, 1.0);
											  //color4 material_diffuse(1.0, 0.8, 0.0, 1.0);
	color4 material_diffuse(1.0, 1.0, 1.0, 1.0);
	//	color4 material_specular(1.0, 0.0, 1.0, 1.0);
	float  material_shininess = 5.0;

	//	color4 ambient_product = light_ambient * material_ambient;
	color4 diffuse_product = light_diffuse * material_diffuse;
	//	color4 specular_product = light_specular * material_specular;

	//	GLfloat tmp1[4] = {ambient_product.r, ambient_product.g, ambient_product.b, ambient_product.a };
	GLfloat tmp2[4] = { diffuse_product.r, diffuse_product.g, diffuse_product.b, diffuse_product.a };
	//	GLfloat tmp3[4] = {specular_product.r, specular_product.g, specular_product.b, specular_product.a};

	//	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"),1, tmp1);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, tmp2);
	//	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"),1, tmp3);

	GLfloat tmp_p[4] = { light_position.x, light_position.y, light_position.z, light_position.w };

	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, tmp_p);

	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);

	// Retrieve transformation uniform variable locations
	ModelView = glGetUniformLocation(program, "ModelView");
	Projection = glGetUniformLocation(program, "Projection");

	glEnable(GL_DEPTH_TEST);

	glClearColor(0.0, 0.0, 0.0, 1.0); /* white background */


}

//----------------------------------------------------------------------------

void
display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::vec3 at(0.0, 0.0, 0.0);
	glm::vec3 eye(0.0, 0.0, 0.5);
	glm::vec3 up(0.0, 1.0, 0.0);

	glm::mat4 model_view = glm::lookAt(eye, at, up);

	model_view = glm::rotate(model_view, theta, rotation_axis);

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, &model_view[0][0]);

	light_position = point4(u, v, w, 1.0);
	GLfloat tmp_p[4] = { light_position.x, light_position.y, light_position.z, light_position.w };

	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, tmp_p);


	//glDrawElements(GL_TRIANGLES, derpsize / sizeof(GLshort), GL_UNSIGNED_SHORT, 0);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);



	glutSwapBuffers();


}

void idle()
{
	if (!stop)
	{
		theta += speed;
	}
	glutPostRedisplay();
}

//----------------------------------------------------------------------------

void
keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{

	case 'x':
		rotation_axis = glm::vec3(1, 0, 0);
		break;
	case 'y':
		rotation_axis = glm::vec3(0, 1, 0);
		break;
	case 'z':
		rotation_axis = glm::vec3(0, 0, 1);
		break;
	case 's':
		stop = !stop;
		break;
	case '=':
		speed += 0.001;
		break;
	case '-':
		speed -= 0.001;
		if (speed <= 0) speed = 0.01;
		break;
	case 'f':
		w += 0.05;
		break;

	case 'n':
		w -= 0.05;
		break;


	case 'a':
		u += 0.05;
		break;

	case 'd':
		u -= 0.05;
		break;

	case 'j':
		v += 0.05;
		break;

	case 'l':
		v -= 0.05;
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
