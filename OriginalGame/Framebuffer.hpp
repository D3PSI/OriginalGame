#pragma once
#include <glad/glad.h>
#include <iostream>
#include <string>
#include "Shader.hpp"

namespace dev {
	void showConsoleWindow(void);
	void error(const std::string errorMsg);
}

class Framebuffer
{
public:
	Framebuffer(const int SCR_WIDTH, const int SCR_HEIGHT, const char *vertPath, const char *fragPath, const char *geomPath = nullptr);
	void bindFBO(void);
	void bindTexture(void);
	void bindRBO(void);
	void bindScreenQuadVAO(void);
	void bindScreenQuadVBO(void);
	void useShader(void);
	void draw();
	~Framebuffer();
private:
	unsigned int fboID, texID, rboID, screenQuadVAO, screenQuadVBO;
	Shader *screenShader;
	void createFBO(const int SCR_WIDTH, const int SCR_HEIGHT, const char *vertPath, const char *fragPath, const char *geomPath = nullptr);
	void createTexture(const int SCR_WIDTH, const int SCR_HEIGHT);
	void createRBO(const int SCR_WIDTH, const int SCR_HEIGHT);
	void createScreenQuadVAO(void);
	void createScreenShader(const char *vertPath, const char *fragPath, const char *geomPath = nullptr);
	void checkFBOStatus(void);
};

