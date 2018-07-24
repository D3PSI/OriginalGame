#define STB_IMAGE_IMPLEMENTATION
#define _CRT_SECURE_NO_WARNINGS
#include "Model.hpp"
#include "Skybox.hpp"
#include "Prototypes.hpp"

/*			GLOBAL VARIABLES	*/
HWND consoleWindow			= GetConsoleWindow();
GLFWwindow *window			= nullptr;
const int SCR_WIDTH			= 1280;
const int SCR_HEIGHT		= 768;
const char *TITLE			= "Aiming Simulator by D3PSI";
float lastFrame				= 0.0;
float deltaTime				= 0.0;
Camera *camera				= new Camera(glm::vec3(
												0.0f,
												0.0f,
												3.0f
											));
glm::vec3 cameraPos			= glm::vec3(
									0.0f,
									0.0f, 
									3.0f
								);
glm::vec3 cameraFront		= glm::vec3(
									0.0f,
									0.0f,
								   -1.0f
								);
glm::vec3 cameraUp			= glm::vec3(
									0.0f,
									1.0f,
									0.0f
								);
bool firstMouse				= true;
float yaw					= -90.0f; 
float lastX					= SCR_WIDTH / 2.0f;
float lastY					= SCR_HEIGHT / 2.0f;
glm::vec3 lightPos(
			-0.7f,
			-0.2f,
			-2.0f
		);

/*
*	Own namespace to prevent any stupid conflicts.
*	
*/
namespace dev {
	std::ofstream errorLogFile;
	std::ofstream startLogFile;
	std::ofstream eventLogFile;

	/*
	*	Processes keyboard input from user.
	*
	*/
	void processInput(GLFWwindow *window) {
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera->ProcessKeyboard(FORWARD, deltaTime);

		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera->ProcessKeyboard(BACKWARD, deltaTime);

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera->ProcessKeyboard(LEFT, deltaTime);

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera->ProcessKeyboard(RIGHT, deltaTime);
	}

	/*
	*	Executes when window is being resized and adjusts viewport.
	*
	*/
	void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
		glViewport(0, 0, width, height);
	}

	/*
	*	Executes when mouse is clicked.
	*
	*/
	void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
		if (firstMouse) {
			lastX = static_cast<float>(xpos);
			lastY = static_cast<float>(ypos);
			firstMouse = false;
		}

		float xoffset = static_cast<float>(xpos) - lastX;
		float yoffset = lastY - static_cast<float>(ypos); // reversed since y-coordinates go from bottom to top

		lastX = static_cast<float>(xpos);
		lastY = static_cast<float>(ypos);

		camera->ProcessMouseMovement(xoffset, yoffset);
	}

	/*
	*	Executes when scroll wheel is activated.
	*
	*/
	void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
		camera->ProcessMouseScroll((float)yoffset);
	}

	/*
	*	Print an error message to "logs/errorLog.log".
	*
	*/
	void error(const std::string errorMsg) {
		errorLogFile.open("logs/errorLog.log", std::ios::app);
		errorLogFile << currentDateTime() << ":		" << errorMsg << std::endl;
		errorLogFile.close();
		eventLog("========EXCEPTION THROWN:  CHECK ERROR LOG FOR DETAILS========");
	}

	/*
	*	Print an log message to "logs/starts.log" whenever the game is started.
	*
	*/
	void startLog() {
		startLogFile.open("logs/starts.log", std::ios::app);
		startLogFile << currentDateTime() << ":    Startup successful" << std::endl;
		startLogFile.close();
	}

	/*
	*	Print an log message to "logs/starts.log" whenever the game is stopped.
	*
	*/
	void stopLog() {
		startLogFile.open("logs/starts.log", std::ios::app);
		startLogFile << currentDateTime() << ":    Shutdown with code 0" << std::endl;
		startLogFile.close();
	}

	/*
	*	Print an event message to "logs/events.log".
	*
	*/
	void eventLog(std::string eventMsg) {
		eventLogFile.open("logs/events.log", std::ios::app);
		eventLogFile << currentDateTime() << ":    " << eventMsg << std::endl;
		eventLogFile.close();
	}

	/*
	*	Retrieve current date and Time in this format: DD-MM-YYYY HH:mm:ss.
	*
	*/
	const std::string currentDateTime() {
		time_t now = time(0);
		struct tm tstruct;
		char buf[80];
		tstruct = *localtime(&now);
		strftime(
			buf,
			sizeof(buf),
			"%d-%m-%Y %X",
			&tstruct
		);
		return buf;
	}

	/*
	*	Prints program-started message to "logs/events.log".
	*	
	*/
	void startEventLog() {
		eventLogFile.open("logs/events.log", std::ios::app);
		eventLogFile << "----------------------" << currentDateTime() << "----" << "PROCESS EXECUTION STARTED" << "----------------------" << std::endl;
		eventLogFile.close();
	}

	/*
	*	Prints program-stopped message to "logs/events.log".
	*	
	*/
	void stopEventLog() {
		eventLogFile.open("logs/events.log", std::ios::app);
		eventLogFile << "----------------------" << currentDateTime() << "----" << "PROCESS EXECUTION TERMINATED" << "----------------------" << std::endl;
		eventLogFile.close();
	}

	/*
	*	Shows hidden console window.
	*	
	*/
	void showConsoleWindow() {
		ShowWindow(consoleWindow, SW_SHOW);
		eventLog("Console-Window successfully shown");
	}

	/*
	*	Hides visible console window.
	*	
	*/
	void hideConsoleWindow() {
		ShowWindow(consoleWindow, SW_HIDE);
		eventLog("Console-Window successfully hidden");
	}

	/*
	*	Utility function for loading a texture from a file.
	*
	*/
	unsigned int loadTexture(char const *path) {
		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, nrComponents;
		unsigned char *data = stbi_load(
									path, 
									&width, 
									&height,
									&nrComponents,
									0
								);
		if (data) {
			GLenum format;
			if (nrComponents == 1) {
				format = GL_RED;
			}
			else if (nrComponents == 3) {
				format = GL_RGB;
			}
			else if (nrComponents == 4) {
				format = GL_RGBA;
			}
			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(
				GL_TEXTURE_2D, 
				0, 
				format, 
				width, 
				height, 
				0, 
				format, 
				GL_UNSIGNED_BYTE, 
				data
			);
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(
				GL_TEXTURE_2D,
				GL_TEXTURE_WRAP_S, 
				GL_REPEAT
			);
			glTexParameteri(
				GL_TEXTURE_2D,
				GL_TEXTURE_WRAP_T,
				GL_REPEAT
			);
			glTexParameteri(
				GL_TEXTURE_2D, 
				GL_TEXTURE_MIN_FILTER,
				GL_LINEAR_MIPMAP_LINEAR
			);
			glTexParameteri(
				GL_TEXTURE_2D,
				GL_TEXTURE_MAG_FILTER,
				GL_LINEAR
			);
			stbi_image_free(data);
		}
		else {
			showConsoleWindow();
			std::cerr << "Texture failed to load at path: " << path << std::endl;
			error("Texture failed to load at path: " + *path);
			stbi_image_free(data);
		}

		return textureID;
	}

	/*
	*	Utility function for loading a texture from a file.
	*
	*/
	unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma) {
		std::string filename = std::string(path);
		filename = directory + '/' + filename;
		unsigned int textureID;
		glGenTextures(1, &textureID);
		int width, height, nrComponents;
		unsigned char *data = stbi_load(
									filename.c_str(),
									&width,
									&height,
									&nrComponents,
									0
								);
		if (data) {
			GLenum format;
			if (nrComponents == 1) {
				format = GL_RED;
			}
			else if (nrComponents == 3) {
				format = GL_RGB;
			}
			else if (nrComponents == 4) {
				format = GL_RGBA;
			}
			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(
				GL_TEXTURE_2D,
				0, 
				format,
				width,
				height,
				0, 
				format,
				GL_UNSIGNED_BYTE,
				data
			);
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(
				GL_TEXTURE_2D,
				GL_TEXTURE_WRAP_S,
				GL_REPEAT
			);
			glTexParameteri(
				GL_TEXTURE_2D, 
				GL_TEXTURE_WRAP_T, 
				GL_REPEAT
			);
			glTexParameteri(
				GL_TEXTURE_2D,
				GL_TEXTURE_MIN_FILTER,
				GL_LINEAR_MIPMAP_LINEAR
			);
			glTexParameteri(
				GL_TEXTURE_2D, 
				GL_TEXTURE_MAG_FILTER,
				GL_LINEAR
			);
			stbi_image_free(data);
		}
		else {
			showConsoleWindow();
			std::cerr << "Texture failed to load at path: " << path << std::endl;
			error("Texture failed to load at path: " + *path);
			stbi_image_free(data);
		}
		return textureID;
	}

	/*
	*	Loads a cubemap texture from file.
	*	
	*/
	unsigned int loadCubemap(std::vector<std::string> faces) {
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		int width, height, nrChannels;
		for (unsigned int i = 0; i < faces.size(); i++) {
			unsigned char *data = stbi_load(
										faces[i].c_str(),
										&width,
										&height,
										&nrChannels,
										0
									);
			if (data) {
				glTexImage2D(
					GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0, 
					GL_RGB, 
					width,
					height,
					0,
					GL_RGB, 
					GL_UNSIGNED_BYTE, 
					data
				);
				stbi_image_free(data);
			}
			else {
				showConsoleWindow();
				std::cerr << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
				error("Cubemap texture failed to load at path:	" + faces[i] + "\n");
				stbi_image_free(data);
			}
		}
		glTexParameteri(
			GL_TEXTURE_CUBE_MAP,
			GL_TEXTURE_MIN_FILTER,
			GL_LINEAR
		);
		glTexParameteri(
			GL_TEXTURE_CUBE_MAP,
			GL_TEXTURE_MAG_FILTER, 
			GL_LINEAR
		);
		glTexParameteri
		(GL_TEXTURE_CUBE_MAP,
			GL_TEXTURE_WRAP_S,
			GL_CLAMP_TO_EDGE
		);
		glTexParameteri(
			GL_TEXTURE_CUBE_MAP,
			GL_TEXTURE_WRAP_T,
			GL_CLAMP_TO_EDGE
		);
		glTexParameteri(
			GL_TEXTURE_CUBE_MAP,
			GL_TEXTURE_WRAP_R,
			GL_CLAMP_TO_EDGE
		);
		return textureID;
	}

	/*
	*	Handles main initialization of GLFW and OpenGL.
	*
	*/
	int init() {
		GLFWimage icon[1];

		// initialize GLFW
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		window = glfwCreateWindow(
							SCR_WIDTH, 
							SCR_HEIGHT,
							TITLE, NULL,
							NULL
						);
		if (!window) {
			showConsoleWindow();
			std::cerr << "ERROR::GLFW_WINDOW_CREATION::FAILED" << std::endl;
			error("ERROR::GLFW_WINDOW_CREATION::FAILED");
			glfwTerminate();
			return -1;
		}

		// create icon
		icon[0].pixels = stbi_load(
							"res/icon/icon.jpg",
							&icon[0].width,
							&icon[0].height,
							0, 
							4
						);
		glfwSetWindowIcon(window, 1, icon);
		stbi_image_free(icon[0].pixels);

		// set current glfwContext
		glfwMakeContextCurrent(window);

		// set callback functions
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
		glfwSetCursorPosCallback(window, mouse_callback);
		glfwSetScrollCallback(window, scroll_callback);

		// capture cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			showConsoleWindow();
			std::cerr << "ERROR::GLAD_INITIALIZATION::FAILED" << std::endl;
			error("ERROR::GLAD_INITIALIZATION::FAILED");
			return -1;
		}
		else {
			return -1;
		}
	}
}

/*
*	Defines the entry point for the application.
*	Includes the main game loop.
*	Runs main-thread.
*/
int main() {
	// initialize everything
	dev::startEventLog();
	dev::eventLog("Started execution of main-thread");
	dev::startLog();
	//dev::hideConsoleWindow();
	dev::init();
	dev::eventLog("Engine successfully initialized");

	/*			SHADERS				*/
	Shader objectShader("src/shaders/objectShader.vert", "src/shaders/objectShader.frag");

	/*			MODELS				*/
	Model target("res/models/nanosuit/nanosuit.obj");

	/*			SKYBOX				*/
	Skybox skybox("res/skybox/");

	/*			OPENGL SETTINGS		*/
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	/*			GAME LOOP			*/	
	while (!glfwWindowShouldClose(window)) {
		// Per-frame time logic
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		//double fps = 1 / deltaTime;
		//std::cout << fps << std::endl;
		
		dev::processInput(window);
		glClearColor(
			0.2f,
			0.3f,
			0.3f,
			1.0f
		);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		objectShader.use();

		// set uniforms
		objectShader.setVec3("viewPos", camera->Position);

		glm::vec3 lightColor = glm::vec3(
			1.0f,
			1.0f,
			1.0f
		);
		glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);

		objectShader.setVec3("light.position", lightPos); 
		objectShader.setVec3("light.direction", lightPos);

		objectShader.setVec3("light.ambient", ambientColor);
		objectShader.setVec3("light.diffuse", diffuseColor);
		objectShader.setVec3(
			"light.specular",
			1.0f,
			1.0f,
			1.0f
		);
		objectShader.setFloat("material.shininess", 32.0f);

		objectShader.setFloat("light.constant", 1.0f);
		objectShader.setFloat("light.linear", 1.0f);
		objectShader.setFloat("light.quadratic", 1.0f);

		// create view matrix
		glm::mat4 view;
		view = camera->GetViewMatrix();
		objectShader.setMat4("view", view);

		// create projection matrix
		glm::mat4 projection;
		projection = glm::perspective(
								glm::radians(camera->Zoom),
								static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 
								0.1f,
								100.0f
							);
		objectShader.setMat4("projection", projection);
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(
			0.0f,
		   -1.75f,
			0.0f
		));
		model = glm::scale(model, glm::vec3(
			0.2f, 
			0.2f,
			0.2f
		));  
		objectShader.setMat4("model", model);
		target.draw(objectShader);

		glDepthFunc(GL_LEQUAL);
		skybox.useShader();
		skybox.setUniforms(camera, SCR_WIDTH, SCR_HEIGHT);
		skybox.bindVAO();
		skybox.bindTexture();
		skybox.draw();
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);

		glfwPollEvents();
		glfwSwapBuffers(window);
	}
	delete camera;

	// shut everything down
	glfwTerminate();
	dev::stopEventLog();
	dev::stopLog();

	return 0;
}