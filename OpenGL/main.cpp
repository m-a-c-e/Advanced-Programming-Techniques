/*
 * Author: Manan Patel
 * Class: ECE 6122
 * Last Date Modified: 12/04/2021
 *
 * Description:
 * This application simulates a half time show demonstrations using UAVs. 
 * It uses openGL for graphics and std::thread for multi-threading.
 * 
 * This project would not have been possible without the help from the
 * online tutorials mentioned below. Some of the code has been implemented
 * directly from these websites.
 * 1. https://learnopengl.com/Introduction
 * 2. https://github.com/opengl-tutorials/ogl
 */

#include "OBJ.h"

// unit conversion
extern const float METERS_CONV_X;
extern const float METERS_CONV_Y;
extern const float METERS_CONV_Z;

// screen width and height
extern const unsigned int SCR_WIDTH;
extern const unsigned int SCR_HEIGHT;

// global variables
extern glm::mat4 view;
extern glm::mat4 projection;
extern glm::vec3 v_CO;				// vector from centre of sphere to origin
extern float R;						// radius of sphere (meters)
extern float K;						// spring constant
extern float delta;					// timestep for main loop

// GLFW window and its callback for resizing
GLFWwindow* window;					
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

void APIENTRY DebugOutputCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {

	printf("OpenGL Debug Output message : ");

	if (source == GL_DEBUG_SOURCE_API_ARB)					printf("Source : API; ");
	else if (source == GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB)	printf("Source : WINDOW_SYSTEM; ");
	else if (source == GL_DEBUG_SOURCE_SHADER_COMPILER_ARB)	printf("Source : SHADER_COMPILER; ");
	else if (source == GL_DEBUG_SOURCE_THIRD_PARTY_ARB)		printf("Source : THIRD_PARTY; ");
	else if (source == GL_DEBUG_SOURCE_APPLICATION_ARB)		printf("Source : APPLICATION; ");
	else if (source == GL_DEBUG_SOURCE_OTHER_ARB)			printf("Source : OTHER; ");

	if (type == GL_DEBUG_TYPE_ERROR_ARB)						printf("Type : ERROR; ");
	else if (type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB)	printf("Type : DEPRECATED_BEHAVIOR; ");
	else if (type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB)	printf("Type : UNDEFINED_BEHAVIOR; ");
	else if (type == GL_DEBUG_TYPE_PORTABILITY_ARB)			printf("Type : PORTABILITY; ");
	else if (type == GL_DEBUG_TYPE_PERFORMANCE_ARB)			printf("Type : PERFORMANCE; ");
	else if (type == GL_DEBUG_TYPE_OTHER_ARB)				printf("Type : OTHER; ");

	if (severity == GL_DEBUG_SEVERITY_HIGH_ARB)				printf("Severity : HIGH; ");
	else if (severity == GL_DEBUG_SEVERITY_MEDIUM_ARB)		printf("Severity : MEDIUM; ");
	else if (severity == GL_DEBUG_SEVERITY_LOW_ARB)			printf("Severity : LOW; ");

	// You can set a breakpoint here ! Your debugger will stop the program,
	// and the callstack will immediately show you the offending call.
	printf("Message : %s\n", message);
}

int main()
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// ARB_debug_output is a bit special, 
	// it requires creating the OpenGL context
	// with paticular flags.
	// GLFW exposes it this way; if you use SDL, SFML, freeGLUT
	// or other, check the documentation.
	// If you use custom code, read the spec : 
	// http://www.opengl.org/registry/specs/ARB/debug_output.txt
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1024, 768, "Tutorial 12 - Extensions", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Example 2 :
	if (GLEW_ARB_debug_output) {
		printf("The OpenGL implementation provides debug output. Let's use it !\n");
		glDebugMessageCallbackARB(&DebugOutputCallback, NULL);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
	}
	else {
		printf("ARB_debug_output unavailable. You have to use glGetError() and/or gDebugger to catch mistakes.\n");
	}
	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// grey background
	glClearColor(0.8f, 0.8f, 0.8f, 0.8f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST); // SHOULD BE GL_DEPTH_TEST ! WILL TRIGGER AN ERROR MESSAGE !
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);  // SHOULD BE GL_LESS ! WILL TRIGGER AN ERROR MESSAGE 
	glEnable(GL_CULL_FACE);

	// build and compile our shader program for football field
// ------------------------------------
	Shader ourShader("texture_vs.txt", "texture_fs.txt");
	float old_vertices[] = {
		// positions          // texture coords
		 1.0f,  1.0f, 0.0f,   1.0f, 1.0f, // top right
		 1.0f, -1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		-1.0f, -1.0f, 0.0f,   0.0f, 0.0f, // bottom left
		-1.0f,  1.0f, 0.0f,   0.0f, 1.0f  // top left 
	};

	unsigned int old_indices[] = {
	0, 1, 3, // first triangle
	1, 2, 3  // second triangle
	};

	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(old_vertices), old_vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(old_indices), old_indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// load and create a texture 
 // -------------------------
	unsigned int texture1;
	// texture 1
	// ---------
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char* data = stbi_load("ff.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	// tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
	// -------------------------------------------------------------------------------------------
	ourShader.use();
	ourShader.setInt("texture1", 0);

	stbi_set_flip_vertically_on_load(false); // tell stb_image.h to flip loaded texture's on the y-axis.

	// Load sphere
	OBJ sphere;
	sphere.load(1);

	// load 15 suzies
	int counter = 0;
	OBJ* suzy_list[15];
	for (int i = 0; i < 15; i++)
	{
		float xPos = -96 + (i % 5) * 48;
		float zPos = 113 - (i / 5) * 113;
		OBJ* obj = new OBJ(glm::vec3(xPos, 0.0f, zPos));
		obj->load(0);
		obj->id = i;
		obj->suzy_list = suzy_list;
		suzy_list[i] = obj;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_FALSE);
	int state = 1;
	glfwSetTime(0.0);
	for (int i = 0; i < 15; i += 1)
	{
		suzy_list[i]->start();
	}
	double timerStop = 90.0; // 1 min and 50 seconds (40 seconds required to reach surface)

	// Render loop
	do {
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//////// DRAWING FOOTBALL FIELD /////////////////////////////////////
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// texture coord attribute
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glBindTexture(GL_TEXTURE_2D, texture1);

		// bind textures on corresponding texture units
		ourShader.use();
		ourShader.setInt("texture1", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);

		// activate shader
		ourShader.use();
		// create transformations
		glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		model = glm::translate(model, glm::vec3(0.03f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		// retrieve the matrix uniform locations
		unsigned int modelLoc = glGetUniformLocation(ourShader.ID, "model");
		unsigned int viewLoc = glGetUniformLocation(ourShader.ID, "view");
		// pass them to the shaders (3 different ways)
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
		// note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
		ourShader.setMat4("projection", projection);
		// render container
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		///////////////////////////////////////////////////////////////////////////////

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		// Draw suzy object
			for (int i = 0; i < 15; i++)
			{
				suzy_list[i]->draw(0);
			}
		// Draw sphere object
		sphere.draw(1);

		// Swap buffers
		glfwPollEvents();
		glfwSwapBuffers(window);

		// sleep for 30 msec
		std::this_thread::sleep_for(std::chrono::milliseconds(30));

		if ((glfwGetTime() - timerStop) > 0.0)
		{
			// end simulation
			break;
		}
	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);
	
	// end all threads
	for (int i = 0; i < 15; i += 1)
	{
		suzy_list[i]->stop();
	}

	// Cleanup VBO and shader
	for (int i = 0; i < 15; i++)
	{
		glDeleteBuffers(1, &suzy_list[i]->vertexbuffer);
		glDeleteBuffers(1, &suzy_list[i]->uvbuffer);
		glDeleteBuffers(1, &suzy_list[i]->normalbuffer);
		glDeleteBuffers(1, &suzy_list[i]->elementbuffer);
		glDeleteProgram(suzy_list[i]->programID);
		glDeleteTextures(1, &suzy_list[i]->texture);
		glDeleteVertexArrays(1, &suzy_list[i]->VertexArrayID);
	}

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}