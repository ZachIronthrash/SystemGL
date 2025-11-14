#include "System.h"
#include "Shader.h"

#include <cmath>
#include <chrono>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

void circleZ(vector<float>& vertices, vector<unsigned int>& indices, float radius, int subdivisions, glm::vec3 color);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main() {
    cout << "Hello, World!" << endl << endl;

    cout << "Creating a simple system..." << endl;

    // initialize particles as separate systems
    // ---------------------------------------
    vec3 pos1(-0.5, 0.0, 0.0);
    vec3 pos2(0.5, 0.0, 0.0);

    vec3 vel(0.0, 1.0 / sqrt(2.0), 0.0);

    System particle1(pos1, vel, 1.0);
    System particle2(pos2, -vel, 1.0);

    // combine particles into a single system
    // ---------------------------------------
    System simpleSystem({ particle1, particle2 });

    // define gravitational interaction between particles
    Interaction grav1_2(&simpleSystem.subsystems[0], &simpleSystem.subsystems[1], 'G');

    cout << "  Particle 1 Position: (" << pos1 << ")" << endl;
    cout << "  Particle 2 Position: (" << pos2 << ")" << endl << endl;

    cout << "  Particle 1 Velocity: (" << vel << ")" << endl;
    cout << "  Particle 2 Velocity: (" << -vel << ")" << endl << endl;

    cout << "Evolving the system for set steps..." << endl << endl;

    std::ofstream simulation;
    simulation.open("simulation.txt");

    simulation << "t" << simpleSystem.time << "; p1, " << pos1.x << ", " << pos1.y << ", " << pos1.z << "; p2, " << pos2.x << ", " << pos2.y << ", " << pos2.z << endl;

    // evolve the system over a single full orbit period    
    while (simpleSystem.time < 2 * PI / sqrt(2)) {
        grav1_2.apply();
        simpleSystem.evolve();

        pos1 = simpleSystem.subsystems[0].position;
        pos2 = simpleSystem.subsystems[1].position;

		simulation << "t" << simpleSystem.time << "; p1, " << pos1.x << ", " << pos1.y << ", " << pos1.z << "; p2, " << pos2.x << ", " << pos2.y << ", " << pos2.z << endl;

        if ((int)(simpleSystem.time * 1000) % 100 == 0) {
            cout << "  Particle 1 Position: (" << pos1 << ")" << endl;
            cout << "  Particle 2 Position: (" << pos2 << ")" << endl << endl;
        }

    }

    cout << "Final Positions after evolution:" << endl;
    cout << "  Particle 1 Position: (" << pos1 << ")" << endl;
    cout << "  Particle 2 Position: (" << pos2 << ")" << endl << endl;

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // shader
    // ------
    Shader shader("C:/Users/chris/source/repos/SystemGL/SystemGL/vertex.glsl", "C:/Users/chris/source/repos/SystemGL/SystemGL/fragment.glsl");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
 //   float vertices[] = {
 //       // positions         // colors
 //        0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // top right
 //        0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom right
 //       -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,   // bottom left
 //       -0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 0.0f   // top left 
 //   };
 //   unsigned int indices[] = {  // note that we start from 0!
 //       0, 1, 3,   // first triangle
 //       1, 2, 3    // second triangle
    //};

    vector<float> circleVert;
    vector<unsigned int> circleInd;

    circleZ(circleVert, circleInd, 0.25f, 10, { 1.0f, 0.0f, 0.0f });

    for (size_t i = 0; i < circleVert.size(); i++) {
        cout << circleVert[i] << ", ";
        if ((i + 1) % 6 == 0) cout << endl;
    }
    cout << endl;
    for (size_t i = 0; i < circleInd.size(); i++) {
        cout << circleInd[i] << ", ";
        if ((i + 1) % 3 == 0) cout << endl;
    }
    cout << endl;

    glm::vec3 circlePositions[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, 0.0f),
        glm::vec3(-1.5f, -2.2f, 0.0f),
        glm::vec3(-3.8f, -2.0f, 0.0f),
        glm::vec3(2.4f, -0.4f, 0.0f),
        glm::vec3(-1.7f,  3.0f, 0.0f),
        glm::vec3(1.3f, -2.0f, 0.0f),
        glm::vec3(1.5f,  2.0f, 0.0f),
        glm::vec3(1.5f,  0.2f, 0.0f),
        glm::vec3(-1.3f,  1.0f, 0.0f)
    };

    for (glm::vec3 &val : circlePositions) {
		val /= 5.0f;
    }

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, circleVert.size() * sizeof(float), circleVert.data(), GL_STATIC_DRAW);
    /*
    GL_STREAM_DRAW: the data is set only once and used by the GPU at most a few times.
    GL_STATIC_DRAW: the data is set only once and used many times.
    GL_DYNAMIC_DRAW: the data is changed a lot and used many times.
    */

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, circleInd.size() * sizeof(unsigned int), circleInd.data(), GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);


    // uncomment this call to draw in wireframe polygons.
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    float angle = 0.0f;

    int iterations = 0;

    chrono::high_resolution_clock::time_point lastFrameTime = chrono::high_resolution_clock::now();
	chrono::milliseconds frameDuration(16); // Approx. 60 FPS

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // activate shader
        shader.use();

        // create transformations
        glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        // retrieve the matrix uniform locations
        unsigned int modelLoc = glGetUniformLocation(shader.ID, "model");
        unsigned int viewLoc = glGetUniformLocation(shader.ID, "view");
        // pass them to the shaders (3 different ways)
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
        // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
        shader.setMat4("projection", projection);

        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

        for (unsigned int i = 0; i < 10; i++) {
            model = glm::mat4(1.0f);
            model = glm::rotate(model, glm::radians(-1.0f * iterations), glm::vec3(0.0f, 0.0f, 0.1f));
			model = glm::translate(model, circlePositions[i]);

			shader.setMat4("model", model);

            // draw the circle
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(circleInd.size()), GL_UNSIGNED_INT, 0);
        }

        //glDrawArrays(GL_TRIANGLES, 0, 6);
        //glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(circleInd.size()), GL_UNSIGNED_INT, 0);
        // glBindVertexArray(0); // no need to unbind it every time 

		// Wait for frame time to elapse (16ms for ~60FPS)
        while (chrono::high_resolution_clock::now() - lastFrameTime < frameDuration) { /* wait */ };

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();

		// Swapping frame time calculation after swapping buffers so that the time taken to render and display is included
		lastFrameTime = chrono::high_resolution_clock::now();

		iterations++;


    }

    // close files
	simulation.close();

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
	
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

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
void circleZ(vector<float>& vertices, vector<unsigned int>& indices, float radius, int subdivisions, glm::vec3 color) {
	assert(radius > 0.0f);
	assert(subdivisions >= 3);
	assert(color.x >= 0.0f && color.x <= 1.0f);
	assert(color.y >= 0.0f && color.y <= 1.0f);
	assert(color.z >= 0.0f && color.z <= 1.0f);

	// We need (subdivisions + 1) vertices: one for each subdivision point and one for the center
	// But the last vertex doesn't follow the loop pattern, so we handle it after
	// We only need subdivisions triangles, each connecting two adjacent subdivision points and the center point
    for (int i = 0; i < subdivisions; i++) {
        float angle = 2.0f * PI * i / subdivisions;
        vertices.push_back(radius * cos(angle));            // x
        vertices.push_back(radius * sin(angle));            // y
        vertices.push_back(0.0f);                           // z
        vertices.push_back(color.x);                           // r
        vertices.push_back(color.y);                          // g
        vertices.push_back(color.z);                           // b

        // triangle: i, (i+1) % subdivisions, centerIndex (will be subdivisions)
        indices.push_back(subdivisions);
        indices.push_back(i);
        indices.push_back((i + 1) % subdivisions);

    }

    vertices.push_back(0.0f);       // center x
    vertices.push_back(0.0f);       // center y
    vertices.push_back(0.0f);       // center z
    vertices.push_back(color.x);       // r
    vertices.push_back(color.y);		// g
    vertices.push_back(color.z); 		// b
}