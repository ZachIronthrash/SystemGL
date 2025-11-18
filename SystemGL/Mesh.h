#pragma once

class Mesh {
public:
    // mesh Data
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    unsigned int VAO = 0, VBO = 0, EBO = 0;

    // constructor
    Mesh(std::vector<float> vertices, std::vector<unsigned int> indices) : vertices(vertices), indices(indices)
    {
        // create buffers/arrays
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
        // set the vertex attribute pointers
        // vertex Positions
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // vertex Colors
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // THIS IS HOW YOU PASS ARRAYS SUCH THAT YOU KNOW THE SIZE
    template <size_t V, size_t I>
    Mesh(float(&vertices)[V], unsigned int(&indices)[I]) : vertices(vector<float>(vertices, vertices + V)), indices(vector<unsigned int>(indices, indices + I))
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
    }
    ~Mesh() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    // render the mesh
    void draw(Shader& shader)
    {
        // draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
};