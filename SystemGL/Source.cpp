#include "System.h"

#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

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

	// evolve the system over a single full orbit period    
	while (simpleSystem.time < 2 * PI / sqrt(2)) {
		grav1_2.apply();
		simpleSystem.evolve();

		pos1 = simpleSystem.subsystems[0].position;
		pos2 = simpleSystem.subsystems[1].position;

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

    // vertex data
    // -----------
    float vertices[] = {
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
	};



    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

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