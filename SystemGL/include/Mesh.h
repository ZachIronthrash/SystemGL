#pragma once

#include <vector>

#include <cmath>

#include <glad/glad.h> // ensure OpenGL symbols are visible when this header is parsed

#include "Shader.h"
#include <cassert>
#include <glm/fwd.hpp>

const float pi = 2 * acos(0.0f);

class Mesh {
public:
	// mesh Data
	std::vector<float> vertices;
	std::vector<unsigned int> indices;
	unsigned int VAO = 0, VBO = 0, EBO = 0;

	Mesh() {};

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

	//void transferFrom(Mesh& mesh) {
	//    this->vertices = mesh.vertices;
	//    this->indices = mesh.indices;

	//    // create buffers/arrays
	//    glGenVertexArrays(1, &VAO);
	//    glGenBuffers(1, &VBO);
	//    glGenBuffers(1, &EBO);
	//    glBindVertexArray(VAO);
	//    glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	//    // set the vertex attribute pointers
	//    // vertex Positions
	//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	//    glEnableVertexAttribArray(0);
	//    // vertex Colors
	//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	//    glEnableVertexAttribArray(1);
	//    glBindBuffer(GL_ARRAY_BUFFER, 0);
	//    glBindVertexArray(0);

	//    // unbind to be safe
	//    glBindBuffer(GL_ARRAY_BUFFER, 0);
	//    glBindVertexArray(0);
	//}
};

/*
* Adds vertex data to vertices and index data to indices for a circle in the Z plane centered at the origin.
*
* @param vertices - vector to append vertex data to (x, y, z, r, g, b for each vertex; alpha value pending further development)
* @param indices - vector to append index data to (unsigned int indices for triangles, starts from 0, middle point is at index = subdivisions)
* @param radius - radius of the circle
* @param subdivisions - number of subdivisions (triangles) to create the circle
* @param color - color of the circle (r, g, b)
*
* @updates vertices - with valid vertex data with specified radius, subdivisions, and color
* @updates indices - with valid index data for triangles composing the circle
*
* @requires radius > 0, subdivisions >= 3, 0 <= color.r,g,b <= 1.0f
* @ensures vertices and indices are appended with new data
*/
static void circleZ(std::vector<float>& vertices, std::vector<unsigned int>& indices, float radius, int subdivisions, glm::vec3 color) {
	assert(radius > 0.0f);
	assert(subdivisions >= 3);
	assert(color.x >= 0.0f && color.x <= 1.0f);
	assert(color.y >= 0.0f && color.y <= 1.0f);
	assert(color.z >= 0.0f && color.z <= 1.0f);

	// We need (subdivisions + 1) vertices: one for each subdivision point and one for the center
	// But the last vertex doesn't follow the loop pattern, so we handle it after
	// We only need subdivisions triangles, each connecting two adjacent subdivision points and the center point
	for (int i = 0; i < subdivisions; i++) {
		float angle = 2.0f * pi * i / subdivisions;
		vertices.push_back(radius * cos(angle));            // x
		vertices.push_back(radius * sin(angle));            // y
		vertices.push_back(0.0f);                           // z
		vertices.push_back(color.x);                        // r
		vertices.push_back(color.y);                        // g
		vertices.push_back(color.z);                        // b

		// triangle: i, (i+1) % subdivisions, centerIndex (will be subdivisions)
		indices.push_back(subdivisions);
		indices.push_back(i);
		indices.push_back((i + 1) % subdivisions);

	}

	vertices.push_back(0.0f);           // center x
	vertices.push_back(0.0f);           // center y
	vertices.push_back(0.0f);           // center z
	vertices.push_back(color.x);        // r
	vertices.push_back(color.y);		// g
	vertices.push_back(color.z); 		// b
}