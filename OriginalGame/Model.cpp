#include "Model.hpp"


/*
*	Constructor, expects filepath to a 3D-model.
*	
*/
Model::Model(std::string const &path, bool gamma) : gammaCorrection(gamma) {
	loadModel(path);
}

/*
*	Renders the model mesh by mesh.
*	
*/
void Model::draw(Shader shader) {
	for (unsigned int i = 0; i < meshes.size(); i++) {
		meshes[i].draw(shader);
	}
}

/*
*	Loads a model with supported ASSIMP formats from the specified filepath 
*	and stores the resulting meshes in the meshes vector.
*/
void Model::loadModel(std::string const &path) {
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(path, 
		aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		dev::showConsoleWindow();
		std::string error = importer.GetErrorString();
		std::cout << "ERROR::ASSIMP::" << error << std::endl;
		dev::error("ERROR::ASSIMP::" + error);
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));
	processNode(scene->mRootNode, scene);
}

/*
*	Processes each of ASSIMP's nodes in a recursive fashion.
*	
*/
void Model::processNode(aiNode *node, const aiScene *scene) {
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}
}

/*
*	Processes each of ASSIMP's meshes in a recursive fashion.
*	
*/
Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;
		glm::vec3 vector;

		// position attribute
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.position = vector;

		// normals
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.normal = vector;

		// texture coordinates
		if (mesh->mTextureCoords[0]) {
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.texCoords = vec;
		}
		else {
			vertex.texCoords = glm::vec2(0.0f, 0.0f);
		}

		// tangent
		vector.x = mesh->mTangents[i].x;
		vector.y = mesh->mTangents[i].y;
		vector.z = mesh->mTangents[i].z;
		vertex.tangent = vector;

		// bitangent
		vector.x = mesh->mBitangents[i].x;
		vector.y = mesh->mBitangents[i].y;
		vector.z = mesh->mBitangents[i].z;
		vertex.bitangent = vector;
		
		vertices.push_back(vertex);

		for (unsigned int j = 0; j < mesh->mNumFaces; j++) {
			aiFace face = mesh->mFaces[j];
			for (unsigned int k = 0; k < face.mNumIndices; k++) {
				indices.push_back(face.mIndices[k]);
			}
		}

		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

		// diffuse maps
		std::vector<Texture> diffuseMaps = loadMaterialTextures(
			material, 
			aiTextureType_DIFFUSE, 
			"texture_diffuse"
		);
		textures.insert(
			textures.end(),
			diffuseMaps.begin(),
			diffuseMaps.end()
		);

		// specular maps
		std::vector<Texture> specularMaps = loadMaterialTextures(
			material, 
			aiTextureType_SPECULAR, 
			"texture_specular"
		);
		textures.insert(
			textures.end(), 
			specularMaps.begin(),
			specularMaps.end()
		);

		// normal maps
		std::vector<Texture> normalMaps = loadMaterialTextures(
			material, 
			aiTextureType_AMBIENT, 
			"texture_normal"
		);
		textures.insert(
			textures.end(),
			normalMaps.begin(),
			normalMaps.end()
		);

		// height maps
		std::vector<Texture> heighMaps = loadMaterialTextures(
			material, 
			aiTextureType_HEIGHT, 
			"texture_height"
		);
		textures.insert(
			textures.end(), 
			heighMaps.begin(),
			heighMaps.end()
		);
		return Mesh(
			vertices,
			indices,
			textures
		);
	}
}

/*
*	Loads up all the different textures and stores them in a Texture-vector.
*	
*/
std::vector<Texture> Model::loadMaterialTextures(aiMaterial *material, aiTextureType type, std::string typeName) {
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < material->GetTextureCount(type); i++) {
		aiString str;
		material->GetTexture(
			type, 
			i, 
			&str
		);
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++) {
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}
		if (!skip) {
			Texture texture;
			texture.ID = dev::TextureFromFile(str.C_Str(), this->directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture);
		}
		return textures;
	}
}

/*
*	Destructor.
*	
*/
Model::~Model() {

}
