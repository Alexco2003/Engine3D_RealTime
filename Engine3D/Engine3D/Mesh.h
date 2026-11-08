#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h" // We need our shader to draw the mesh

#include <string>
#include <vector>

// A structure to hold all data for a single point (vertex) in 3D space
struct Vertex {
    glm::vec3 Position;  // Where is this point? (X, Y, Z)
    glm::vec3 Normal;    // Which way is it facing?
    glm::vec2 TexCoords; // How does an image (texture) map to this point? (U, V)
};

// A structure to hold texture information
struct Texture {
    unsigned int id;   // The ID given by OpenGL when we load the image
    std::string type;  // Is this a regular color texture or something else? (diffuse, specular, etc.)
    std::string path;  // The file path of the image (useful to avoid loading the same image twice)
};

class Mesh {
public:
    // Mesh data
    std::vector<Vertex>       vertices; // List of all points
    std::vector<unsigned int> indices;  // Connect-the-dots instructions to make triangles
    std::vector<Texture>      textures; // Images applied to the mesh
    unsigned int VAO;                   // The "manager" ID that holds all our buffer settings

    // Constructor: gets the data and sets up the buffers automatically
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        // Now that we have the data, configure the graphics card
        setupMesh();
    }

    // Function to draw the mesh on the screen
    void Draw(Shader& shader) {

        // Draw the mesh
        glBindVertexArray(VAO); // Tell OpenGL to use our mesh's settings
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0); // Draw the triangles
        glBindVertexArray(0);   // Clean up (unbind)
    }

private:
    // Render data variables for the graphics card
    unsigned int VBO, EBO;

    // Initializes all the buffer objects and arrays
    void setupMesh() {
        // Create the buffers on the graphics card
        glGenVertexArrays(1, &VAO); // Vertex Array Object (The Manager)
        glGenBuffers(1, &VBO);      // Vertex Buffer Object (Holds the raw point data)
        glGenBuffers(1, &EBO);      // Element Buffer Object (Holds the connect-the-dots rules)

        // 1. Bind the VAO first, so it records our settings
        glBindVertexArray(VAO);

        // 2. Load data into the VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // We pass the size of the whole vector and a pointer to its first element
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        // 3. Load data into the EBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // 4. Tell OpenGL how to read the Vertex struct we made above

        // Position attribute (starts at the beginning of the struct)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

        // Normal attribute (starts after the 3 floats of the Position)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

        // Texture Coordinates attribute (starts after Position and Normal)
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

        // 5. Unbind the VAO so we don't accidentally mess up its settings later
        glBindVertexArray(0);
    }
};
#endif