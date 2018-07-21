#pragma once
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
#include <thread>
#include "Shader.hpp"

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
	void showConsoleWindow(void);
	void hideConsoleWindow(void); 
	void renderTriangle(void);
	const std::string currentDateTime(void);
}