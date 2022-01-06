#ifndef __OBJ_H__
#define __OBJ_H__

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <chrono>
#include <thread>
#include <random>
#include <thread>
#include <mutex>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>
#include "old_shader.h"

// States for the UAVs
#define WAIT 0				// wait for 5 seconds
#define TAKEOFF 1			// move towards the sphere
#define RANDOMPATH 2		// generate a random velocity tangential to the surface of the sphere
#define SIMULATION 3		// move in random paths

// unit conversion
extern const float METERS_CONV_X;
extern const float METERS_CONV_Y;
extern const float METERS_CONV_Z;

// screen dimensions
extern const unsigned int SCR_WIDTH;
extern const unsigned int SCR_HEIGHT;

extern glm::mat4 view;
extern glm::mat4 projection;
extern glm::vec3 v_CO;

extern float R;				// radius of sphere
extern float K;				// spring constant
extern float B;				// damping constant
extern float delta;			// msec
extern int globalState;		// used to synchronize the states of all UAVs

class OBJ
{
/// Used to initialize the sphere and UAV objects.
/// Used to keep track of the movement of UAV objects.

private:

public:
	// general
	int id;								
	bool stopFlag = false;				// to end simulation
	int state = 0;

	// Used for threading application
	OBJ** suzy_list;					// used to keep track of all other UAV objects
	std::mutex mtx;
	std::thread suzyT;

	// Used to track collissions
	bool first = false;					// flag used to see if the current UAV thread detects collision first
	double timers[15] = { 0 };			// used to keep track of time elapsed since last collission with UAV at id i where i is an index
	bool timerFlag[15] = { false, false, false, false, false,			// timerFlag[i] is set to true when collission is detected with obj at i
							false, false, false, false, false,
							false, false, false, false, false };

	// position
	glm::vec3 v_SO = glm::vec3(0.0f, 0.0f, 0.0f);		// vector from origin to suzy
	glm::vec3 v_CS = glm::vec3(0.0f, 0.0f, 0.0f);		// vector from suzy to centre of sphere

	// velocity
	glm::vec3 vel = glm::vec3(0.0f, 0.0f, 0.0f);	 
//	glm::vec3 collisionVel = glm::vec3(0.0f, 0.0f, 0.0f);

	// acceleration
	glm::vec3 acc = glm::vec3(0.0f, 0.0f, 0.0f);

	// graphics
	GLuint VertexArrayID;
	GLuint programID;
	GLuint texture;
	GLuint textureID;

	GLuint MatrixID;
	GLuint ViewMatrixID;
	GLuint ModelMatrixID;

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;

	std::vector<unsigned short> indices;
	std::vector<glm::vec3> indexed_vertices;
	std::vector<glm::vec2> indexed_uvs;
	std::vector<glm::vec3> indexed_normals;

	GLuint vertexbuffer;
	GLuint uvbuffer;
	GLuint normalbuffer;
	GLuint elementbuffer;

	GLuint LightID;
	GLuint alphaID;		// for changing UAV color
	GLuint transID;		// for transperancy

	glm::mat4 ProjectionMatrix = glm::mat4(1.0f);
	glm::mat4 ModelMatrix = glm::mat4(1.0f);
	glm::mat4 MVP;

	// see OJB.cpp for function descriptions
	OBJ();	
	OBJ(glm::vec3 pos);
	void load(int check);
	void draw(int check);
	void start();
	void stop();
};

glm::vec3 addVec(glm::vec3 v1, glm::vec3 v2);

glm::vec3 subVec(glm::vec3 v1, glm::vec3 v2);

#endif // !__OBJ_H__
