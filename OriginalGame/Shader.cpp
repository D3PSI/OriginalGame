#include "Shader.hpp"

/*
*	Constructor for a shader program which compiles and links the shaders.
*	Geometry shader has a nullptr as its default argument, so you don't necessarily need one.
*/
Shader::Shader(const char *vertexPath, const char *fragmentPath, const char *geometryPath) {
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	std::ifstream gShaderFile;
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		vShaderFile.close();
		fShaderFile.close();
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
		if (geometryPath != nullptr) {
			gShaderFile.open(geometryPath);
			std::stringstream gShaderStream;
			gShaderStream << gShaderFile.rdbuf();
			gShaderFile.close();
			geometryCode = gShaderStream.str();
		}
	}
	catch (std::ifstream::failure e) {
		dev::showConsoleWindow();
		std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		dev::error("ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ");
	}
	const char* vShaderCode = vertexCode.c_str();
	const char * fShaderCode = fragmentCode.c_str();
	unsigned int vertex, fragment;
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX");
	dev::eventLog("Vertex-Shader successfully compiled");
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	checkCompileErrors(fragment, "FRAGMENT");
	dev::eventLog("Fragment-Shader successfully compiled");
	unsigned int geometry;
	if (geometryPath != nullptr) {
		const char * gShaderCode = geometryCode.c_str();
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, NULL);
		glCompileShader(geometry);
		checkCompileErrors(geometry, "GEOMETRY");
		dev::eventLog("Geometry-Shader successfully compiled");
	}
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	if (geometryPath != nullptr) {
		glAttachShader(ID, geometry);
	}
	glLinkProgram(ID);
	checkCompileErrors(ID, "PROGRAM");
	dev::eventLog("Shader-Program linked successfully");
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	if (geometryPath != nullptr) {
		glDeleteShader(geometry);
	}
}

/*
*	Utility function to check for any compile errors in the process of creating the shader program.
*	
*/
void Shader::checkCompileErrors(GLuint shader, std::string type) {
	GLint success;
	GLchar infoLog[1024];
	if (type != "PROGRAM") {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			dev::showConsoleWindow();
			std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			dev::error("ERROR::SHADER_COMPILATION_ERROR of type: " + type + "\n" + infoLog + "\n -- --------------------------------------------------- -- ");
		}
	}
	else {
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success) {
			dev::showConsoleWindow();
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			dev::error("ERROR::PROGRAM_LINKING_ERROR of type: " + type + "\n" + infoLog + "\n -- --------------------------------------------------- -- ");
		}
	}
}

/*
*	Function to actually use / bind the shader program
*
*/
void Shader::use() {
	glUseProgram(ID);
}

/*
*	Utility function to set bool uniform variables.
*
*/
void Shader::setBool(const std::string &name, bool value) const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

/*
*	Utility function to set int uniform variables.
*
*/
void Shader::setInt(const std::string &name, int value) const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

/*
*	Utility function to set float uniform variables.
*
*/
void Shader::setFloat(const std::string &name, float value) const {
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

/*
*	Utility function to set vec2 uniform variables.
*
*/
void Shader::setVec2(const std::string &name, const glm::vec2 &value) const {
	glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

/*
*	Utility function to vec2 bool uniform variables.
*
*/
void Shader::setVec2(const std::string &name, float x, float y) const {
	glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}

/*
*	Utility function to set vec3 uniform variables.
*
*/
void Shader::setVec3(const std::string &name, const glm::vec3 &value) const {
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

/*
*	Utility function to set vec3 uniform variables.
*
*/
void Shader::setVec3(const std::string &name, float x, float y, float z) const {
	glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

/*
*	Utility function to set vec4 uniform variables.
*
*/
void Shader::setVec4(const std::string &name, const glm::vec4 &value) const {
	glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

/*
*	Utility function to set vec4 uniform variables.
*
*/
void Shader::setVec4(const std::string &name, float x, float y, float z, float w) const {
	glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

/*
*	Utility function to set mat2 uniform variables.
*
*/
void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const {
	glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

/*
*	Utility function to set mat3 uniform variables.
*
*/
void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const {
	glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

/*
*	Utility function to set mat4 uniform variables.
*
*/
void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const {
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}