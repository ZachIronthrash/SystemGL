#pragma once

#include "Shader.h"
#include "SystemGLMath.h"
#include <cassert>
#include <cmath>
#include <glad/glad.h> // ensure OpenGL symbols are visible when this header is parsed
//#include <glm/fwd.hpp>
#include <vector>
#include <glm/glm.hpp>
#include <algorithm>
#include <cstdint>
#include <unordered_map>

/*
* Source code for Mesh class is from https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/mesh.h
* This header and its associated helper function(s) are used for creating and rendering meshes, which are used for rendering particles and other objects in the scene.
* The focus of this project is not on rendering, so much of the additional code is based on AI responses.
* Functions whose implementation is based on AI responses are marked with "AI GENERATED" in the comments, and may be subject to change as development continues.
*/

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
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
		// set the vertex attribute pointers
		// vertex Positions
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// vertex Normals
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

//static void sphere(std::vector<float>& vert, std::vector<unsigned>& ind, float r, unsigned stacks, unsigned slices, glm::vec3 c) {
//	assert(r > 0.0f);
//	assert(stacks > 0);
//	assert(slices > 0);
//
//	// Each vertex stores 6 floats: x,y,z,r,g,b
//	const unsigned floatsPerVertex = 6u;
//
//	// If vert already contains data, compute base index so indices are appended correctly.
//	unsigned int baseIndex = static_cast<unsigned int>(vert.size() / floatsPerVertex);
//
//	// Reserve capacity to avoid repeated reallocations
//	vert.reserve(vert.size() + (static_cast<size_t>(stacks) + 1) * (static_cast<size_t>(slices) + 1) * floatsPerVertex);
//	ind.reserve(ind.size() + static_cast<size_t>(stacks) * static_cast<size_t>(slices) * 6u); // 2 triangles * 3 indices
//
//	for (unsigned i = 0; i <= stacks; i++) {
//		float phi = pi * static_cast<float>(i) / static_cast<float>(stacks);
//		float sinPhi = std::sin(phi);
//		float cosPhi = std::cos(phi);
//
//		for (unsigned j = 0; j <= slices; j++) {
//			float theta = 2.0f * pi * static_cast<float>(j) / static_cast<float>(slices);
//			float sinTheta = std::sin(theta);
//			float cosTheta = std::cos(theta);
//
//			float x = r * sinPhi * cosTheta;
//			float y = r * cosPhi;
//			float z = r * sinPhi * sinTheta;
//
//			vert.push_back(x);
//			vert.push_back(y);
//			vert.push_back(z);
//
//			vert.push_back(c.x);
//			vert.push_back(c.y);
//			vert.push_back(c.z);
//		}
//	}
//
//	for (unsigned i = 0; i < stacks; ++i) {
//		for (unsigned j = 0; j < slices; ++j) {
//			unsigned p1 = baseIndex + i * (slices + 1) + j;
//			unsigned p2 = baseIndex + i * (slices + 1) + (j + 1);
//			unsigned p3 = baseIndex + (i + 1) * (slices + 1) + (j + 1);
//			unsigned p4 = baseIndex + (i + 1) * (slices + 1) + j;
//
//			ind.push_back(p1);
//			ind.push_back(p2);
//			ind.push_back(p4);
//
//			ind.push_back(p2);
//			ind.push_back(p3);
//			ind.push_back(p4);
//		}
//	}
//}

//static void simpleCube(std::vector<float>& vert, std::vector<unsigned>& ind, float width, glm::vec3 c) {
//	float half = width / 2.0f;
//
//	float vertices[] = {
//		half, half, half, c.x, c.y, c.z, // 0
//		half, half, -half, c.x, c.y, c.z, // 1
//		half, -half, half, c.x, c.y, c.z, // 2
//		half, -half, -half, c.x, c.y, c.z, // 3
//		-half, half, half, c.x, c.y, c.z, // 4
//		-half, half, -half, c.x, c.y, c.z, // 5
//		-half, -half, half, c.x, c.y, c.z, // 6
//		-half, -half, -half, c.x, c.y, c.z // 7
//	};
//
//	for (float f : vertices) {
//		vert.push_back(f);
//	}
//
//	unsigned indices[] = {
//		0, 1, 2, 1, 3, 2, // right face
//		4, 6, 5, 5, 6, 7, // left face
//		4, 5, 0, 5, 1, 0, // top face
//		6, 2, 7, 7, 2, 3, // bottom face
//		4, 0, 6, 6, 0, 2, // front face
//		5, 7, 1, 7, 3, 1 // back face
//	};
//
//	for (unsigned i : indices) {
//		ind.push_back(i);
//	}
//}

//static void simpleCubeWithNormals(std::vector<float>& vert, std::vector<unsigned>& ind, float width) {
//	float half = width / 2.0f;
//
//	float vertices[] = {
//		half, half, half,// c.x, c.y, c.z, // 0
//		half, half, -half,// c.x, c.y, c.z, // 1
//		half, -half, half,// c.x, c.y, c.z, // 2
//		half, -half, -half,// c.x, c.y, c.z, // 3
//		-half, half, half,// c.x, c.y, c.z, // 4
//		-half, half, -half,// c.x, c.y, c.z, // 5
//		-half, -half, half,// c.x, c.y, c.z, // 6
//		-half, -half, -half//, c.x, c.y, c.z // 7
//	};
//
//	//for (float f : vertices) {
//	//	vert.push_back(f);
//	//}
//
//	unsigned indices[36] = {
//		0, 1, 2, 1, 3, 2, // right face
//		4, 6, 5, 5, 6, 7, // left face
//		4, 5, 0, 5, 1, 0, // top face
//		6, 2, 7, 7, 2, 3, // bottom face
//		4, 0, 6, 6, 0, 2, // front face
//		5, 7, 1, 7, 3, 1 // back face
//	};
//
//	//for (unsigned i : indices) {
//	//	vert.push_back(vertices[6 * i]);     // x
//	//	vert.push_back(vertices[6 * i + 1]); // y
//	//	vert.push_back(vertices[6 * i + 2]); // z
//	//	vec3 normal;
//	//	switch (i / 6) { // each face has 6 indices, so integer division gives us the face index
//	//	case 0: normal = vec3(1.0f, 0.0f, 0.0f); break; // right face
//	//	case 1: normal = vec3(-1.0f, 0.0f, 0.0f); break; // left face
//	//	case 2: normal = vec3(0.0f, 1.0f, 0.0f); break; // top face
//	//	case 3: normal = vec3(0.0f, -1.0f, 0.0f); break; // bottom face
//	//	case 4: normal = vec3(0.0f, 0.0f, 1.0f); break; // front face
//	//	case 5: normal = vec3(0.0f, 0.0f, -1.0f); break; // back face
//	//	default: normal = vec3(1.0f, 1.0f, 1.0f); break; // should never happen
//	//	}
//	//	vert.push_back(normal.x);
//	//	vert.push_back(normal.y);
//	//	vert.push_back(normal.z);
//	//}
//
//	for (int i = 0; i < 36; i++) {
//		int index = (int)indices[i];
//
//		ind.push_back(i);
//		//ind.push_back(count);
//
//		vert.push_back(vertices[3 * index]);     // x
//		vert.push_back(vertices[3 * index + 1]); // y
//		vert.push_back(vertices[3 * index + 2]); // z
//		glm::vec3 normal;
//		switch (i / 6) { // each face has 6 indices, so integer division gives us the face index
//		case 0: normal = glm::vec3(1.0f, 0.0f, 0.0f); break; // right face
//		case 1: normal = glm::vec3(-1.0f, 0.0f, 0.0f); break; // left face
//		case 2: normal = glm::vec3(0.0f, 1.0f, 0.0f); break; // top face
//		case 3: normal = glm::vec3(0.0f, -1.0f, 0.0f); break; // bottom face
//		case 4: normal = glm::vec3(0.0f, 0.0f, 1.0f); break; // front face
//		case 5: normal = glm::vec3(0.0f, 0.0f, -1.0f); break; // back face
//		default: normal = glm::vec3(1.0f, 1.0f, 1.0f); break; // should never happen
//		}
//		vert.push_back(normal.x);
//		vert.push_back(normal.y);
//		vert.push_back(normal.z);
//	}
//
//	//int count = 0;
//	//for (float f : vert) {
//	//	std::cout << f;
//	//	if (count % 6 == 5) {
//	//		std::cout << std::endl;
//	//	}
//	//	else {
//	//		std::cout << ", ";
//	//	}
//	//	count++;
//	//}
//
//}

static void invertedSimpleBox(std::vector<float>& vert, std::vector<unsigned>& ind, vec3 size, glm::vec3 c) {
	float half[3] = { (float)size.x / 2.0f, (float)size.y / 2.0f, (float)size.z / 2.0f };

	float vertices[] = {
		 half[0],  half[1],  half[2], c.x, c.y, c.z, // 0
		 half[0],  half[1], -half[2], c.x, c.y, c.z, // 1
		 half[0], -half[1],  half[2], c.x, c.y, c.z, // 2
		 half[0], -half[1], -half[2], c.x, c.y, c.z, // 3
		-half[0],  half[1],  half[2], c.x, c.y, c.z, // 4
		-half[0],  half[1], -half[2], c.x, c.y, c.z, // 5
		-half[0], -half[1],  half[2], c.x, c.y, c.z, // 6
		-half[0], -half[1], -half[2], c.x, c.y, c.z // 7
	};

	for (float f : vertices) {
		vert.push_back(f);
	}

	unsigned indices[] = {
		0, 2, 1, 1, 2, 3, // right face
		4, 5, 6, 5, 7, 6, // left face
		4, 0, 5, 5, 0, 1, // top face
		6, 7, 2, 7, 3, 2, // bottom face
		4, 6, 0, 6, 2, 0, // front face
		5, 1, 7, 7, 1, 3 // back face
	};

	for (unsigned i : indices) {
		ind.push_back(i);
	}
}

static void invertedSimpleBoxWithNormals(std::vector<float>& vert, std::vector<unsigned>& ind, vec3 size) {
	float half[3] = { (float)size.x / 2.0f, (float)size.y / 2.0f, (float)size.z / 2.0f };

	float vertices[] = {
		 half[0],  half[1],  half[2],// c.x, c.y, c.z, // 0
		 half[0],  half[1], -half[2],// c.x, c.y, c.z, // 1
		 half[0], -half[1],  half[2],// c.x, c.y, c.z, // 2
		 half[0], -half[1], -half[2],// c.x, c.y, c.z, // 3
		-half[0],  half[1],  half[2],// c.x, c.y, c.z, // 4
		-half[0],  half[1], -half[2],// c.x, c.y, c.z, // 5
		-half[0], -half[1],  half[2],// c.x, c.y, c.z, // 6
		-half[0], -half[1], -half[2]//, c.x, c.y, c.z // 7
	};

	//for (float f : vertices) {
	//	vert.push_back(f);
	//}

	unsigned indices[] = {
		0, 2, 1, 1, 2, 3, // right face
		4, 5, 6, 5, 7, 6, // left face
		4, 0, 5, 5, 0, 1, // top face
		6, 7, 2, 7, 3, 2, // bottom face
		4, 6, 0, 6, 2, 0, // front face
		5, 1, 7, 7, 1, 3 // back face
	};

	for (int i = 0; i < 36; i++) {
		int index = (int)indices[i];

		ind.push_back(i);

		vert.push_back(vertices[3 * index]);     // x
		vert.push_back(vertices[3 * index + 1]); // y
		vert.push_back(vertices[3 * index + 2]); // z
		glm::vec3 normal;
		switch (i / 6) { // each face has 6 indices, so integer division gives us the face index
		case 0: normal = glm::vec3(-1.0f, 0.0f, 0.0f); break; // right face
		case 1: normal = glm::vec3(1.0f, 0.0f, 0.0f); break; // left face
		case 2: normal = glm::vec3(0.0f, -1.0f, 0.0f); break; // top face
		case 3: normal = glm::vec3(0.0f, 1.0f, 0.0f); break; // bottom face
		case 4: normal = glm::vec3(0.0f, 0.0f, -1.0f); break; // front face
		case 5: normal = glm::vec3(0.0f, 0.0f, 1.0f); break; // back face
		default: normal = glm::vec3(1.0f, 1.0f, 1.0f); break; // should never happen
		}
		vert.push_back(normal.x);
		vert.push_back(normal.y);
		vert.push_back(normal.z);
	}
}

// useful when your light source is above/outside the inverted box, since shadows might behave weirdly on the intervening face
// should revisit this once I have some version of diffuse lighting
static void invertedSimpleBoxNoTop(std::vector<float>& vert, std::vector<unsigned>& ind, vec3 size) {
	float half[3] = { (float)size.x / 2.0f, (float)size.y / 2.0f, (float)size.z / 2.0f };

	float vertices[] = {
		 half[0],  half[1],  half[2],// c.x, c.y, c.z, // 0
		 half[0],  half[1], -half[2],// c.x, c.y, c.z, // 1
		 half[0], -half[1],  half[2],// c.x, c.y, c.z, // 2
		 half[0], -half[1], -half[2],// c.x, c.y, c.z, // 3
		-half[0],  half[1],  half[2],// c.x, c.y, c.z, // 4
		-half[0],  half[1], -half[2],// c.x, c.y, c.z, // 5
		-half[0], -half[1],  half[2],// c.x, c.y, c.z, // 6
		-half[0], -half[1], -half[2]//, c.x, c.y, c.z // 7
	};

	//for (float f : vertices) {
	//	vert.push_back(f);
	//}

	unsigned indices[] = {
		0, 2, 1, 1, 2, 3, // right face
		4, 5, 6, 5, 7, 6, // left face
		//4, 0, 5, 5, 0, 1, // top face
		6, 7, 2, 7, 3, 2, // bottom face
		4, 6, 0, 6, 2, 0, // front face
		5, 1, 7, 7, 1, 3 // back face
	};

	for (int i = 0; i < 30; i++) {
		int index = (int)indices[i];

		ind.push_back(i);

		vert.push_back(vertices[3 * index]);     // x
		vert.push_back(vertices[3 * index + 1]); // y
		vert.push_back(vertices[3 * index + 2]); // z
		glm::vec3 normal;
		switch (i / 6) { // each face has 6 indices, so integer division gives us the face index
		case 0: normal = glm::vec3(-1.0f, 0.0f, 0.0f); break; // right face
		case 1: normal = glm::vec3(1.0f, 0.0f, 0.0f); break; // left face
			//case 2: normal = glm::vec3(0.0f, -1.0f, 0.0f); break; // top face
		case 2: normal = glm::vec3(0.0f, 1.0f, 0.0f); break; // bottom face
		case 3: normal = glm::vec3(0.0f, 0.0f, -1.0f); break; // front face
		case 4: normal = glm::vec3(0.0f, 0.0f, 1.0f); break; // back face
		default: normal = glm::vec3(1.0f, 1.0f, 1.0f); break; // should never happen
		}
		vert.push_back(normal.x);
		vert.push_back(normal.y);
		vert.push_back(normal.z);
	}
}

/*
* Human written with AI input for debugging.
*/
static Mesh cube(float width) {
	std::vector<float> vert;
	std::vector<unsigned> ind;

	float half = width / 2.0f;

	float vertices[] = {
		half, half, half,// c.x, c.y, c.z, // 0
		half, half, -half,// c.x, c.y, c.z, // 1
		half, -half, half,// c.x, c.y, c.z, // 2
		half, -half, -half,// c.x, c.y, c.z, // 3
		-half, half, half,// c.x, c.y, c.z, // 4
		-half, half, -half,// c.x, c.y, c.z, // 5
		-half, -half, half,// c.x, c.y, c.z, // 6
		-half, -half, -half//, c.x, c.y, c.z // 7
	};

	unsigned indices[36] = {
		0, 1, 2, 1, 3, 2, // right face
		4, 6, 5, 5, 6, 7, // left face
		4, 5, 0, 5, 1, 0, // top face
		6, 2, 7, 7, 2, 3, // bottom face
		4, 0, 6, 6, 0, 2, // front face
		5, 7, 1, 7, 3, 1 // back face
	};

	for (int i = 0; i < 36; i++) {
		int index = (int)indices[i];

		ind.push_back(i);
		//ind.push_back(count);

		vert.push_back(vertices[3 * index]);     // x
		vert.push_back(vertices[3 * index + 1]); // y
		vert.push_back(vertices[3 * index + 2]); // z
		glm::vec3 normal;
		switch (i / 6) { // each face has 6 indices, so integer division gives us the face index
		case 0: normal = glm::vec3(1.0f, 0.0f, 0.0f); break; // right face
		case 1: normal = glm::vec3(-1.0f, 0.0f, 0.0f); break; // left face
		case 2: normal = glm::vec3(0.0f, 1.0f, 0.0f); break; // top face
		case 3: normal = glm::vec3(0.0f, -1.0f, 0.0f); break; // bottom face
		case 4: normal = glm::vec3(0.0f, 0.0f, 1.0f); break; // front face
		case 5: normal = glm::vec3(0.0f, 0.0f, -1.0f); break; // back face
		default: normal = glm::vec3(1.0f, 1.0f, 1.0f); break; // should never happen
		}
		vert.push_back(normal.x);
		vert.push_back(normal.y);
		vert.push_back(normal.z);
	}

	return Mesh(vert, ind);
}

static Mesh invertedBox(vec3 size) {
	float half[3] = { (float)size.x / 2.0f, (float)size.y / 2.0f, (float)size.z / 2.0f };

	std::vector<float> vert;
	std::vector<unsigned> ind;

	float vertices[] = {
		 half[0],  half[1],  half[2],// c.x, c.y, c.z, // 0
		 half[0],  half[1], -half[2],// c.x, c.y, c.z, // 1
		 half[0], -half[1],  half[2],// c.x, c.y, c.z, // 2
		 half[0], -half[1], -half[2],// c.x, c.y, c.z, // 3
		-half[0],  half[1],  half[2],// c.x, c.y, c.z, // 4
		-half[0],  half[1], -half[2],// c.x, c.y, c.z, // 5
		-half[0], -half[1],  half[2],// c.x, c.y, c.z, // 6
		-half[0], -half[1], -half[2]//, c.x, c.y, c.z // 7
	};

	unsigned indices[] = {
		0, 2, 1, 1, 2, 3, // right face
		4, 5, 6, 5, 7, 6, // left face
		4, 0, 5, 5, 0, 1, // top face
		6, 7, 2, 7, 3, 2, // bottom face
		4, 6, 0, 6, 2, 0, // front face
		5, 1, 7, 7, 1, 3 // back face
	};

	for (int i = 0; i < 36; i++) {
		int index = (int)indices[i];

		ind.push_back(i);

		vert.push_back(vertices[3 * index]);     // x
		vert.push_back(vertices[3 * index + 1]); // y
		vert.push_back(vertices[3 * index + 2]); // z
		glm::vec3 normal;
		switch (i / 6) { // each face has 6 indices, so integer division gives us the face index
		case 0: normal = glm::vec3(-1.0f, 0.0f, 0.0f); break; // right face
		case 1: normal = glm::vec3(1.0f, 0.0f, 0.0f); break; // left face
		case 2: normal = glm::vec3(0.0f, -1.0f, 0.0f); break; // top face
		case 3: normal = glm::vec3(0.0f, 1.0f, 0.0f); break; // bottom face
		case 4: normal = glm::vec3(0.0f, 0.0f, -1.0f); break; // front face
		case 5: normal = glm::vec3(0.0f, 0.0f, 1.0f); break; // back face
		default: normal = glm::vec3(1.0f, 1.0f, 1.0f); break; // should never happen
		}
		vert.push_back(normal.x);
		vert.push_back(normal.y);
		vert.push_back(normal.z);
	}

	return Mesh(vert, ind);
}

static Mesh invertedBoxNoTop(vec3 size) {
	float half[3] = { (float)size.x / 2.0f, (float)size.y / 2.0f, (float)size.z / 2.0f };

	std::vector<float> vert;
	std::vector<unsigned> ind;

	float vertices[] = {
		 half[0],  half[1],  half[2],// c.x, c.y, c.z, // 0
		 half[0],  half[1], -half[2],// c.x, c.y, c.z, // 1
		 half[0], -half[1],  half[2],// c.x, c.y, c.z, // 2
		 half[0], -half[1], -half[2],// c.x, c.y, c.z, // 3
		-half[0],  half[1],  half[2],// c.x, c.y, c.z, // 4
		-half[0],  half[1], -half[2],// c.x, c.y, c.z, // 5
		-half[0], -half[1],  half[2],// c.x, c.y, c.z, // 6
		-half[0], -half[1], -half[2]//, c.x, c.y, c.z // 7
	};

	unsigned indices[] = {
		0, 2, 1, 1, 2, 3, // right face
		4, 5, 6, 5, 7, 6, // left face
		//4, 0, 5, 5, 0, 1, // top face
		6, 7, 2, 7, 3, 2, // bottom face
		4, 6, 0, 6, 2, 0, // front face
		5, 1, 7, 7, 1, 3 // back face
	};

	for (int i = 0; i < 30; i++) {
		int index = (int)indices[i];

		ind.push_back(i);

		vert.push_back(vertices[3 * index]);     // x
		vert.push_back(vertices[3 * index + 1]); // y
		vert.push_back(vertices[3 * index + 2]); // z
		glm::vec3 normal;
		switch (i / 6) { // each face has 6 indices, so integer division gives us the face index
		case 0: normal = glm::vec3(-1.0f, 0.0f, 0.0f); break; // right face
		case 1: normal = glm::vec3(1.0f, 0.0f, 0.0f); break; // left face
			//case 2: normal = glm::vec3(0.0f, -1.0f, 0.0f); break; // top face
		case 2: normal = glm::vec3(0.0f, 1.0f, 0.0f); break; // bottom face
		case 3: normal = glm::vec3(0.0f, 0.0f, -1.0f); break; // front face
		case 4: normal = glm::vec3(0.0f, 0.0f, 1.0f); break; // back face
		default: normal = glm::vec3(1.0f, 1.0f, 1.0f); break; // should never happen
		}
		vert.push_back(normal.x);
		vert.push_back(normal.y);
		vert.push_back(normal.z);
	}

	return Mesh(vert, ind);
}

/*
* Partially **AI GENERATED** - based on the deprecated sphere function above, but modified to return a Mesh object directly and to compute normals for each vertex.
*/
static Mesh sphereStack_n_Slice(float r, unsigned stacks, unsigned slices) {
	if (stacks < 2) stacks = 2;
	if (slices < 3) slices = 3;

	std::vector<float> vertices;
	std::vector<unsigned int> indices;

	for (unsigned i = 0; i <= stacks; i++) {
		float phi = pi * static_cast<float>(i) / static_cast<float>(stacks);
		float sinPhi = std::sin(phi);
		float cosPhi = std::cos(phi);
		for (unsigned j = 0; j <= slices; j++) {
			float theta = 2.0f * pi * static_cast<float>(j) / static_cast<float>(slices);
			float sinTheta = std::sin(theta);
			float cosTheta = std::cos(theta);
			float x = r * sinPhi * cosTheta;
			float y = r * cosPhi;
			float z = r * sinPhi * sinTheta;
			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);
			glm::vec3 normal(x, y, z);
			normal = glm::normalize(normal);
			vertices.push_back(normal.x);
			vertices.push_back(normal.y);
			vertices.push_back(normal.z);
		}
	}

	unsigned verticesPerStack = slices + 1;

	for (unsigned i = 0; i < stacks; ++i) {
		for (unsigned j = 0; j < slices; ++j) {
			unsigned p1 = i * verticesPerStack + j;
			unsigned p2 = i * verticesPerStack + (j + 1);
			unsigned p3 = (i + 1) * verticesPerStack + (j + 1);
			unsigned p4 = (i + 1) * verticesPerStack + j;
			indices.push_back(p1);
			indices.push_back(p4);
			indices.push_back(p2);
			indices.push_back(p2);
			indices.push_back(p4);
			indices.push_back(p3);
		}
	}

	Mesh mesh(vertices, indices);

	return mesh;
}

/*
* **AI GENERATED** - initializes an icosphere mesh with the specified radius and subdivision level, and returns it as a Mesh object.
*/
static Mesh icosphere(float r, unsigned subdivisions)
{
	struct Vec3 { float x, y, z; };

	std::vector<Vec3> positions;
	std::vector<unsigned int> indices;

	auto normalize = [](Vec3 v)->Vec3 {
		float len = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
		return { v.x / len, v.y / len, v.z / len };
		};

	auto addVertex = [&](Vec3 v)->unsigned int {
		Vec3 n = normalize(v);
		positions.push_back(n);
		return (unsigned int)positions.size() - 1;
		};

	// ---- base icosahedron ----
	const float t = (1.0f + std::sqrt(5.0f)) * 0.5f;

	addVertex({ -1,  t,  0 });
	addVertex({ 1,  t,  0 });
	addVertex({ -1, -t,  0 });
	addVertex({ 1, -t,  0 });
	addVertex({ 0, -1, -t });
	addVertex({ 0,  1, -t });
	addVertex({ 0, -1, t });
	addVertex({ 0,  1, t });
	addVertex({ t,  0, 1 });
	addVertex({ t,  0,  -1 });
	addVertex({ -t,  0, 1 });
	addVertex({ -t,  0,  -1 });

	unsigned int baseIdx[] = {
		0,11,5,  0,5,1,  0,1,7,  0,7,10, 0,10,11,
		1,5,9,   5,11,4, 11,10,2, 10,7,6, 7,1,8,
		3,9,4,   3,4,2,  3,2,6,  3,6,8,  3,8,9,
		4,9,5,   2,4,11, 6,2,10, 8,6,7,  9,8,1
	};
	indices.assign(baseIdx, baseIdx + 60);

	// ---- midpoint cache ----
	std::unordered_map<uint64_t, unsigned int> midpointCache;

	auto midpoint = [&](unsigned int a, unsigned int b)->unsigned int {
		uint64_t key = ((uint64_t)std::min(a, b) << 32) | std::max(a, b);
		auto it = midpointCache.find(key);
		if (it != midpointCache.end()) return it->second;

		Vec3 v1 = positions[a];
		Vec3 v2 = positions[b];
		Vec3 m = { (v1.x + v2.x) * 0.5f, (v1.y + v2.y) * 0.5f, (v1.z + v2.z) * 0.5f };
		unsigned int idx = addVertex(m);
		midpointCache[key] = idx;
		return idx;
		};

	// ---- subdivisions ----
	for (unsigned s = 0; s < subdivisions; ++s)
	{
		std::vector<unsigned int> newIdx;
		newIdx.reserve(indices.size() * 4);

		for (size_t i = 0; i < indices.size(); i += 3)
		{
			unsigned int i0 = indices[i];
			unsigned int i1 = indices[i + 1];
			unsigned int i2 = indices[i + 2];

			unsigned int a = midpoint(i0, i1);
			unsigned int b = midpoint(i1, i2);
			unsigned int c = midpoint(i2, i0);

			// preserve CCW orientation of parent triangle
			newIdx.push_back(i0); newIdx.push_back(a); newIdx.push_back(c);
			newIdx.push_back(i1); newIdx.push_back(b); newIdx.push_back(a);
			newIdx.push_back(i2); newIdx.push_back(c); newIdx.push_back(b);
			newIdx.push_back(a);  newIdx.push_back(b); newIdx.push_back(c);
		}

		indices.swap(newIdx);
	}

	// ---- build vertex buffer ----
	std::vector<float> vertices;
	vertices.reserve(positions.size() * 6);

	for (auto& p : positions)
	{
		glm::vec3 n = glm::normalize(glm::vec3(p.x, p.y, p.z));
		glm::vec3 pos = n * r;

		vertices.push_back(pos.x);
		vertices.push_back(pos.y);
		vertices.push_back(pos.z);

		vertices.push_back(n.x);
		vertices.push_back(n.y);
		vertices.push_back(n.z);
	}

	return Mesh(vertices, indices);
}