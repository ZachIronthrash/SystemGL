#include "System.h"

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

void configureGLFW();

int configureWindow(GLFWwindow* window, bool& retFlag);

int loadGLAD(bool& retFlag);

void getIntWithDefault(istream& in, int& v);


void getLongDoubleWithDefault(istream& in, long double& v);

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
    // variable initialization
    // -----------------------
    int numParticles = 2000;

    cout << "Select particle count (\"-\" for default: " << numParticles << "): ";

    getIntWithDefault(cin, numParticles);

    // default to helium molar mass
    long double molarMass = 0.004003l; // [kg / mol]

    cout << "Select molar mass of particle (default: helium molar mass 0.004003): ";

    getLongDoubleWithDefault(cin, molarMass);

    long double targetTemp = 100.0l;

    cout << "Select target temperature (default: " << targetTemp << " K): ";

    getLongDoubleWithDefault(cin, targetTemp);

    long double systemMass = 0.5 * molarMass; // [kg]

    cout << "Select total system mass (default: " << systemMass << " Kg): ";

    getLongDoubleWithDefault(cin, systemMass);

    long double dt = 0.001l;

    cout << "Select simulation time step (default: " << dt << "): ";

    getLongDoubleWithDefault(cin, dt);

    vec3 boxSize = vec3(1.0l, 1.0l, 5.0l);

    cout << "Select bounding box size (\"-\" for any default values: " << boxSize << "): ";

    getLongDoubleWithDefault(cin, boxSize.x);
    getLongDoubleWithDefault(cin, boxSize.y);
    getLongDoubleWithDefault(cin, boxSize.z);

    // configure glfw
    // --------------
    configureGLFW();

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "SystemGL by Christopher Hart", NULL, NULL);

    bool retFlag;
    int retVal = configureWindow(window, retFlag); // auto-gen: probably not ideal
    if (retFlag) return retVal;

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    retVal = loadGLAD(retFlag); // auto-gen: probably not ideal
    if (retFlag) return retVal;

    // shader
    // ------
    Shader shader("C:/Users/chris/source/repos/SystemGL/SystemGL/vertex.glsl", "C:/Users/chris/source/repos/SystemGL/SystemGL/fragment.glsl");

    // circle mesh
    // -----------
    vector<float> circleVert;
    vector<unsigned int> circleInd;

    float circleRadius = 0.01f;

    circleZ(circleVert, circleInd, circleRadius, 6, { 1.0f, 0.0f, 0.0f });

    Mesh circleMesh(circleVert, circleInd);

    // init time tracking
    // ------------------
    int iterations = 0;

    float targetTime = FRAME_TIME;

    chrono::high_resolution_clock::time_point lastFrameTime = chrono::high_resolution_clock::now();
	chrono::high_resolution_clock::time_point prevSpacePressedTime = chrono::high_resolution_clock::now();

    chrono::milliseconds spaceBuffer(250);

    // init simulation
    // ---------------
    unsigned seed = (int)std::chrono::system_clock::now().time_since_epoch().count();

    mt19937 generator(seed);

    PressureSystem system(numParticles, systemMass, molarMass, targetTemp, generator, boxSize, dt);

    // calculate predicted values
    // --------------------------
    vec3 systemPos = vec3(0);
    long double massSum = 0l;
    for (Particle& particle : system.getParticles()) {
        massSum += particle.getMass();
        systemPos += particle.getPosition() * particle.getMass();
    }

    systemPos /= massSum;

    cout << "System position: " << systemPos << endl;

    vec3 systemVel = vec3(0);
    massSum = 0l;
    for (Particle& particle : system.getParticles()) {
        massSum += particle.getMass();
        systemVel += particle.getVelocity() * particle.getMass();
    }

    systemVel /= massSum;

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
    // we use this new formula because the kinetic energies of the tiny particles
    //      we are simulating are super small so the sum remains in accurate limits     
    long double temperature = (2 * systemKE * molarMass) / (3 * R_GAS * systemMass);
    cout << "System temperature: " << temperature << " vs. Target temperature: " << targetTemp << endl;

    long double molesOfGasParticles = systemMass / molarMass;

    // PV = nRT
    long double predictedPressure = molesOfGasParticles * R_GAS * temperature / (8.0l * boxSize.volume());

    cout << "Predicted pressure: " << predictedPressure << endl;

    unsigned int simIterations = 0;
    unsigned int missedFrameIterations = 0;

    // init pressure approximation arrays
    // ----------------------------------
    /*std::array<long double, 6>*/ vec3 impulse;

    /*std::array<long double, 6>*/ vec3 avgForce = { 0L, 0L, 0L };

    // window size to orthographic projection scaling factor
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

            // switch comments for projection matrix
            //projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
            projection = glm::ortho(-(float)SCR_WIDTH * s, (float)SCR_WIDTH * s, -(float)SCR_HEIGHT * s, (float)SCR_HEIGHT * s, -10.0f, 10.0f);

            shader.setMat4("projection", projection);

            // view transformation
            glm::mat4 view = glm::mat4(1.0f);

            // uncomment for projection matrix
            //view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

            shader.setMat4("view", view);

			// Draw bounding box before particles
            system.drawSystemBounds(shader);

            // draw particles
            system.drawSystemParticles(shader, circleMesh, 10);

            bool foundTargetTime = false;

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

                    double dt = system.getDt();

                    simIterations++;

                    avgForce.x += (impulse.x / dt - avgForce.x) / simIterations;
                    avgForce.y += (impulse.y / dt - avgForce.y) / simIterations;
                    avgForce.z += (impulse.z / dt - avgForce.z) / simIterations;
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
                // calcuate pressure approximation and output
                // ------------------------------------------
                vec3 approximatedPressure = {
                    avgForce.x / (4.0l * boxSize.y * boxSize.z),    
                    avgForce.y / (4.0l * boxSize.x * boxSize.z),
                    avgForce.z / (4.0l * boxSize.x * boxSize.y)
                };

                float avgApproxPressure = static_cast<float>(approximatedPressure.x);
                avgApproxPressure += static_cast<float>(approximatedPressure.y);
                avgApproxPressure += static_cast<float>(approximatedPressure.z);
                avgApproxPressure /= 6.0f; // over six not three because we collate adjacent faces
                // pressureX = |pressureTopX| + |pressureBottomX|

                cout << setfill(' ') << setw(15) << fixed << setprecision(10) << "Pressure approximation: " << avgApproxPressure;
                cout << fixed << setprecision(5) << ", % error: " << 100 * (avgApproxPressure - predictedPressure) / predictedPressure << endl;

                /*cout << fixed << setprecision(5) << approximatedPressure << endl;*/
            }


            // Swapping frame time calculation after waiting to avoid time spent in file I/O or rendering
            lastFrameTime = chrono::high_resolution_clock::now();

            iterations++; // dont think this is used rn
        }
    } catch (const std::exception& e) {
        std::cerr << "An error occurred during the render loop: " << e.what() << std::endl;
	}

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
	
	return 0;
}

void getIntWithDefault(istream& in, int& v)
{
    string str;

    getline(in, str);

    while (str.empty()) {
        cout << "Invalid entry. Enter again: ";
        getline(in, str);
    }
    {
        int i;
        stringstream ss(str);

        if (!(ss >> i)) {
            i = v;
        }

        v = i;
    }
}

void getLongDoubleWithDefault(istream& in, long double& v)
{
    string str;

    getline(in, str);

    while (str.empty()) {
        cout << "Invalid entry. Enter again: ";
        getline(in, str);
    }
    {
        long double l;
        stringstream ss(str);

        if (!(ss >> l)) {
            l = v;
        }

        v = l;
    }
}

int loadGLAD(bool& retFlag)
{
    retFlag = true;
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    retFlag = false;
    return {};
}

int configureWindow(GLFWwindow* window, bool& retFlag)
{
    retFlag = true;
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    retFlag = false;
    return {};
}

void configureGLFW()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
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

// ARCHIVAL CODE
// -------------

//void drawSystemParticles(System& system, Shader& shader, Mesh& mesh, unsigned fidelity) {
//    int i = 0;
//    for (Particle particle : system.getParticles()) {
//        if (i % fidelity == 0) {
//            glm::mat4 model = glm::mat4(1.0f);
//            vec3 particlePos = particle.getPosition();
//            model = glm::translate(model, glm::vec3(particlePos.x, particlePos.y, particlePos.z));
//            shader.setMat4("model", model);
//            mesh.draw(shader);
//        }
//        i++;
//    }
//}

// bounding box mesh
    // -------------------
  //  float bbWidth = 2.0f;
  //  float bbHeight = 2.5f;
  //  float bbDepth = 10.0f;
  //  float boundingBoxVert[] = {
        //// positions                                            // colors
  //       bbWidth / 2.0f,  bbHeight / 2.0f, -bbDepth / 2.0f,     0, 0, 0,    // Top right
  //       bbWidth / 2.0f, -bbHeight / 2.0f, -bbDepth / 2.0f,     0, 0, 0,    // Bottom right
  //      -bbWidth / 2.0f,  bbHeight / 2.0f, -bbDepth / 2.0f,     0, 0, 0,    // Top left
  //      -bbWidth / 2.0f, -bbHeight / 2.0f, -bbDepth / 2.0f,     0, 0, 0     // Bottom left
  //  };
  //  unsigned int boundingBoxInd[] = {
  //      0, 1, 2,
  //      2, 3, 1
  //  };

  //   Mesh bbMesh(boundingBoxVert, boundingBoxInd);

    // uncomment this call to draw in wireframe polygons.
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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