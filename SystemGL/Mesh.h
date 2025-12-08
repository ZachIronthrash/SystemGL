#pragma once

#include <vector>
#include <cstddef>

#include <glad/glad.h> // ensure OpenGL symbols are visible when this header is parsed

#include "Shader.h"

class Mesh {
public:
    // mesh Data
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    unsigned int VAO = 0, VBO = 0, EBO = 0;

    // constructor
    Mesh(std::vector<float> vertices, std::vector<unsigned int> indices);

    // render the mesh
    void draw(Shader& shader);

    
    // THIS IS HOW YOU PASS ARRAYS SUCH THAT YOU KNOW THE SIZE
    template <size_t V, size_t I>
    Mesh(float(&vertices)[V], unsigned int(&indices)[I]) : vertices(std::vector<float>(vertices, vertices + V)), indices(std::vector<unsigned int>(indices, indices + I))
    {
        /*this->vertices.assign(vertices, vertices + V);
        this->indices.assign(indices, indices + I);*/
        /*this->vertices = vector<float>(vertices, vertices + V);
        this->indices = vector<unsigned int>(indices, indices + I);*/

        /*for (float vertex : this->vertices) {
            cout << vertex << ", ";
        }
        cout << endl;
        for (float index : this->indices) {
            cout << index << ", ";
        }*/

        // create buffers/arrays
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        // set the vertex attribute pointers
        // vertex Positions
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // vertex Colors
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        // unbind to be safe
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
};