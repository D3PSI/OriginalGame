#include "Framebuffer.hpp"


float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	 // positions   // texCoords
   -1.0f,  1.0f,  0.0f, 1.0f,
   -1.0f, -1.0f,  0.0f, 0.0f,
	1.0f, -1.0f,  1.0f, 0.0f,

   -1.0f,  1.0f,  0.0f, 1.0f,
	1.0f, -1.0f,  1.0f, 0.0f,
	1.0f,  1.0f,  1.0f, 1.0f
};

/*
*	Constructor with arguments.
*	
*/
Framebuffer::Framebuffer(const int SCR_WIDTH, const int SCR_HEIGHT, const char *vertPath, const char *fragPath, const char *geomPath) {
	glGenFramebuffers(1, &fboMSAAID);
	bindMSAAFBO();
	createMSAATexture(SCR_WIDTH, SCR_HEIGHT);
	createMSAARBO(SCR_WIDTH, SCR_HEIGHT);
	checkFBOStatus();
	glGenFramebuffers(1, &fboID);
	bindFBO();
	createTexture(SCR_WIDTH, SCR_HEIGHT);
	checkFBOStatus();
	createScreenQuadVAO();
	createScreenShader(
		vertPath,
		fragPath,
		geomPath
	);
}

/*
*	Binds the framebuffer object.
*	
*/
void Framebuffer::bindFBO() {
	glBindFramebuffer(GL_FRAMEBUFFER, fboID);
}

/*
*	Bind the multisampled framebuffer object.
*	
*/
void Framebuffer::bindMSAAFBO() {
	glBindFramebuffer(GL_FRAMEBUFFER, fboMSAAID);
}

/*
*	Creates the texture attachment for the framebuffer/renderbuffer object.
*	
*/
void Framebuffer::createTexture(const int SCR_WIDTH, const int SCR_HEIGHT) {
	glGenTextures(1, &texID);
	bindTexture();
	glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGB, 
			SCR_WIDTH, 
			SCR_HEIGHT, 
			0, 
			GL_RGB,
			GL_UNSIGNED_BYTE,
			NULL
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
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(
			GL_FRAMEBUFFER,	
			GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D,
			texID,
			0
	);
}

/*
*	Creaets the multisampled texture attachment.
*	
*/
void Framebuffer::createMSAATexture(const int SCR_WIDTH, const int SCR_HEIGHT) {
	glGenTextures(1, &texMSAAID);
	bindTexture();
	glTexImage2DMultisample(
		GL_TEXTURE_2D_MULTISAMPLE,
		4,
		GL_RGB,
		SCR_WIDTH,
		SCR_HEIGHT,
		GL_TRUE
	);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D_MULTISAMPLE,
		texMSAAID,
		0
	);
}

/*
*	Binds the texture attachment
*	
*/
void Framebuffer::bindTexture() {
	glBindTexture(GL_TEXTURE_2D, texID);
}

/*
*	Binds the multisampled texture attachment
*
*/
void Framebuffer::bindMSAATexture() {
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texMSAAID);
}

/*
*	Creates the renderbuffer object.
*	
*/
void Framebuffer::createRBO(const int SCR_WIDTH, const int SCR_HEIGHT) {
	glGenRenderbuffers(1, &rboID);
	bindRBO();
	glRenderbufferStorage(
					GL_RENDERBUFFER,
					GL_DEPTH24_STENCIL8,
					SCR_WIDTH,
					SCR_HEIGHT
	);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(
		GL_FRAMEBUFFER,
		GL_DEPTH_STENCIL_ATTACHMENT,
		GL_RENDERBUFFER,
		rboID
	);
}

/*
*	Creates the multisampled renderbuffer object.
*	
*/
void Framebuffer::createMSAARBO(const int SCR_WIDTH, const int SCR_HEIGHT) {
	glGenRenderbuffers(1, &rboMSAAID);
	bindMSAARBO();
	glRenderbufferStorageMultisample(
							GL_RENDERBUFFER,
							4,
							GL_DEPTH24_STENCIL8,
							SCR_WIDTH,
							SCR_HEIGHT
	);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(
							GL_FRAMEBUFFER,
							GL_DEPTH_STENCIL_ATTACHMENT, 
							GL_RENDERBUFFER,
							rboMSAAID
	);
}

/*
*	Binds the renderbuffer object.
*	
*/
void Framebuffer::bindRBO() {
	glBindRenderbuffer(GL_RENDERBUFFER, rboID);
}

/*
*	Binds the multisampled renderbuffer object.
*	
*/
void Framebuffer::bindMSAARBO() {
	glBindRenderbuffer(GL_RENDERBUFFER, rboMSAAID);
}

/*
*	Checks for errors on completion of the framebuffer object's generation.
*	
*/
void Framebuffer::checkFBOStatus() {
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		dev::showConsoleWindow();
		std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete" << std::endl;
		dev::error("ERROR::FRAMEBUFFER:: Framebuffer is not complete");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/*
*	Creates a vertex array object that contains the screen quad, 
*	where we'll be rendering our texture to.
*/
void Framebuffer::createScreenQuadVAO() {
	glGenVertexArrays(1, &screenQuadVAO);
	glGenBuffers(1, &screenQuadVBO);
	bindScreenQuadVAO();
	bindScreenQuadVBO();
	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(quadVertices),
		&quadVertices,
		GL_STATIC_DRAW
	);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
					0,
					2,
					GL_FLOAT,
					GL_FALSE, 
					4 * sizeof(float),
					(void*)0
	);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
					1, 
					2,
					GL_FLOAT,
					GL_FALSE, 4 * sizeof(float),
					(void*)(2 * sizeof(float))
	);
}

/*
*	Binds the screen quad VAO.
*	
*/
void Framebuffer::bindScreenQuadVAO() {
	glBindVertexArray(screenQuadVAO);
}

/*
*	Binds the screen quad VBO.
*
*/
void Framebuffer::bindScreenQuadVBO() {
	glBindBuffer(GL_ARRAY_BUFFER, screenQuadVBO);
}

/*
*	Creates the shader program needed for rendering to 
*	the screenQuadVAO texture.
*/
void Framebuffer::createScreenShader(const char *vertPath, const char *fragPath, const char *geomPath) {
	screenShader = new Shader(vertPath, fragPath, geomPath);
	useShader();
	screenShader->setInt("screenTexture", 0);
}

/*
*	Use the screen quad shader to actually see something.
*	
*/
void Framebuffer::useShader() {
	screenShader->use();
}

/*
*	Draws the screen quad with the framebuffer texture.
*	
*/
void Framebuffer::draw() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	useShader();
	bindScreenQuadVAO();
	bindTexture();
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

/*
*	Blits the multisampled buffers to normal colorbuffer of fboID, image is stored in texID.
*	
*/
void Framebuffer::blit(const int SCR_WIDTH, const int SCR_HEIGHT) {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fboMSAAID);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboID);
	glBlitFramebuffer(
		0,
		0,
		SCR_WIDTH,
		SCR_HEIGHT,
		0,
		0,
		SCR_WIDTH,
		SCR_HEIGHT,
		GL_COLOR_BUFFER_BIT,
		GL_NEAREST
	);
}

/*
*	Destructor.
*	
*/
Framebuffer::~Framebuffer() {
	delete screenShader;
}
