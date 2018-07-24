#include "Mesh.hpp"

/*
*	Constructor.
*	
*/
Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) {
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;
	setupMesh();
}

/*
*	Renders the mesh.
*	
*/
void Mesh::draw(Shader shader) {
	unsigned int diffuseNr		= 1;
	unsigned int specularNr		= 1;
	unsigned int normalNr		= 1;
	unsigned int heightNr		= 1;
	for (unsigned int i = 0; i < textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		std::string number;
		std::string name = textures[i].type;
		if (name == "material.texture_diffuse") {
			number = std::to_string(diffuseNr++);
		}
		else if (name == "material.texture_specular") {
			number = std::to_string(specularNr++);
		}
		else if (name == "material.texture_normal") {
			number = std::to_string(normalNr++);
		}
		else if (name == "material.texture_height") {
			number = std::to_string(heightNr++);
		}
		glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
		glBindTexture(GL_TEXTURE_2D, textures[i].ID);
	}
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
}

/*
*	Initializes all the buffer objects and arrays.
*	
*/
void Mesh::setupMesh() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(
		GL_ARRAY_BUFFER, 
		vertices.size() * sizeof(Vertex), 
		&vertices[0],
		GL_STATIC_DRAW
	);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER, 
		indices.size() * sizeof(unsigned int), 
		&indices[0], 
		GL_STATIC_DRAW
	);
	
	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0, 
		3, 
		GL_FLOAT, 
		GL_FALSE, 
		sizeof(Vertex), 
		(void*)0
	);
	
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1, 
		3, 
		GL_FLOAT, 
		GL_FALSE, 
		sizeof(Vertex), 
		(void*)offsetof(Vertex, normal)
	);

	// vertex texture coordinates
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2, 
		3, 
		GL_FLOAT, 
		GL_FALSE, 
		sizeof(Vertex), 
		(void*)offsetof(Vertex, texCoords)
	);

	// vertex tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(
		3, 
		3, 
		GL_FLOAT,
		GL_FALSE, 
		sizeof(Vertex), 
		(void*)offsetof(Vertex, tangent)
	);

	// vertex bitangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(
		4, 
		3, 
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(void*)offsetof(Vertex, bitangent)
	);
	
	glBindVertexArray(0);
}

/*
*	Destructor.
*	
*/
Mesh::~Mesh() {

}
