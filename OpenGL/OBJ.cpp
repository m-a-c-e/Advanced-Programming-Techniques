#include "OBJ.h"

const float METERS_CONV_X = 0.6561;
const float METERS_CONV_Y = 0.8;
const float METERS_CONV_Z = 0.278851;
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 1024;


// GLOBALS
glm::mat4 view = glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.0f, -2.8f)), glm::radians(10.0f), glm::vec3(1.0f, 0.0f, 0.0f));
glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
glm::vec3 v_CO = glm::vec3(0.0f, 40.0f, 0.0f);
float R = 20.0f;
float K = 1.0;			// spring constant
float B = 0.2;			// damping constant
float delta = 0.03;		// msec
int globalState = 0;	// used to synchronize the states for all UAVs

glm::vec3 subVec(glm::vec3 v1, glm::vec3 v2)
{
/* 
* Used to subtract two vectors v1 and v2 of dimension 3.
* Returns the result after subtraction
*/
	glm::vec4 vec = glm::vec4(v1.x, v1.y, v1.z, 1.0f);
	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::translate(trans, -1.0f * v2);
	vec = trans * vec;
	return vec;
}

glm::vec3 addVec(glm::vec3 v1, glm::vec3 v2)
{
/*
* Used to add two vectors v1 and v2 of dimension 3.
* Returns the result after add
*/
	glm::vec4 vec = glm::vec4(v1.x, v1.y, v1.z, 1.0f);
	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::translate(trans, v2);
	vec = trans * vec;
	return vec;
}

void threadFunction(OBJ** suzy_list, int id)
{
/*
* External function used for multi-threading.
* --> Updates the suzy states
* --> Updates the suzy location
* --> Keeps track of collissions between suzies
* suzy_list: list of suzy pointers
* id:		 id assigned to current thread
*/
	// initialize a random number generator
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine generator(seed);
	std::uniform_int_distribution<int> distribution(-5, 5);

	// suzy corresponding to the current thread
	OBJ* suzy = suzy_list[id];
	float mag1;
	float mag2;
	float diff;
	glm::vec3 temp;

	// position and state update loop
	do
	{
		switch (suzy->state)
		{
		case WAIT:
			std::this_thread::sleep_for(std::chrono::milliseconds(5000));
			suzy->state = TAKEOFF;
			break;

		case TAKEOFF:
			// Here threads will try to access each others states. Make thread safe.
			if (std::abs(glm::length(suzy->v_CS) - 20.00f) <= 0.1)
			{
				if (suzy->id == 0)
				{
					suzy->mtx.lock();
					globalState = RANDOMPATH;
					suzy->state = globalState;
					suzy->mtx.unlock();
				}
				else
				{
					suzy->mtx.lock();
					suzy->state = globalState;
					suzy->mtx.unlock();
				}
			}
			break;
			
		case RANDOMPATH:
			// generate a random velocity vector	
			glm::vec3 randVec = glm::vec3((float)distribution(generator),
				(float)distribution(generator),
				(float)distribution(generator));
			
			// random tangential direction initialised
			randVec = glm::normalize(glm::cross(suzy->v_CS, randVec));

			// set magnitude
			mag1 = 3.0f;		// m/sec

			// set velocity
			suzy->mtx.lock();
			suzy->vel = mag1 * randVec;
			suzy->mtx.unlock();

			// once random velocity is set, go to simulation
			suzy->state = SIMULATION;
			break;

		case SIMULATION:
			// check for collissions
			for (int i = 0; i < 15; i += 2)
			{
				if (i == suzy->id)
				{
					continue;
				}

				suzy->mtx.lock();
				if (glm::length(subVec(suzy->v_SO, suzy_list[i]->v_SO)) - 3.0f <= 0.001f)
				{
					// Here current suzy at index suzy->id  is colliding with suzy at index i
					if (suzy_list[i]->first == false)		
					{
						// current suzy has detected the collission first

						// get the difference in time sinze last collission with suzy at id i
						if (glfwGetTime() - suzy->timers[i] >= 2.0)
						{
							// more than two seconds has passed since the last collision.
							// Allow collission and reset timer.
							suzy->timers[i] = glfwGetTime();					// set the collision time for suzy at suzy->id
							suzy_list[i]->timers[suzy->id] = glfwGetTime();		// set the collision time for suzy at i 
							suzy->first = true;									// set the flag so that when suzy at i detects this collision,
																				// it knows that the collision was already detected.
							// swap velocities
							temp = suzy->vel;
							suzy->vel = suzy_list[i]->vel;
							suzy_list[i]->vel = temp;
						}
						else
						{
							// less than one second has passed. skip colliding.
							suzy->first = true;
							temp = suzy->vel;
						}
					}
					else
					{
						// if current suzy is second to detect collision, reset the flag of the other suzy.
						// no need to update velocities or timer here since it was already done when the collision was detected first.
						suzy_list[i]->first = false;
					}
				}
				suzy->mtx.unlock();
			}
			break;
		}
		// 1. update pos
		glm::vec3 t1 = suzy->v_SO;
		glm::vec3 t2 = delta * suzy->vel;
		glm::vec3 t3 = (0.5f * 0.01f * 0.01f) * suzy->acc;
		t1 = addVec(t1, t2);
		t1 = addVec(t1, t3);

		// thread safety for position
		suzy->mtx.lock();
		suzy->v_SO = t1;
		suzy->mtx.unlock();
		suzy->v_CS = subVec(v_CO, suzy->v_SO);

		// 2. update v
		glm::vec3 velocity = suzy->vel;

		// thread safety for velocity /////////////////
		suzy->mtx.lock();
		suzy->vel = addVec(velocity, suzy->acc * delta);
		suzy->mtx.unlock();

		// 3. update a
		// acceleration due to the spring
		glm::vec3 dir = glm::normalize(suzy->v_CS);
		mag1 = K * (glm::length(suzy->v_CS) - R);

		// acceleration due to the damping factor
		mag2 = 0.0;
		if (mag1 <= 0)
		{
			mag2 = B * glm::dot(suzy->vel, suzy->v_CS);
		}
		else
		{
			mag2 = -1 * B * glm::dot(suzy->vel, suzy->v_CS);
		}
		suzy->acc = addVec(mag1 * dir, mag2 * dir);
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

	} while(!suzy->stopFlag);
}

// default constructor
OBJ::OBJ() {};

OBJ::OBJ(glm::vec3 pos)
{
/* Constructor to initialise suzy.
* pos: initial position
*/
	// set the vector from origin to suzy
	this->v_SO = pos;

	// set the vector from suzy to center of sphere
	this->v_CS = subVec(v_CO, v_SO);
};

void OBJ::load(int check)
{
/*
* Loading a .obj file.
* Input:
* check: 0 --> load suzy
*		 1 --> load sphere
*/
	glGenVertexArrays(1, &(this->VertexArrayID));
	glBindVertexArray(this->VertexArrayID);

	this->programID = LoadShaders("StandardShading.vertexshader",
								"StandardTransparentShading.fragmentshader");
	this->MatrixID = glGetUniformLocation(programID, "MVP");
	this->ViewMatrixID = glGetUniformLocation(programID, "V");
	this->ModelMatrixID = glGetUniformLocation(programID, "M");
	this->texture = loadDDS("uvmap.DDS");
	this->textureID = glGetUniformLocation(programID, "myTextureSampler");

	if (check == 0)
	{
		loadOBJ("suzanne.obj", this->vertices, this->uvs, this->normals);
	}
	else
	{
		loadOBJ("sphere.obj", this->vertices, this->uvs, this->normals);
	}
	indexVBO(this->vertices, this->uvs, this->normals, this->indices, this->indexed_vertices, this->indexed_uvs, this->indexed_normals);

	glGenBuffers(1, &(this->vertexbuffer));
	glBindBuffer(GL_ARRAY_BUFFER, this->vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, (this->indexed_vertices).size() * sizeof(glm::vec3), &(this->indexed_vertices)[0], GL_STATIC_DRAW);

	glGenBuffers(1, &(this->uvbuffer));
	glBindBuffer(GL_ARRAY_BUFFER, this->uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, (this->indexed_uvs).size() * sizeof(glm::vec2), &(this->indexed_uvs)[0], GL_STATIC_DRAW);

	glGenBuffers(1, &(this->normalbuffer));
	glBindBuffer(GL_ARRAY_BUFFER, this->normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, (this->indexed_normals).size() * sizeof(glm::vec3), &(this->indexed_normals)[0], GL_STATIC_DRAW);

	glGenBuffers(1, &(this->elementbuffer));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (this->indices).size() * sizeof(unsigned short), &(this->indices)[0], GL_STATIC_DRAW);

	glUseProgram(this->programID);
	this->LightID = glGetUniformLocation(this->programID, "LightPosition_worldspace");
	this->alphaID = glGetUniformLocation(this->programID, "alp");
	this->transID = glGetUniformLocation(this->programID, "trans");

}

void OBJ::draw(int check)
{
/*
* Used to draw a 3D model
* Input:
* check: 0 --> draw suzy
*		 1 --> draw sphere
*/
	// Activate our shader
	glUseProgram(this->programID);
	this->ModelMatrix = glm::mat4(1.0f);
	if (check == 0)
	{
		// For suzy
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// scale to fit 0.2 m bounding box
		this->ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.0125, 0.015f, 0.025f));

		// since the the world is rotated 90 degrees
		this->ModelMatrix = glm::rotate(ModelMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		// to rotate it back so that it faces up
		this->ModelMatrix = glm::rotate(ModelMatrix, glm::radians(+90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		// update position here
		// thread safety for position while drawing
		this->mtx.lock();
		this->ModelMatrix = glm::translate(ModelMatrix, glm::vec3(v_SO.x * METERS_CONV_X, v_SO.y * METERS_CONV_Y, v_SO.z * METERS_CONV_Z));
		this->mtx.unlock();
		//////////////////////////////

		// Changing color for suzy
		glUniform1f(this->alphaID, abs(sin(glfwGetTime()) / 2.0f) + 0.5f);
		glUniform1f(this->transID, 1.0f);
	}
	else
	{
		// For sphere
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		this->ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.044444f, 0.072727f, 0.035f));
		this->ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f * METERS_CONV_X, 10.0f * METERS_CONV_X, 0.0f * METERS_CONV_Z));

		// Make sphere transperant
		glUniform1f(this->alphaID, 1.0f);
		glUniform1f(this->transID, 0.3f);
	}

	this->MVP = projection * view * this->ModelMatrix;

	glUniformMatrix4fv(this->MatrixID, 1, GL_FALSE, &this->MVP[0][0]);
	glUniformMatrix4fv(this->ModelMatrixID, 1, GL_FALSE, &this->ModelMatrix[0][0]);
	glUniformMatrix4fv(this->ViewMatrixID, 1, GL_FALSE, &view[0][0]);

	glm::vec3 lightPos = glm::vec3(4, 4, 4);
	glUniform3f(this->LightID, lightPos.x, lightPos.y, lightPos.z);

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->texture);

	// Set our "myTextureSampler" sampler to use Texture Unit 0
	glUniform1i(this->textureID, 0);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, this->vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, this->uvbuffer);
	glVertexAttribPointer(
		1,                                // attribute
		2,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// 3rd attribute buffer : normals
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, this->normalbuffer);
	glVertexAttribPointer(
		2,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// Index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->elementbuffer);

	// Draw the triangles !
	glDrawElements(
		GL_TRIANGLES,      // mode
		this->indices.size(),    // count
		GL_UNSIGNED_SHORT,   // type
		(void*)0           // element array buffer offset
	);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	if (check == 0)
	{
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND);
	}
}

void OBJ::start()
{
/*
* Used to start thread.
*/
	this->suzyT = std::thread(threadFunction, this->suzy_list, this->id);
}

void OBJ::stop()
{
/*
* Used to stop a thread.
*/
	this->stopFlag = true;
	this->suzyT.join();
}
