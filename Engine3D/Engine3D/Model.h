#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>
#include <glm/glm.hpp>

// Assimp libraries for loading 3D files
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Shader.h"

#include <string>
#include <vector>
#include <iostream>

class Model {
public:
    // A model can be made of multiple meshes (e.g. a car has a mesh for the body, and meshes for the wheels)
    std::vector<Mesh> meshes;
    std::string directory;

    // Constructor: give it a file path (like "house.obj") and it loads it
    Model(std::string const& path) {
        loadModel(path);
    }

    // Draws all the meshes that make up this model
    void Draw(Shader& shader) {
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }

private:
    // Uses Assimp to read the file
    void loadModel(std::string const& path) {
        Assimp::Importer importer;

        // Read the file and tell Assimp to automatically turn everything into triangles and flip textures if needed
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

        // Check if something went wrong (file not found, corrupted, etc.)
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
            return;
        }

        // Save the folder path where the model is located
        directory = path.substr(0, path.find_last_of('/'));

        // Start processing the data from the root node
        processNode(scene->mRootNode, scene);
    }

    // Goes through the model's hierarchy and processes each mesh it finds
    void processNode(aiNode* node, const aiScene* scene) {
        // Process all meshes in the current node
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }
        // Then do the same for all its children nodes
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene);
        }
    }

    // Translates Assimp's data format into our own custom Mesh data format
    Mesh processMesh(aiMesh* mesh, const aiScene* scene) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        // 1. Get all the Vertices (Points in 3D space)
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex;
            glm::vec3 vector;

            // X, Y, Z coordinates
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;

            // Normals (Which way the point is facing)
            if (mesh->HasNormals()) {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.Normal = vector;
            }

            // Texture Coordinates (Where the image goes on the point)
            if (mesh->mTextureCoords[0]) {
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
            }
            else {
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            }

            vertices.push_back(vertex);
        }

        // 2. Get all the Indices (Connect-the-dots instructions for triangles)
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        // Return a new Mesh object built with the data we just extracted
        return Mesh(vertices, indices, textures);
    }
};
#endif