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

//void resetSystem(System& system, int n);
//void findParticlePositions(System& system, vector<vec3>& particlePositions, ifstream& inputData, float& targetTime, bool& foundTargetTime);
//void updateSystemFromFile(System& system, ifstream& inputData, float& targetTime, bool& foundTargetTime);
void drawSystemParticles(System& system, Shader& shader, Mesh& mesh, unsigned fidelity);
void circleZ(vector<float>& vertices, vector<unsigned int>& indices, float radius, int subdivisions, glm::vec3 color);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

chrono::milliseconds FRAME_DURATION(16); // Approx. 60 FPS
float FRAME_TIME = FRAME_DURATION.count() / 1000.0f; // in seconds

// keyboard input processing
bool KEY_E = false;
bool KEY_SPACE = false;
bool KEY_N = false;

bool PAUSE = true;

int main() {
    /*cout << "Hello, World!" << endl << endl;

    cout << "Creating a simple system..." << endl;
    
    System simpleSystem;
    resetSystem(simpleSystem, 100);

    System pressureApproximationSystem;

    ifstream data;
	data.open("simulation.txt");

    float zero = 0.0f;
    resetSystem(pressureApproximationSystem, 10);
	bool foundTarget = false;
	updateSystemFromFile(pressureApproximationSystem, data, zero, foundTarget);

	for (int i = 0; i < pressureApproximationSystem.numberOfSubsystems(); i++) {
        cout << "Particle Position: (" << pressureApproximationSystem.subsystem(i).derivePosition() << ")" << endl;
	}*/

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

    float circleRadius = 0.005f;

    circleZ(circleVert, circleInd, circleRadius, 6, { 1.0f, 0.0f, 0.0f });

    float bbWidth = 1.5f;
    float bbHeight = 1.5f;
    float bbDepth = 10.0f;
    float boundingBoxVert[] = {
		// positions                        // colors
         bbWidth / 2.0f,  bbHeight / 2.0f, -bbDepth / 2.0f,     0, 0, 0,    // Top right
         bbWidth / 2.0f, -bbHeight / 2.0f, -bbDepth / 2.0f,     0, 0, 0,    // Bottom right
        -bbWidth / 2.0f,  bbHeight / 2.0f, -bbDepth / 2.0f,     0, 0, 0,    // Top left
        -bbWidth / 2.0f, -bbHeight / 2.0f, -bbDepth / 2.0f,     0, 0, 0     // Bottom left
    };
    unsigned int boundingBoxInd[] = {
        0, 1, 2,
        2, 3, 1
    };

	Mesh circleMesh(circleVert, circleInd);
    Mesh bbMesh(boundingBoxVert, boundingBoxInd);

    // uncomment this call to draw in wireframe polygons.
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    int iterations = 0;

    float targetTime = FRAME_TIME;

    chrono::high_resolution_clock::time_point lastFrameTime = chrono::high_resolution_clock::now();
    chrono::milliseconds spaceBuffer(250); 
	chrono::high_resolution_clock::time_point prevSpacePressedTime = chrono::high_resolution_clock::now();

    int numParticles = 2000;

    unsigned seed = (int)std::chrono::system_clock::now().time_since_epoch().count();

    mt19937 generator(seed);

    long double heliumMolarMass = 0.004003l; // [kg / mol]

    long double targetTemp = 10.0l;
    
    // using helium mass
    long double systemMass = 0.5 * heliumMolarMass; // [kg]

    long double bbHalfWidth = bbWidth / 2.0l;
    long double bbHalfHeight = bbHeight / 2.0l;
    long double bbHalfDepth = bbDepth / 2.0l;

    PressureSystem system(numParticles, systemMass, heliumMolarMass, targetTemp, generator, new long double[3]{ bbHalfWidth, bbHalfHeight, bbHalfDepth});

    vec3 systemPos = vec3(0);
    long double massSum = 0l;
    for (Particle& particle : system.getParticles()) {
        massSum += particle.getMass();
        systemPos += particle.getPosition() * particle.getMass();
    }

    systemPos /= massSum;

    cout << "System position: " << systemPos << endl;

    //ofstream velocityData("velocity.txt");

    vec3 systemVel = vec3(0);
    massSum = 0l;
    for (Particle& particle : system.getParticles()) {
        massSum += particle.getMass();
        systemVel += particle.getVelocity() * particle.getMass();
    }

    systemVel /= massSum;

    //velocityData.close();

    cout << "System velocity: " << systemVel << endl;

    long double systemKE = 0l; // this calculation will be within a few orders of magnitude of 0
                               //  my source is an extremely small amount of desmos graphing so take it with salt
    
    for (Particle& particle : system.getParticles()) {
        systemKE += particle.calcKE();
    }

    // the traditional formula relating temperature and kinetic energy is:
    //      <KE> = (3/2) * BOLTZMANN * TEMPERATURE
    //      where <KE> = sum(KE) / NUMBER_OF_PARTICLES
    //      and NUMBER_OF_PARTICLES is the number of gas molecules being modeled
    //      not the number of particles being simulated
    // an equivalent formula can be obtained for the systemKE:
    //      sum(KE) = (3 * SYSTEM_MASS * R_GAS * TEMPERATURE) / (2 * MOLAR_MASS)
    //      from the definitions of the above equations:
    //          NUMBER_OF_PARTICLES = SYSTEM_MASS * MOLE / MOLAR_MASS
    //          BOLTZMANN = R_GAS / MOLE
    //      
    long double temperature = (2 * systemKE * heliumMolarMass) / (3 * R_GAS * systemMass);
    cout << "System temperature: " << temperature << " vs. Target temperature: " << targetTemp << endl;

    long double numberOfGasParticles = systemMass / heliumMolarMass;

    // PV = nRT
    long double predictedPressure = numberOfGasParticles * R_GAS * temperature / (bbWidth * bbHeight * bbWidth);

    cout << "Predicted pressure: " << predictedPressure << endl;

 //   srand(unsigned int(time(0))); // Seed random number generator

 //   BoundSystem basicSystem;

 //   for (int i = 0; i < numParticles; i++) {
 //       vec3 randomPos(
 //           ((double)rand() / RAND_MAX) * 2.0f - 1.0f,
 //           ((double)rand() / RAND_MAX) * 2.0f - 1.0f,
 //           0 /*((double)rand() / RAND_MAX) * 2.0f - 1.0f*/
 //       );
 //       vec3 randomVel(
 //           ((double)rand() / RAND_MAX) * 2.0f - 1.0f,
 //           ((double)rand() / RAND_MAX) * 2.0f - 1.0f,
 //           0 /*((double)rand() / RAND_MAX) * 2.0f - 1.0f*/
 //       );

	//	Particle particle(randomPos, randomVel, 1.0);
 //       basicSystem.addParticle(particle);
 //   }

 //   // Create a bound subsystem and add it to the basic system.
 //   // NOTE: BoundSystem (and System) is non-copyable because it contains std::unique_ptr members.
 //   // Construct the subsystem directly as a unique_ptr and populate it, then move it into the parent.
 //   auto boundSubsystemPtr = std::make_unique<BoundSystem>();
 //   boundSubsystemPtr->addParticle(Particle(vec3(0, 0, 0), vec3(1, 0, 0), 1.0));
 //   boundSubsystemPtr->addParticle(Particle(vec3(0, 0, 0), vec3(-1, 0, 0), 1.0));

 //   basicSystem.addSubsystem(std::move(boundSubsystemPtr));

 //   /*cout << basicSystem.getSubsystem(0).getParticle(0).getPosition() << endl;
 //   cout << basicSystem.getSubsystem(0).getParticle(1).getPosition() << endl;*/

	//Simulation simulation(basicSystem, "systemData.txt", "simulationTest.txt");

    unsigned int simIterations = 0;
    unsigned int missedFrameIterations = 0;

    // std::array<unsigned int, 6> impulseCounts = { 0, 0, 0, 0, 0, 0 };

    std::array<long double, 6> impulse;

    std::array<long double, 6> avgForce = { 0L, 0L, 0L, 0L, 0L, 0L };

    float s = 0.002f;

    // render loop
    // -----------
    try {
        while (!glfwWindowShouldClose(window))
        {
            // input
            // -----

            if (KEY_N) {
                KEY_N = false;
            }
            processInput(window);

            //if (KEY_E) {
            //    //resetSystem(pressureApproximationSystem, 10);

            //    //// Execute the loaded simulation and re-open the created simulation file
            //    //inputData.close();
            //    ////simpleGravitationalOrbitSim(simpleSystem, ofstream());
            //    //pressureApproximation(pressureApproximationSystem, 10.0, ofstream());
            //    //inputData.open("simulation.txt");

            //    //updateSystemFromFile(pressureApproximationSystem, inputData, zero, foundTarget);


            //    targetTime = FRAME_TIME;

            //    PAUSE = true;
            //}
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
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // activate shader
            shader.use();

            // update projection matrix each frame in case user changes window size
            glm::mat4 projection = glm::mat4(1.0f);
            //projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
            projection = glm::ortho(-(float)SCR_WIDTH * s, (float)SCR_WIDTH * s, -(float)SCR_HEIGHT * s, (float)SCR_HEIGHT * s, -10.0f, 10.0f);
            shader.setMat4("projection", projection);

            // view transformation
            glm::mat4 view = glm::mat4(1.0f);
            //view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
            shader.setMat4("view", view);

            //glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

			// Draw bounding box before particles
			glm::mat4 model = glm::mat4(1.0f);
			shader.setMat4("model", model);
			bbMesh.draw(shader);

            drawSystemParticles(system, shader, circleMesh, 10);

    //        for (int i = 0; i < pressureApproximationSystem.numberOfSubsystems(); i++) {
    //            glm::mat4 model = glm::mat4(1.0f);
				//vec3 subsystemPos = pressureApproximationSystem.subsystem(i).derivePosition();
    //            model = glm::translate(model, glm::vec3(subsystemPos.x, subsystemPos.y, subsystemPos.z));
    //            shader.setMat4("model", model);
    //            // draw the circle
    //            //glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(circleInd.size()), GL_UNSIGNED_INT, 0);
				//circleMesh.draw(shader);
    //        }

            bool foundTargetTime = false;

            //float nextTime = (float)simpleSystem.time;

            // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
            // -------------------------------------------------------------------------------
            glfwSwapBuffers(window);
            glfwPollEvents();

            // Wait for frame time to elapse (16ms for ~60FPS)
            while (chrono::high_resolution_clock::now() - lastFrameTime < FRAME_DURATION) {
                /* wait */

				// Iteratively evolve the system until we reach the target time for this frame
				// Temp solution until better file I/O or in-memory simulation data management is implemented
                if (system.getTime() < targetTime && (!PAUSE || KEY_N)) {
                    //system.evolve();
                    impulse = system.impulseEvolve(); 

                   /* double dt = system.getDt();
                    if (impulse[0] != 0.0) { avgForce[0] += (impulse[0] / dt - avgForce[0]) / ++impulseCounts[0]; }
                    if (impulse[1] != 0.0) { avgForce[1] += (impulse[1] / dt - avgForce[1]) / ++impulseCounts[1]; }
                    if (impulse[2] != 0.0) { avgForce[2] += (impulse[2] / dt - avgForce[2]) / ++impulseCounts[2]; }
                    if (impulse[3] != 0.0) { avgForce[3] += (impulse[3] / dt - avgForce[3]) / ++impulseCounts[3]; }
                    if (impulse[4] != 0.0) { avgForce[4] += (impulse[4] / dt - avgForce[4]) / ++impulseCounts[4]; }
                    if (impulse[5] != 0.0) { avgForce[5] += (impulse[5] / dt - avgForce[5]) / ++impulseCounts[5]; }*/

                    double dt = system.getDt();

                    if (simIterations != 0) {
                        avgForce[0] += (impulse[0] / dt - avgForce[0]) / simIterations;
                        avgForce[1] += (impulse[1] / dt - avgForce[1]) / simIterations;
                        avgForce[2] += (impulse[2] / dt - avgForce[2]) / simIterations;
                        avgForce[3] += (impulse[3] / dt - avgForce[3]) / simIterations;
                        avgForce[4] += (impulse[4] / dt - avgForce[4]) / simIterations;
                        avgForce[5] += (impulse[5] / dt - avgForce[5]) / simIterations;
                    }
                    else {
                        avgForce[0] = impulse[0] / dt;
                        avgForce[1] = impulse[1] / dt;
                        avgForce[2] = impulse[2] / dt;
                        avgForce[3] = impulse[3] / dt;
                        avgForce[4] = impulse[4] / dt;
                        avgForce[5] = impulse[5] / dt;
                    }

                    /*cout << avgForce[0] << endl;
                    cout << avgForce[1] << endl;
                    cout << avgForce[2] << endl;
                    cout << avgForce[3] << endl;
                    cout << avgForce[4] << endl;
                    cout << avgForce[5] << endl;*/

                    simIterations++;

                   /* cout << "Impulses;" << endl;
                    cout << "  +x: " << impulse[0] << endl;
                    cout << "  -x: " << impulse[1] << endl;
                    cout << "  +y: " << impulse[2] << endl;
                    cout << "  -y: " << impulse[3] << endl;*/
                }
            };

            if (system.getTime() >= targetTime) {

                //cout << setfill(' ') << setw(15) << fixed << setprecision(10) << "Average force: " << avgForce[0] << ", " << avgForce[1] << ", " << avgForce[2] << ", " << avgForce[3] << ", " << avgForce[4] << ", " << avgForce[5] << endl;
                
                
                /*systemPos = vec3(0);
                massSum = 0;
                for (Particle& particle : system.getParticles()) {
                    massSum += particle.getMass();
                    systemPos += (particle.getPosition() - systemPos) / massSum;
                }
                cout << setw(20) << fixed << setprecision(5) << "System position: " << systemPos << endl;*/

                targetTime += FRAME_TIME;
                //cout << basicSystem.getTime() << endl;
            }
            else if (!PAUSE || KEY_N) {
                cout << "** Skipped iteration at time " << targetTime << " seconds" << endl;
                cout << "     Iterations skipped: " << ++missedFrameIterations << endl;
            }
            if (!PAUSE) {
                // replace 1/4 with proper area calculation in the future
                //double approximatedPressure = 0.25 * (1.0/6.0) * (avgForce[0] + avgForce[1] + avgForce[2] + avgForce[3] + avgForce[4] + avgForce[5]);

                array<float, 6> approximatedPressure = {
                    static_cast<float>(avgForce[0]) / (bbHeight * bbDepth),
                    static_cast<float>(avgForce[1]) / (bbHeight * bbDepth),
                    static_cast<float>(avgForce[2]) / (bbWidth * bbDepth),
                    static_cast<float>(avgForce[3]) / (bbWidth * bbDepth),
                    static_cast<float>(avgForce[4]) / (bbHeight * bbWidth),
                    static_cast<float>(avgForce[5]) / (bbHeight * bbWidth)
                };

                float avgApproxPressure = approximatedPressure[0];
                avgApproxPressure += approximatedPressure[1];
                avgApproxPressure += approximatedPressure[2];
                avgApproxPressure += approximatedPressure[3];
                avgApproxPressure += approximatedPressure[4];
                avgApproxPressure += approximatedPressure[5];
                avgApproxPressure /= 6.0f;
                //avgApproxPressure = 0.25 * (1.0 / 6.0) * (avgForce[0] + avgForce[1] + avgForce[2] + avgForce[3] + avgForce[4] + avgForce[5]);

                // TODO:
                // why are the pressures different
                // and why is the approximation only wrong when not a cube?
                // despite each of the pressures being roughly the same when not a cube?
                cout << approximatedPressure[0] << endl;
                cout << approximatedPressure[1] << endl;
                cout << approximatedPressure[2] << endl;
                cout << approximatedPressure[3] << endl;
                cout << approximatedPressure[4] << endl;
                cout << approximatedPressure[5] << endl;
                cout << setfill(' ') << setw(15) << fixed << setprecision(10) << "Pressure approximation: " << avgApproxPressure;
                cout << fixed << setprecision(5) << ", % error: " << 100 * (avgApproxPressure - predictedPressure) / predictedPressure << endl;
            }


            // Swapping frame time calculation after waiting to avoid time spent in file I/O or rendering
            lastFrameTime = chrono::high_resolution_clock::now();

            iterations++;


        }
    } catch (const std::exception& e) {
        std::cerr << "An error occurred during the render loop: " << e.what() << std::endl;
	}

    //inputData.close();

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
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
        KEY_N = true;
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

void drawSystemParticles(System& system, Shader& shader, Mesh& mesh, unsigned fidelity) {
    int i = 0;
    for (Particle particle : system.getParticles()) {
        if (i % fidelity == 0) {
            glm::mat4 model = glm::mat4(1.0f);
            vec3 particlePos = particle.getPosition();
            model = glm::translate(model, glm::vec3(particlePos.x, particlePos.y, particlePos.z));
            shader.setMat4("model", model);
            mesh.draw(shader);
        }
        i++;
    }
    //for (size_t i = 0; i < system.numberOfSubsystems(); i++) {
    //    drawSystemParticles(*system.getSubsystem(i), shader, mesh);
    //}

    //        for (int i = 0; i < pressureApproximationSystem.numberOfSubsystems(); i++) {
    //            glm::mat4 model = glm::mat4(1.0f);
                //vec3 subsystemPos = pressureApproximationSystem.subsystem(i).derivePosition();
    //            model = glm::translate(model, glm::vec3(subsystemPos.x, subsystemPos.y, subsystemPos.z));
    //            shader.setMat4("model", model);
    //            // draw the circle
    //            //glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(circleInd.size()), GL_UNSIGNED_INT, 0);
                //circleMesh.draw(shader);
    //        }
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

//void resetSystem(System& system, int n) {
//    system.setTime(0.0);
//
//	system.clearSubsystems();
//    for (int i = 0; i < n; i++) {
//		System particle;
//		system.addSubsystem(particle);
//    }
//}
//void findParticlePositions(System& system, vector<vec3>& particlePositions, ifstream& inputData, float& targetTime, bool& foundTargetTime) {
//    string line = "";
//
//	// Clear previous positions and resize to number of particles
//    particlePositions.clear();
//	for (int i = 0; i < system.numberOfParticles(); i++) {
//        particlePositions.push_back(vec3(0.0f));
//    }
//
//	getline(inputData, line);
//    float nextTime = system.getTime();
//    if (system.getTime() >= targetTime) {
//        foundTargetTime = true;
//        cout << "System Time: " << system.getTime() << endl;
//        cout << "Target Time: " << targetTime << endl;
//        targetTime += FRAME_TIME;
//        int particleIndex = 0;
//        for (int i = 0; i < system.numberOfSubsystems(); i++) {
//            std::string search = std::string("p") + std::to_string(i + 1) + ",";
//            size_t pPos = line.find(search);
//            if (pPos == std::string::npos) continue;
//            size_t start = pPos + search.length();
//            size_t end = line.find(';', start);
//            if (end == std::string::npos) end = line.size();
//            std::string positionStr = line.substr(start, end - start);
//            // remove commas to simplify parsing, or replace with spaces
//            for (char& c : positionStr) if (c == ',') c = ' ';
//            std::istringstream iss(positionStr);
//            float x, y, z;
//            if (!(iss >> x >> y >> z)) {
//                // parsing failed; log and continue
//                std::cerr << "Failed to parse position for particle " << (i + 1) << ": '" << positionStr << "'\n";
//                continue;
//            }
//            if (system.subsystem(i).isParticle()) {
//                particlePositions[particleIndex] = vec3(x, y, z);
//                particleIndex++;
//            }
//            //cout << "  Particle " << (i + 1) << " Position: (" << simpleSystem.subsystems[i].position << ")" << endl;
//        }
//    }
//    else if (line.length() != 0) {
//        size_t tPos = line.find(';');
//        if (tPos != string::npos) {
//            string timeStr = line.substr(1, tPos - 1);
//            nextTime = stof(timeStr);
//        }
//    }
//    system.setTime(nextTime);
//}
//void updateParticlePositions(System& system, vector<vec3>& particlePositions, string line, int& particleIndex) {
//    for (int i = particleIndex; i < system.numberOfSubsystems(); i++) {
//        std::string search = std::string("p") + std::to_string(i + 1) + ",";
//        size_t pPos = line.find(search);
//        if (pPos == std::string::npos) continue;
//        size_t start = pPos + search.length();
//        size_t end = line.find(';', start); 
//        if (end == std::string::npos) end = line.size();
//        std::string positionStr = line.substr(start, end - start);
//        // remove commas to simplify parsing, or replace with spaces
//        for (char& c : positionStr) if (c == ',') c = ' ';
//        std::istringstream iss(positionStr);
//        float x, y, z;
//        if (!(iss >> x >> y >> z)) {
//            // parsing failed; log and continue
//            std::cerr << "Failed to parse position for particle " << (i + 1) << ": '" << positionStr << "'\n";
//            continue;
//        }
//        if (system.subsystem(i).isParticle()) {
//            particlePositions[particleIndex] = vec3(x, y, z);
//            particleIndex++;
//        }
//        else {
//			updateParticlePositions(system.subsystem(i), particlePositions, line, particleIndex);
//        }
//        //cout << "  Particle " << (i + 1) << " Position: (" << simpleSystem.subsystems[i].position << ")" << endl;
//    }
//}
//void updateSystemFromFile(System& system, ifstream& inputData, float& targetTime, bool& foundTargetTime) {
//        string line = "";
//        getline(inputData, line);
//
//        float nextTime = system.getTime();
//
//        if (system.getTime() >= targetTime) {
//            foundTargetTime = true;
//
//            cout << "System Time: " << system.getTime() << endl;
//            cout << "Target Time: " << targetTime << endl;
//
//            targetTime += FRAME_TIME;
//
//            for (int i = 0; i < system.numberOfSubsystems(); i++) {
//                std::string search = std::string("p") + std::to_string(i + 1) + ",";
//                size_t pPos = line.find(search);
//                if (pPos == std::string::npos) continue;
//
//                size_t start = pPos + search.length();
//                size_t end = line.find(';', start);
//                if (end == std::string::npos) end = line.size();
//
//                std::string positionStr = line.substr(start, end - start);
//                // remove commas to simplify parsing, or replace with spaces
//                for (char& c : positionStr) if (c == ',') c = ' ';
//
//                std::istringstream iss(positionStr);
//                float x, y, z;
//                if (!(iss >> x >> y >> z)) {
//                    // parsing failed; log and continue
//                    std::cerr << "Failed to parse position for particle " << (i + 1) << ": '" << positionStr << "'\n";
//                    continue;
//                }
//                if (system.subsystem(i).isParticle()) {
//                    system.subsystem(i).translateTo(vec3(x, y, z));
//                }
//                //cout << "  Particle " << (i + 1) << " Position: (" << simpleSystem.subsystems[i].position << ")" << endl;
//            }
//        }
//        else if (line.length() != 0) {
//            size_t tPos = line.find(';');
//            if (tPos != string::npos) {
//                string timeStr = line.substr(1, tPos - 1);
//                nextTime = stof(timeStr);
//            }
//        }
//
//        system.setTime(nextTime);
//}