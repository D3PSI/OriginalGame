#define STB_IMAGE_IMPLEMENTATION
#define _CRT_SECURE_NO_WARNINGS
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glad/glad.c>
#include "Model.hpp"
#include "Skybox.hpp"
#include "Framebuffer.hpp"
#include "Prototypes.hpp"

/*			GLOBAL VARIABLES	*/
struct Character {
	GLuint     TextureID;  // ID handle of the glyph texture
	glm::ivec2 Size;       // Size of glyph
	glm::ivec2 Bearing;    // Offset from baseline to left/top of glyph
	GLuint     Advance;    // Offset to advance to next glyph
};

std::map<GLchar, Character> Characters;
GLuint VAO, VBO;
HWND consoleWindow			= GetConsoleWindow();
GLFWwindow *window			= nullptr;
const int SCR_WIDTH			= 1280;
const int SCR_HEIGHT		= 768;
const char *TITLE			= "Aiming Simulator by D3PSI";
float lastFrame				= 0.0;
float deltaTime				= 0.0;
bool process				= true;
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
		   -2.0f,
			4.0f,
		   -1.0f
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
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
			process = false;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE) {
			process = true;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		if(process) {
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
		if (process) {
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
	}

	/*
	*	Executes when scroll wheel is activated.
	*
	*/
	void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
		if(process)
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
		glTexParameteri(
				GL_TEXTURE_CUBE_MAP,
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
	*	renderQuad() renders a 1x1 XY quad in NDC.
	*	
	*/
	unsigned int quadVAO = 0;
	unsigned int quadVBO;
	void renderQuad() {
		if (quadVAO == 0) {
			float quadVertices[] = {
				// positions        // texture Coords
			   -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			   -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
				1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			};

			// setup quad VAO
			glGenVertexArrays(1, &quadVAO);
			glGenBuffers(1, &quadVBO);
			glBindVertexArray(quadVAO);
			glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		}
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}

	/*
	*	 renderCube() renders a 1x1 3D cube in NDC.
	*
	*/
	unsigned int cubeVAO = 0;
	unsigned int cubeVBO = 0;
	void renderCube() {
		// initialize (if necessary)
		if (cubeVAO == 0) {
			float vertices[] = {
				// back face
			   -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
				1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
				1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
				1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			   -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			   -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left

				// front face
			   -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
				1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
				1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
				1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			   -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
			   -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left

				// left face
			   -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			   -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
			   -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			   -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			   -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			   -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right

				// right face
				1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
				1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			    1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
				1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
				1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
				1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     

				// bottom face
			   -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
				1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
				1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
				1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			   -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			   -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right

				// top face
			   -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
				1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
				1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
				1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			   -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			   -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
			};

			glGenVertexArrays(1, &cubeVAO);
			glGenBuffers(1, &cubeVBO);

			// fill buffer
			glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			// link vertex attributes
			glBindVertexArray(cubeVAO);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}
		// render Cube
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}

	/*
	*	Renders the entire scene.
	*	
	*/
	void renderScene(const Shader &shader) {
		float planeVertices[] = {
			// positions            // normals         // texcoords
			25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
		   -25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
		   -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

			25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
		   -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
			25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
		};

		// plane VAO
		unsigned int planeVBO, planeVAO;
		glGenVertexArrays(1, &planeVAO);
		glGenBuffers(1, &planeVBO);
		glBindVertexArray(planeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindVertexArray(0);
		glm::mat4 model;
		shader.setMat4("model", model);
		glBindVertexArray(planeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// cubes
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
		model = glm::scale(model, glm::vec3(0.5f));
		shader.setMat4("model", model);
		renderCube();
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
		model = glm::scale(model, glm::vec3(0.5f));
		shader.setMat4("model", model);
		renderCube();
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
		model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
		model = glm::scale(model, glm::vec3(0.25));
		shader.setMat4("model", model);
		renderCube();
		glm::mat4 newModel;
		newModel = glm::translate(newModel, glm::vec3(
											0.0f, 
											0.0f,
											0.0f
										));
		shader.setMat4("model", newModel);
	}

	/*
	*	Renders a string of text to the screen.
	*	
	*/
	void RenderText(Shader &s, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color) {
		// Activate corresponding render state	
		s.use();
		glUniform3f(glGetUniformLocation(s.ID, "textColor"), color.x, color.y, color.z);
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(VAO);

		// Iterate through all characters
		std::string::const_iterator c;
		for (c = text.begin(); c != text.end(); c++) {
			Character ch = Characters[*c];

			GLfloat xpos = x + ch.Bearing.x * scale;
			GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

			GLfloat w = ch.Size.x * scale;
			GLfloat h = ch.Size.y * scale;
			// Update VBO for each character
			GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },

			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }
			};

			// Render glyph texture over quad
			glBindTexture(GL_TEXTURE_2D, ch.TextureID);

			// Update content of VBO memory
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			// Render quad
			glDrawArrays(GL_TRIANGLES, 0, 6);

			// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
			x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
		}
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
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

	/*			BUFFERS				*/
	Framebuffer framebuffer(
					SCR_WIDTH, 
					SCR_HEIGHT,
					"src/shaders/screenShader.vert", 
					"src/shaders/screenShader.frag"
	);

	// shadow map / depth map
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	const int SHADOW_WIDTH = 1024;
	const int SHADOW_HEIGHT = 1024;
	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_DEPTH_COMPONENT,
		SHADOW_WIDTH,
		SHADOW_HEIGHT,
		0,
		GL_DEPTH_COMPONENT,
		GL_FLOAT,
		NULL
	);
	glTexParameteri(
		GL_TEXTURE_2D, 
		GL_TEXTURE_MIN_FILTER, 
		GL_NEAREST
	);
	glTexParameteri(
		GL_TEXTURE_2D,
		GL_TEXTURE_MAG_FILTER,
		GL_NEAREST
	);
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
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER, 
		GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, 
		depthMap,
		0
	);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/*			SHADERS				*/
	Shader objectShader("src/shaders/objectShader.vert", "src/shaders/objectShader.frag");
	Shader simpleDepthShader("src/shaders/simpleDepthShader.vert", "src/shaders/simpleDepthShader.frag");
	Shader debugDepthQuad("src/shaders/debugDepthQuad.vert", "src/shaders/debugDepthQuad.frag");
	Shader glyphShader("src/shaders/glyph.vert", "src/shaders/glyph.frag");

	objectShader.use();
	objectShader.setInt("diffuseTexture", 0);
	objectShader.setInt("shadowMap", 1);

	debugDepthQuad.use();
	debugDepthQuad.setInt("depthMap", 0);
	glm::mat4 glyphProjection = glm::ortho(
		0.0f,
		static_cast<GLfloat>(SCR_WIDTH),
		0.0f,
		static_cast<GLfloat>(SCR_HEIGHT)
	);
	glyphShader.use();
	glyphShader.setMat4("projection", glyphProjection);

	/*			FONTS				*/
	FT_Library ft;
	if (FT_Init_FreeType(&ft)) {
		dev::showConsoleWindow();
		std::cerr << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		dev::error("ERROR::FREETYPE: Could not init FreeType Library");
	}

	FT_Face face;
	if (FT_New_Face(
			ft,
			"res/fonts/arial.ttf",
			0,
			&face
		)) {
		dev::showConsoleWindow();
		std::cerr << "ERROR::FREETYPE: Failed to load font" << std::endl;
		dev::error("ERROR::FREETYPE: Failed to load font");
	}
	FT_Set_Pixel_Sizes(
		face,
		0,
		48
	);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	for (GLubyte c = 0; c < 128; c++) {
		// Load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			dev::showConsoleWindow();
			std::cerr << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			dev::error("ERROR::FREETYTPE: Failed to load Glyph");
			continue;
		}

		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);

		// Set texture options
		glTexParameteri(
			GL_TEXTURE_2D,
			GL_TEXTURE_WRAP_S,
			GL_CLAMP_TO_EDGE
		);
		glTexParameteri(
			GL_TEXTURE_2D, 
			GL_TEXTURE_WRAP_T,
			GL_CLAMP_TO_EDGE
		);
		glTexParameteri(
			GL_TEXTURE_2D,
			GL_TEXTURE_MIN_FILTER,
			GL_LINEAR
		);
		glTexParameteri(
			GL_TEXTURE_2D, 
			GL_TEXTURE_MAG_FILTER, 
			GL_LINEAR
		);

		// Now store character for later use
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<GLchar, Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	/*			MODELS				*/
	Model target("res/models/nanosuit/nanosuit.obj");

	/*			SKYBOX				*/
	Skybox skybox("res/skybox/");

	unsigned int woodTexture = dev::loadTexture("res/textures/wood.png");

	/*			OPENGL SETTINGS		*/
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	/*			GAME LOOP			*/	
	while (!glfwWindowShouldClose(window)) {
		// Per-frame time logic
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		float fps = 1 / deltaTime;
		//std::cout << fps << std::endl;
		
		dev::processInput(window);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glClearColor(
			0.2f,
			0.3f,
			0.3f,
			1.0f
		);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 1. render pass
		float near_plane = 1.0f, far_plane = 7.5f;
		glm::mat4 lightProjection = glm::ortho(
										   -10.0f, 
											10.0f,
										   -10.0f, 
											10.0f,
											near_plane,
											far_plane
										);

		glm::mat4 lightView = glm::lookAt(
									lightPos, 
									glm::vec3(0.0f),
									glm::vec3(
										0.0,
										1.0,
										0.0
									)
								);

		glm::mat4 lightSpaceMatrix = lightProjection * lightView;
		simpleDepthShader.use();
		simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		glViewport(
			0,
			0,
			SHADOW_WIDTH,
			SHADOW_HEIGHT
		);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, woodTexture);
		dev::renderScene(simpleDepthShader);
		target.draw(simpleDepthShader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(
			0,
			0,
			SCR_WIDTH,
			SCR_HEIGHT
		);
		framebuffer.bindMSAAFBO();
		glClearColor(
			0.1f,
			0.1f,
			0.1f,
			1.0f
		);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		objectShader.use();
		glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 0.1f, 100.0f);
		glm::mat4 view = camera->GetViewMatrix();
		objectShader.setMat4("projection", projection);
		objectShader.setMat4("view", view);

		// set light uniforms
		objectShader.setVec3("viewPos", camera->Position);
		objectShader.setVec3("lightPos", lightPos);
		objectShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, woodTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		dev::renderScene(objectShader);
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(
											0.0f,
											0.0f, 
											0.0f
										));
		objectShader.setMat4("model", model);
		target.draw(objectShader);
		glDepthFunc(GL_LEQUAL);
		skybox.useShader();
		skybox.setUniforms(
			camera,
			SCR_WIDTH,
			SCR_HEIGHT
		);
		skybox.bindVAO();
		skybox.bindTexture();
		skybox.draw();
		glBindVertexArray(0);

		framebuffer.blit(SCR_WIDTH, SCR_HEIGHT);
		framebuffer.draw();

		debugDepthQuad.use();
		debugDepthQuad.setFloat("near_plane", near_plane);
		debugDepthQuad.setFloat("far_plane", far_plane);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		//renderQuad();

		glfwPollEvents();
		dev::RenderText(
			glyphShader,
			std::to_string(fps),
			25.0f,
			25.0f, 
			1.0f,
			glm::vec3(
				0.5,
				0.8f, 
				0.2f
			)
		);
		glfwSwapBuffers(window);
	}
	delete camera;

	// shut everything down
	glfwTerminate();
	dev::stopEventLog();
	dev::stopLog();

	return 0;
}