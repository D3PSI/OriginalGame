#include "Skybox.hpp"

float skyboxVertices[] = {
	// positions           
   -1.0f,  1.0f, -1.0f,
   -1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f,  1.0f, -1.0f,
   -1.0f,  1.0f, -1.0f,

   -1.0f, -1.0f,  1.0f,
   -1.0f, -1.0f, -1.0f,
   -1.0f,  1.0f, -1.0f,
   -1.0f,  1.0f, -1.0f,
   -1.0f,  1.0f,  1.0f,
   -1.0f, -1.0f,  1.0f,

	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,

   -1.0f, -1.0f,  1.0f,
   -1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f, -1.0f,  1.0f,
   -1.0f, -1.0f,  1.0f,

   -1.0f,  1.0f, -1.0f,
	1.0f,  1.0f, -1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
   -1.0f,  1.0f,  1.0f,
   -1.0f,  1.0f, -1.0f,

   -1.0f, -1.0f, -1.0f,
   -1.0f, -1.0f,  1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
   -1.0f, -1.0f,  1.0f,
	1.0f, -1.0f,  1.0f
};

/*
*	Constructor.
*	
*/
Skybox::Skybox(const std::string &path) {
	skyboxShader = new Shader("src/shaders/skyboxShader.vert", "src/shaders/skyboxShader.frag");
	setUp();
	setBuffers();
	directory = getDirectory(path);
	faces = {
		directory + "/right.jpg",
		directory + "/left.jpg",
		directory + "/top.jpg",
		directory + "/bottom.jpg",
		directory + "/front.jpg",
		directory + "/back.jpg",
	};
	skyboxTexture = dev::loadCubemap(faces);
}

/*
*	Uses the skybox shader program.
*	
*/
void Skybox::useShader() {
	skyboxShader->use();
}

/*
*	Sets uniforms, that only need to be set once, for the skybox shader.
*	
*/
void Skybox::setUp() {
	useShader();
	skyboxShader->setInt("skybox", 0);
}

void Skybox::setUniforms(Camera *camera, const int SCR_WIDTH, const int SCR_HEIGHT) {
	useShader();
	glm::mat4 view, projection;
	view = glm::mat4(glm::mat3(camera->GetViewMatrix()));
	projection = glm::perspective(
							glm::radians(camera->Zoom),
							static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT),
							0.1f,
							100.0f
						);
	skyboxShader->setMat4("view", view);
	skyboxShader->setMat4("projection", projection);
}

/*
*	Retrieves the directory of the different skybox faces.
*	
*/
std::string Skybox::getDirectory(const std::string &path) {
	return path.substr(0, path.find_last_of('/'));
}

/*
*	Set up all the relevant buffers.
*	
*/
void Skybox::setBuffers() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	bindVAO();
	bindVBO();
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

/*
*	Binds the skybox' VAO.
*	
*/
void Skybox::bindVAO() {
	glBindVertexArray(VAO);
}

/*
*	Binds the skybox' VBO.
*	
*/
void Skybox::bindVBO() {
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
}

/*
*	Binds the beforehand loaded skybox texture.
*	
*/
void Skybox::bindTexture() {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
}

/*
*	Renders the skybox.
*	
*/
void Skybox::draw() {
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

/*
*	Destructor.
*	
*/
Skybox::~Skybox() {
	delete skyboxShader;
}
