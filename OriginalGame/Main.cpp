#define STB_IMAGE_IMPLEMENTATION
#define _CRT_SECURE_NO_WARNINGS
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stbimage/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <time.h>
#include <Windows.h>

/*			PROTOTYPES			*/
namespace dev {
	int init(void);
	void processInput(GLFWwindow* window);
	void framebuffer_size_callback(GLFWwindow *window, int width, int height);
	void mouse_callback(GLFWwindow *window, double xpos, double ypos);
	void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
	void error(const std::string errorMsg);
	void startLog(void);
	void stopLog(void);
	void eventLog(const std::string eventMsg);
	void startEventLog(void);
	void stopEventLog(void);
	const std::string currentDateTime(void);
}

/*			GLOBAL VARIABLES	*/
HWND consoleWindow = GetConsoleWindow();
GLFWwindow *window = nullptr;
const int SCR_WIDTH = 1280;
const int SCR_HEIGHT = 768;
const char *TITLE = "Original Game by D3PSI";
std::ofstream errorLogFile;
std::ofstream startLogFile;
std::ofstream eventLogFile;
double lastFrame = 0.0, deltaTime = 0.0;

/*
*	Own namespace to prevent any stupid conflicts.
*	
*/
namespace dev {
	/*
	*	Processes keyboard input from user.
	*
	*/
	void processInput(GLFWwindow *window) {
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {	// exit when ESCAPE is pressed
			glfwSetWindowShouldClose(window, true);
			stopEventLog();
			stopLog();
		}
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

	}

	/*
	*	Executes when scroll wheel is activated.
	*
	*/
	void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {

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
		strftime(buf, sizeof(buf), "%d-%m-%Y %X", &tstruct);
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
		window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, TITLE, NULL, NULL);
		if (!window) {
			showConsoleWindow();
			std::cerr << "ERROR::GLFW_WINDOW_CREATION::FAILED" << std::endl;
			error("ERROR::GLFW_WINDOW_CREATION::FAILED");
			glfwTerminate();
			return -1;
		}

		// create icon
		icon[0].pixels = stbi_load("res/icon/icon.jpg", &icon[0].width,
			&icon[0].height, 0, 4);
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
	dev::hideConsoleWindow();
	dev::init();
	dev::eventLog("Engine successfully initialized");

	/*			GAME LOOP			*/	
	while (!glfwWindowShouldClose(window)) {
		/*
		*	// Per-frame time logic
		*	double currentFrame = glfwGetTime();
		*	deltaTime = currentFrame - lastFrame;
		*	lastFrame = currentFrame;
		*	double fps = 1 / deltaTime;
		*	std::cout << fps << std::endl;
		*/
		dev::processInput(window);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	// shut everything down
	glfwTerminate();
	dev::stopEventLog();
	dev::stopLog();

	return 0;
}