#include "System.h"
#include "Shader.h"
#include "Mesh.h"

#include <cmath>
#include <chrono>
#include <sstream> // For file parsing

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

void resetSystem(System& system, int n);
void updateSystemFromFile(System& system, ifstream& inputData, float& targetTime, bool& foundTargetTime);
void circleZ(vector<float>& vertices, vector<unsigned int>& indices, float radius, int subdivisions, glm::vec3 color);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

chrono::milliseconds FRAME_DURATION(16); // Approx. 60 FPS
float FRAME_TIME = FRAME_DURATION.count() / 1000.0f; // in seconds

// keyboard input processing
bool KEY_E = false;
bool KEY_SPACE = false;

bool PAUSE = true;

int main() {
    cout << "Hello, World!" << endl << endl;

    cout << "Creating a simple system..." << endl;
    
    System simpleSystem;
    resetSystem(simpleSystem, 100);

    System pressureApproximationSystem;

    ifstream data;
	data.open("simulation.txt");

    float zero = 0.0f;
    resetSystem(pressureApproximationSystem, 100);
	bool foundTarget = false;
	updateSystemFromFile(pressureApproximationSystem, data, zero, foundTarget);

    for (System& particle : pressureApproximationSystem.subsystems) {
        cout << "Particle Position: (" << particle.position << ")" << endl;
	}

 //   // initialize particles as separate systems
 //   // ---------------------------------------
 //   vec3 pos1(-0.5, 0.0, 0.0);
 //   vec3 pos2(0.5, 0.0, 0.0);

 //   vec3 vel(0.0, 1.0 / sqrt(2.0), 0.0);

 //   System particle1(pos1, vel, 1.0);
 //   System particle2(pos2, -vel, 1.0);
	//System particle3(vec3(0, 0.0, 0.0), vec3(0, 0.0, 0.0), 0.05);

 //   // combine particles into a single system
 //   // ---------------------------------------
 //   System simpleSystem({ particle1, particle2, particle3 });

    // define gravitational interaction between particles
    //Interaction grav1_2(&simpleSystem.subsystems[0], &simpleSystem.subsystems[1], 'G');

	//simpleGravitationalOrbitSim(simpleSystem, ofstream());

    /*cout << "Final Positions after evolution:" << endl;
    cout << "  Particle 1 Position: (" << pos1 << ")" << endl;
    cout << "  Particle 2 Position: (" << pos2 << ")" << endl << endl;*/

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

    vector<float> circleVert;
    vector<unsigned int> circleInd;

    circleZ(circleVert, circleInd, 0.02f, 12, { 1.0f, 0.0f, 0.0f });

    float bbWidth = 2.0f;
    float bbHeight = 2.0f;
    float boundingBoxVert[] = {
		// positions                        // colors
         bbWidth / 2,  bbHeight / 2, 0,     0, 0, 0,    // Top right
         bbWidth / 2, -bbHeight / 2, 0,     0, 0, 0,    // Bottom right
        -bbWidth / 2,  bbHeight / 2, 0,     0, 0, 0,    // Top left
        -bbWidth / 2, -bbHeight / 2, 0,     0, 0, 0     // Bottom left
    };
    unsigned int boundingBoxInd[] = {
        0, 1, 2,
        2, 3, 1
    };

	Mesh circleMesh(circleVert, circleInd);
    Mesh bbMesh(boundingBoxVert, boundingBoxInd);

    //unsigned int VBO, VAO, EBO;
    //glGenVertexArrays(1, &VAO);
    //glGenBuffers(1, &VBO);
    //glGenBuffers(1, &EBO);
    //// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    //glBindVertexArray(VAO);

    ///*
    //GL_STREAM_DRAW: the data is set only once and used by the GPU at most a few times.
    //GL_STATIC_DRAW: the data is set only once and used many times.
    //GL_DYNAMIC_DRAW: the data is changed a lot and used many times.
    //*/
    //glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //glBufferData(GL_ARRAY_BUFFER, circleVert.size() * sizeof(float), circleVert.data(), GL_STATIC_DRAW);

    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, circleInd.size() * sizeof(unsigned int), circleInd.data(), GL_STATIC_DRAW);

    //// position attribute
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    //glEnableVertexAttribArray(0);
    //// color attribute
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    //glEnableVertexAttribArray(1);


    //// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    //glBindBuffer(GL_ARRAY_BUFFER, 0);

    //// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    ////glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    //// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    //// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    //glBindVertexArray(0);


    // uncomment this call to draw in wireframe polygons.
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    int iterations = 0;

    float targetTime = FRAME_TIME;

	ifstream inputData("simulation.txt");

	simpleSystem.time = 0.0;

    chrono::high_resolution_clock::time_point lastFrameTime = chrono::high_resolution_clock::now();
    chrono::milliseconds spaceBuffer(250); 
	chrono::high_resolution_clock::time_point prevSpacePressedTime = chrono::high_resolution_clock::now();

    // shader.use();

    // render loop
    // -----------
    try {
        while (!glfwWindowShouldClose(window))
        {
            // input
            // -----
            processInput(window);

            if (KEY_E) {
                resetSystem(pressureApproximationSystem, 100);

                // Execute the loaded simulation and re-open the created simulation file
                inputData.close();
                //simpleGravitationalOrbitSim(simpleSystem, ofstream());
                pressureApproximation(pressureApproximationSystem, 100.0, ofstream());
                inputData.open("simulation.txt");

                updateSystemFromFile(pressureApproximationSystem, inputData, zero, foundTarget);

                targetTime = FRAME_TIME;

                PAUSE = true;
            }
            if (KEY_SPACE && chrono::high_resolution_clock::now() - prevSpacePressedTime > spaceBuffer) {
                PAUSE = !PAUSE;
                KEY_SPACE = false;
				prevSpacePressedTime = chrono::high_resolution_clock::now();
            }
				
            KEY_SPACE = false;
            KEY_E = false;

            // render
            // ------
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            // activate shader
            shader.use();

            // update projection matrix each frame in case user changes window size
            glm::mat4 projection = glm::mat4(1.0f);
            projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
            shader.setMat4("projection", projection);

            // view transformation
            glm::mat4 view = glm::mat4(1.0f);
            view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
            shader.setMat4("view", view);

            //glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

			// Draw bounding box before particles
			glm::mat4 model = glm::mat4(1.0f);
			shader.setMat4("model", model);
			bbMesh.draw(shader);

            for (System& subsystem : pressureApproximationSystem.subsystems) {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(subsystem.position.x, subsystem.position.y, subsystem.position.z));
                shader.setMat4("model", model);
                // draw the circle
                //glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(circleInd.size()), GL_UNSIGNED_INT, 0);
				circleMesh.draw(shader);
            }

            bool foundTargetTime = false;

            //float nextTime = (float)simpleSystem.time;

            // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
            // -------------------------------------------------------------------------------
            glfwSwapBuffers(window);
            glfwPollEvents();

            // Wait for frame time to elapse (16ms for ~60FPS)
            while (chrono::high_resolution_clock::now() - lastFrameTime < FRAME_DURATION) {
                /* wait */


				// IMPORTANT: AFTER RENDERING WE UPDATE THE SIMULATION STATE FOR THE NEXT FRAME
                // Read next time from file until we find the time that matches or exceeds the target frame time
                // Right now, if there is enough lag, this implementation will skip frames, but by preloading the files
                // like this, we should avoid stuttering due to the low processing cost
                // If issues occur, the sim can output a lower fidelity sim to reduce file size and read time
                // ***** THIS IS UNTESTED *****
                if (inputData.eof()) {
                    PAUSE = true;
                }
                else if (!foundTargetTime && !PAUSE) {
                    // This call doesn't guarantee a successful update so this name is a misnomer
                    // Instead this checks the next line for the target time and this while loop
                    // calls the function repeatadly until the target time is found
                    updateSystemFromFile(pressureApproximationSystem, inputData, targetTime, foundTargetTime); 
                }
            };

            // Swapping frame time calculation after waiting to avoid time spent in file I/O or rendering
            lastFrameTime = chrono::high_resolution_clock::now();

            iterations++;


        }
    } catch (const std::exception& e) {
        std::cerr << "An error occurred during the render loop: " << e.what() << std::endl;
	}

    inputData.close();

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
   /* glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);*/

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
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        // Execute the loaded simulation and render the created simulation file
		KEY_E = true;
	}
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        // Pause functionality can be implemented here
        KEY_SPACE = true;
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void resetSystem(System& system, int n) {
    system.time = 0;

    system.subsystems.clear();
    for (int i = 0; i < n; i++) {
		System particle;
		system.subsystems.push_back(particle);
    }
}
void updateSystemFromFile(System& system, ifstream& inputData, float& targetTime, bool& foundTargetTime) {
        string line = "";
        getline(inputData, line);

        float nextTime = system.time;

        if (system.time >= targetTime) {
            foundTargetTime = true;

            cout << "System Time: " << system.time << endl;
            cout << "Target Time: " << targetTime << endl;

            targetTime += FRAME_TIME;

            for (int i = 0; i < system.subsystems.size(); i++) {
                std::string search = std::string("p") + std::to_string(i + 1) + ",";
                size_t pPos = line.find(search);
                if (pPos == std::string::npos) continue;

                size_t start = pPos + search.length();
                size_t end = line.find(';', start);
                if (end == std::string::npos) end = line.size();

                std::string positionStr = line.substr(start, end - start);
                // remove commas to simplify parsing, or replace with spaces
                for (char& c : positionStr) if (c == ',') c = ' ';

                std::istringstream iss(positionStr);
                float x, y, z;
                if (!(iss >> x >> y >> z)) {
                    // parsing failed; log and continue
                    std::cerr << "Failed to parse position for particle " << (i + 1) << ": '" << positionStr << "'\n";
                    continue;
                }
                system.subsystems[i].position = vec3(x, y, z);
                //cout << "  Particle " << (i + 1) << " Position: (" << simpleSystem.subsystems[i].position << ")" << endl;
            }
        }
        else if (line.length() != 0) {
            size_t tPos = line.find(';');
            if (tPos != string::npos) {
                string timeStr = line.substr(1, tPos - 1);
                nextTime = stof(timeStr);
            }
        }

        system.time = nextTime;
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
        float angle = 2.0f * (float)PI * i / subdivisions;
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