#pragma once
#include <glad/glad.h>
#include <vector>
#include <string>
#include "Shader.hpp"

namespace dev {
	unsigned int loadCubemap(std::vector<std::string> faces);
}

class Skybox
{
public:
	Skybox(const std::string &path);
	void useShader(void);
	void setUniforms(Camera *camera, const int SCR_WIDTH, const int SCR_HEIGHT);
	void bindVAO(void);
	void bindVBO(void);
	void bindTexture(void);
	void draw(void);
	~Skybox();
private:
	unsigned int VAO, VBO;
	unsigned int skyboxTexture;
	std::string directory;
	Shader *skyboxShader;
	std::vector<std::string> faces;
	void setUp(void);
	void setBuffers(void);
	std::string getDirectory(const std::string &path);
};

