#include "../SystemGLCore/include/Camera.h"
#include "../SystemGLCore/include/Mesh.h"
#include "../SystemGLCore/include/Shader.h"
#include "../SystemGLCore/include/StateIO.h"
#include "../SystemGLCore/include/SystemGLMath.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <exception>
#include <functional>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>
#include <iostream>
#include <istream>
#include <limits>
#include <ostream>
#include <Particle.h>
#include <random>
#include <Simulation.h>
#include <sstream>
#include <string>
#include <System.h>
#include <tuple>
#include <type_traits>
#include <vector>

/* TODO:
*  - Refine model for extended stability (should not need damping for stability, but it is currently necessary for larger systems, otherwise energy explodes)
*		- Categorize the degree of instability by summing the energies from the speed and potentials of each particle, and comparing to the initial energy of the system.
*		- Verify that reflection algorithm conserves energy and momentum, and that it is implemented correctly (currently using a simple reflection algorithm which breaks down in the high velocity limit, and may not be conserving energy correctly)
*		- Implement a more stable integration method (currently using Euler's method, which is not very stable for larger systems)
*			- Research different integration methods and their stability properties, and implement one that is more stable for larger systems (e.g. Verlet integration, Runge-Kutta methods, etc.)
*			- Look into differential equation solvers which I believe should constrain simple motion like SHOs to their expected trajectories, which should help with stability.
*  ~ Sort meshes by distance from camera and draw in order
*		x Mesh drawing class for holding meshes and their draw order
*			x Sorted vector of meshes
*			x Insertion handling based on mesh pos and camera pos
*			x Drawing logic which draws in the order of the sorted vector
*				^ NOTE: distance sorting doesn't work for inverted meshes
*				^ UPDATE: inverted meshes will be drawn before non-inverted meshes, maybe with their own sorting scheme, but this is a bit of an edge case and can be handled later if it becomes an issue
*					(ideally the only inverted mesh is the box representing the bounds of the system)
*			~ Shader logic for drawing meshes 
*				(ideally every mesh would use the same shader, but this is highly unlikely if I want any style flexibility, 
*				so the shader should probably be stored in the mesh drawing class as well. The issue is that .use(...) and
*				pushing uniforms might be unfeasable for each mesh)
*		x Create mesh drawing object with mesh and camera positions in main loop scope
* - Standardize mesh implementation
*		- Refactor mesh sorting to match existing schemes in literature (check learnopengl.com and other resources for best practices)
*		- Standardize mesh class
*			~ Vertexes (should probably use a vertex struct)
*			~ Normals (could re-use vertex struct)
*			- Textures
*			- Color/transparency or other unforseen needs
*		- Loading meshes from file
*		- Outputting meshes to file
*  ~ UI elements for setting system parameters
*		x Container for system parameters
*		x I/O system for parameters
*		- UI system
*		- UI-parameter integration (switching between a scene for setting parameters, and one for playing simulations)
*  x Camera controls
*		x Up/Down controls
*		x Everything else
*  x Render-to-simulation time scaling as well as space scaling
*/

using namespace std;

void configureGLFW();

int configureWindow(GLFWwindow* window, bool& retFlag);

int loadGLAD(bool& retFlag);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

void centerMouse(GLFWwindow* window, int windowWidth, int windowHeight);

void getUnsignedWithDefault(istream& in, unsigned& v);

void getLongDoubleWithDefault(istream& in, long double& v);

// void drawSystemBounds(Mesh boxMesh, Shader& shader, vec3 offset);
void drawSystemBounds(Shader& objectShader, Shader& lightingShader, Mesh boxMesh, vec3 offset);

//void createVertexAndIndexData(vec3 boxSize, std::vector<float>& vertices, std::vector<unsigned int>& indices);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = false;
float yaw = -90.0f;	// YAW is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
//float lastX = (float)SCR_WIDTH / 2.0f;
//float lastY = (float)SCR_HEIGHT / 2.0f;
//float fov = 90.0f;

float baseCameraSpeed = 1.0f;
float baseFOV = 45.0f;

Camera camera(cameraPos, cameraUp, yaw, pitch);

chrono::milliseconds FRAME_DURATION(16); // Approx. 60 FPS
float FRAME_TIME = FRAME_DURATION.count() / 1000.0f; // in seconds

// keyboard input processing
//bool KEY_E = false;
bool KEY_SPACE = false;
//bool KEY_N = false;

bool PAUSE = true;

unsigned seed = (int)std::chrono::system_clock::now().time_since_epoch().count();

mt19937 generator(seed);

class MeshSorter {
public:
	MeshSorter(Shader& s, Camera& c) : shader(s), camera(c) {};

	void add(Mesh& mesh, Shader& shader, glm::vec3 meshPos, glm::vec3 meshColor) {
		meshes.push_back(std::make_tuple(std::reference_wrapper<Mesh>(mesh), 0.0f, meshPos, meshColor));
	}

	void sort(glm::vec3 cameraPos, std::function<float(float, float)> op = std::greater<float>()) {
		for (size_t i = 0; i < meshes.size(); i++) {
			//std::get<2>(meshes[i]) = glm::distance(std::get<1>(meshes[i]), cameraPos);
			fetchDistance(i) = glm::distance(fetchPos(i), cameraPos);
			//std::cout << "Mesh " << i << " distance from camera: " << fetchDistance(i) << std::endl;
		}
		std::sort(meshes.begin(), meshes.end(), MeshDistanceComparator(op));
	}

	// TWO POSSIBILITIES IN THE FUTURE
	//  1. save transformations in tuple (scale and rotation), and apply to model matrix
	//		upsides			-> for repeated meshes, transformations don't need new references
	//		downsides		-> consequences from increased vector entry size (large tuple reads take longer?)
	//  2. modify model with scale and rotation before passing to add(...)
	//		upsides			-> no slowdown from larger tuple size
	//		downsides		-> many, many new mesh references for each unique pair of scaling & rotation
	void draw(float aspectRatio) {
		for (size_t i = 0; i < meshes.size(); i++) {
			glm::mat4 model = glm::mat4(1.0f);
			glm::vec3 position = fetchPos(i);
			model = glm::translate(model, position);
			this->shader.setVec3("objectColor", fetchColor(i));
			this->shader.setMat4("model", model);
			fetchMesh(i).draw(shader);
		}
	}

private:
	std::vector<std::tuple<std::reference_wrapper<Mesh>, float, glm::vec3, glm::vec3>> meshes; // tuple of mesh reference, distance from camera, position, and color
	Shader& shader;
	Camera& camera;

	Mesh& fetchMesh(size_t index) {
		return std::get<0>(meshes[index]);
	}
	glm::vec3& fetchPos(size_t index) {
		return std::get<2>(meshes[index]);
	}
	glm::vec3& fetchColor(size_t index) {
		return std::get<3>(meshes[index]);
	}
	float& fetchDistance(size_t index) {
		return std::get<1>(meshes[index]);
	}

	class MeshDistanceComparator {
	public:
		// template <typename comparisonFunction> <- could use this instead of std::function
		MeshDistanceComparator(std::function<float(float, float)> op = std::greater<float>()) : op(op) {}
		bool operator()(const std::tuple<std::reference_wrapper<Mesh>, float, glm::vec3, glm::vec3>& a, const std::tuple<std::reference_wrapper<Mesh>, float, glm::vec3, glm::vec3>& b) {
			float distA = std::get<1>(a); // distance from camera is pre-computed and stored in the tuple for efficiency
			float distB = std::get<1>(b);
			return op(distA, distB); // Sort according to supplied operator (greater than for back-to-front, less than for front-to-back)
		}

	private:
		std::function<float(float, float)> op; // deafults to > in constructor: equates to furthest objects first, which is what we want for transparency rendering
	};
};

int main() {
	// variable initialization
	// -----------------------
	long double renderTimeScale = 1.0l;
	long double renderSpaceScale = 1.0l;

	unsigned numParticles = (unsigned)pow(4, 3);

	long double pointSeparation = 0.1l;
	long double connectionThreshold = 1.01l * pointSeparation * sqrt(3);

	long double springConstant = 500.0l;
	long double dampingConstant = 0.0001l;

	vec3 gravitationalAcceleration = vec3(0.0l, -10.0l, 0.0l);

	long double dt = 1e-4l;

	vec3 boxSize = vec3(1.0l, 1.0l, 1.0l) / renderSpaceScale;

	long double renderDuration = 15.0l; // [s]

	bool runSim = true;

	string input;

	cout << "Run simulation? (Y/N): ";
	while (getline(cin, input) && input != "Y" && input != "N") {
		cout << "invalid input.\n";
	}

	if (input == "N") {
		runSim = false;
		cout << "Loading last simulation...\n";
	}
	input = "";

	bool usePrevious = true;

	if (runSim == true) {
		cout << "Load previous parameters? (Y/N): ";
		while (getline(cin, input) && input != "Y" && input != "N") {
			cout << "invalid input.\n";
		}

		if (input == "N") {
			usePrevious = false;
		}
	}

	StateIO stateFile("data/simulation_states.txt");

	if (runSim && !usePrevious) {
		cout << "Render defaults to " << renderTimeScale << "x speed and " << renderSpaceScale << "x \"zoom\"" << endl;
		cout << "  (1 sec render time = " << 1.0l / renderTimeScale << " sec simulation time)" << endl;
		cout << "  (1 unit render distance = " << 1.0l / renderSpaceScale << " m sim distance)" << endl;
		cout << endl << "To skip prompts and use defaults type \"SKIP\": ";

		string input;
		getline(cin, input);

		if (input == "SKIP") {
			cout << "Using default parameters.";
		}
		else {
			cout << "Select render time scale (\"-\" for default): ";
			getLongDoubleWithDefault(cin, renderTimeScale);

			cout << "Select render space scale (\"-\" for default): ";
			getLongDoubleWithDefault(cin, renderSpaceScale);

			cout << "Select particle count (default: " << numParticles << "): ";
			getUnsignedWithDefault(cin, numParticles);

			cout << "Select the separation of the points (default: " << pointSeparation << "): ";
			getLongDoubleWithDefault(cin, pointSeparation);

			cout << "Select the separation of the connections (default: " << connectionThreshold << "): ";
			getLongDoubleWithDefault(cin, connectionThreshold);

			cout << "Select the strength of the springs (default: " << springConstant << "): ";
			getLongDoubleWithDefault(cin, springConstant);

			cout << "Select the strength of the damping (default: " << dampingConstant << "): ";
			getLongDoubleWithDefault(cin, dampingConstant);

			cout << "Select simulation time step (default: " << dt << "): ";
			getLongDoubleWithDefault(cin, dt);

			cout << "Select the direction and strength of gravity (default: " << gravitationalAcceleration << "\n    \"-\" for any defaults, enter each component on separate lines): ";
			getLongDoubleWithDefault(cin, gravitationalAcceleration.x);
			getLongDoubleWithDefault(cin, gravitationalAcceleration.y);
			getLongDoubleWithDefault(cin, gravitationalAcceleration.z);

			cout << "Select bounding box size (\"-\" for any default values, enter each component on separate lines: " << boxSize << "): ";
			getLongDoubleWithDefault(cin, boxSize.x);
			getLongDoubleWithDefault(cin, boxSize.y);
			getLongDoubleWithDefault(cin, boxSize.z);
		}
		cout << endl;

		cout << "Select render duration in seconds (\"-\" for default: " << renderDuration << "s): ";
		getLongDoubleWithDefault(cin, renderDuration);

		stateFile.states.add("render_time_scale", renderTimeScale);
		stateFile.states.add("render_space_scale", renderSpaceScale);
		stateFile.states.add("particle_number", numParticles);
		stateFile.states.add("point_separation", pointSeparation);
		stateFile.states.add("connection_threshold", connectionThreshold);
		stateFile.states.add("spring_constant", springConstant);
		stateFile.states.add("damping_constant", dampingConstant);
		stateFile.states.add("gravitational_acceleration", gravitationalAcceleration);
		stateFile.states.add("delta_time", dt);
		stateFile.states.add("box_size", boxSize);
		stateFile.states.add("render_duration", renderDuration);
		stateFile.outputStatesToFile();
	}
	else if (usePrevious) {
		stateFile.readStatesFromFile();
		renderTimeScale = stateFile.states.fetchLD("render_time_scale");
		renderSpaceScale = stateFile.states.fetchLD("render_space_scale");
		numParticles = stateFile.states.fetchU("particle_number");
		pointSeparation = stateFile.states.fetchLD("point_separation");
		connectionThreshold = stateFile.states.fetchLD("connection_threshold");
		springConstant = stateFile.states.fetchLD("spring_constant");
		dampingConstant = stateFile.states.fetchLD("damping_constant");
		gravitationalAcceleration = stateFile.states.fetchV("gravitational_acceleration");
		dt = stateFile.states.fetchLD("delta_time");
		boxSize = stateFile.states.fetchV("box_size");
		renderDuration = stateFile.states.fetchLD("render_duration");
	}

	// init simulation
	// ---------------
	SoftBoxInBox system(numParticles, pointSeparation, connectionThreshold, springConstant, dampingConstant, 0.1l, gravitationalAcceleration, boxSize, dt);

	for (Particle& p : system.getParticles()) {
		p.translateBy(vec3(0, -boxSize.y * 0.0l, 0));
	}

	Simulation simulation(system, "data/softbody_simulation.txt", renderTimeScale, renderSpaceScale);

	if (runSim) {
		simulation.run((long double)FRAME_TIME, renderDuration, std::cout);
	}

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
	//Shader shader("vertex.glsl", "fragment.glsl");
	Shader lightingShader("colors.vs", "colors.fs");
	Shader lightCubeShader("light_cube.vs", "light_cube.fs");

	// sphere mesh
	// -----------
	float sphereRadius = 0.01f;

	unsigned stacks = 16;
	unsigned slices = 32;

	unsigned subdivisions = 2;

	//Mesh sphereMesh = sphereStack_n_Slice(sphereRadius, stacks, slices);
	Mesh sphereMesh = icosphere(sphereRadius, subdivisions);

	// box mesh
	// --------
	Mesh boxMesh = invertedBoxNoTop(2.0l * renderSpaceScale * boxSize);

	// cube mesh
	// ---------
	Mesh cubeMesh = cube(1.0f);

	// light mesh
	// ----------
	// USES CUBEMESH

	// enable gl settings (TODO: pull out into a method)

	//glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);

	camera.MovementSpeed = baseCameraSpeed;
	camera.Zoom = baseFOV;

	glfwMakeContextCurrent(window);
	glfwMaximizeWindow(window);

	// init time tracking
	// ------------------
	//int iterations = 0;

	float targetTime = FRAME_TIME;

	chrono::high_resolution_clock::time_point lastFrameTime = chrono::high_resolution_clock::now();
	chrono::high_resolution_clock::time_point prevSpacePressedTime = chrono::high_resolution_clock::now();

	chrono::high_resolution_clock::time_point totalRenderTime = chrono::high_resolution_clock::now();

	chrono::milliseconds spaceBuffer(250);

	//unsigned int simIterations = 0;
	unsigned int missedFrameIterations = 0;

	unsigned int renderIterations = simulation.readNext();
	unsigned int targetRenderIteration = 1;

	// render loop
	// -----------
	try {
		while (!glfwWindowShouldClose(window))
		{
			// update time
			lastFrameTime = chrono::high_resolution_clock::now();

			// input
			// -----
			/*if (KEY_N) {
				KEY_N = false;
			}*/

			if (KEY_SPACE && chrono::high_resolution_clock::now() - prevSpacePressedTime > spaceBuffer) {
				if (PAUSE == true) {
					totalRenderTime = chrono::high_resolution_clock::now();
				}

				PAUSE = !PAUSE;
				KEY_SPACE = false;
				prevSpacePressedTime = chrono::high_resolution_clock::now();
			}

			KEY_SPACE = false;
			//KEY_E = false;

			glfwGetWindowSize(window, (int*)&SCR_WIDTH, (int*)&SCR_HEIGHT);
			processInput(window);
			centerMouse(window, SCR_WIDTH, SCR_HEIGHT);
			camera.BalanceMovementSpeedWithZoom();
			camera.BalanceSensitivityWithZoom();

			// render
			// ------
			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glm::vec3 lightPos = { 0.0f, 1.5f, 0.0f };
			
			lightingShader.use();
			//lightCubeShader.use();
			lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
			lightingShader.setVec3("lightPos", lightPos);
			lightingShader.setVec3("viewPos", camera.Position);

			lightingShader.setMat4("projection", camera.GetProjectionMatrix((float)SCR_WIDTH / (float)SCR_HEIGHT));
			lightingShader.setMat4("view", camera.GetViewMatrix());

			// Draw bounding box before anything else b/c it is inverted
			lightingShader.setVec3("objectColor", 0.1f, 0.1f, 0.1f);
			drawSystemBounds(lightingShader, lightCubeShader, boxMesh, vec3(0));

			// Lamp should be a part of the sort unlike the bounding box
			// but since the lamp uses a difference shader, I'm hesitant to add it to the sorter because I don't know the runtime of .use() and setMat4() for the shaders, and it may cause performance issues to call those for each entry in the sorter
			// really need to look into whether that's an issue or not, and if it is, look into ways to optimize shader switching (if possible)
			lightCubeShader.use();
			lightCubeShader.setMat4("projection", camera.GetProjectionMatrix((float)SCR_WIDTH / (float)SCR_HEIGHT));
			lightCubeShader.setMat4("view", camera.GetViewMatrix());
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, lightPos);
			model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
			lightCubeShader.setMat4("model", model);
			cubeMesh.draw(lightCubeShader);

			lightingShader.use();

			// set shader Mat4's
			//glm::mat4 projection;
			// shader.setMat4("projection", camera.GetProjectionMatrix((float)SCR_WIDTH / (float)SCR_HEIGHT));

			try {
				MeshSorter sorter(lightingShader, camera);

				for (Particle& p : system.getParticles()) {
					vec3 gamma = p.getPosition();
					glm::vec3 particlePos = { gamma.x , gamma.y, gamma.z };
					sorter.add(sphereMesh, lightingShader, particlePos, { 0.8f, 0.4f, 0.4f });
				}

				sorter.sort(camera.Position);

				sorter.draw((float)SCR_WIDTH / (float)SCR_HEIGHT);
			} catch (const std::exception& e) {
				std::cerr << "An error occurred during sorting: " << e.what() << std::endl;
			}

			//// Draw bounding box before particles
			//lightingShader.setVec3("objectColor", 0.1f, 0.1f, 0.1f);
			//drawSystemBounds(lightingShader, lightCubeShader, boxMesh, vec3(0));

			//// draw particles
			//lightingShader.setVec3("objectColor", 0.8f, 0.4f, 0.4f);
			//system.drawSystemParticles(lightingShader, lightCubeShader, cubeMesh /*sphereMesh*/);

			bool foundTargetTime = false;

			// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
			// -------------------------------------------------------------------------------
			glfwSwapBuffers(window);
			glfwPollEvents();

			// Wait for frame time to elapse (16ms for ~60FPS)
			while (chrono::high_resolution_clock::now() - lastFrameTime < FRAME_DURATION) {
				/* wait */

				// Instead of solving at render-time we pre-run the sim and read in positions from a file
				if (!PAUSE /*|| KEY_N*/) {
					while (!PAUSE && renderIterations < targetRenderIteration && renderIterations != std::numeric_limits<unsigned int>::max()) {
						renderIterations = simulation.readNext();
						//cout << renderIterations << endl;
					}
					if (renderIterations == std::numeric_limits<unsigned int>::max()) {
						PAUSE = true;
						simulation.resetIn();
						targetRenderIteration = 0;
						cout << "Total render time: " << chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - totalRenderTime).count() << " ms" << endl;
						totalRenderTime = chrono::high_resolution_clock::now();
						renderIterations = 0;
						//renderIterations = simulation.readNext();
					}
				}
			}

			if (!PAUSE) {
				targetRenderIteration++;
			}
		}
	}
	catch (const std::exception& e) {
		std::cerr << "An error occurred during the render loop: " << e.what() << std::endl;
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();

	return 0;
}

//void drawSystemBounds(Mesh boxMesh, Shader& shader, vec3 offset) {
//	glm::mat4 model = glm::mat4(1.0f);
//	model = glm::translate(model, glm::vec3(offset.x, offset.y, offset.z));
//	shader.setMat4("model", model);
//	boxMesh.draw(shader);
//}

void drawSystemBounds(Shader& objectShader, Shader& lightingShader, Mesh boxMesh, vec3 offset) {
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(offset.x, offset.y, offset.z));
	// shader.setMat4("model", model);
	objectShader.setMat4("model", model);
	boxMesh.draw(objectShader);
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

		if (ss >> l) {
			v = l;
		}
	}
}

void getUnsignedWithDefault(istream& in, unsigned& v)
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

void centerMouse(GLFWwindow* window, int windowWidth, int windowHeight) {
	// 1. Calculate center coordinates
	double centerX = windowWidth / 2.0;
	double centerY = windowHeight / 2.0;

	// 2. Set the cursor position to the center
	glfwSetCursorPos(window, centerX, centerY);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		// Pause functionality can be implemented here
		KEY_SPACE = true;
	}

	float cameraSpeed = (2.5f * FRAME_TIME);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::FORWARD, FRAME_TIME);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::BACKWARD, FRAME_TIME);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::LEFT, FRAME_TIME);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::RIGHT, FRAME_TIME);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::UP, FRAME_TIME);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::DOWN, FRAME_TIME);

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
		firstMouse = true;
	}
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll((float)yoffset);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	float xcenter = (float)SCR_WIDTH / 2.0f;
	float ycenter = (float)SCR_HEIGHT / 2.0f;

	// ignore mouse input when holding click
	if (firstMouse)
	{
		xpos = xcenter;
		ypos = ycenter;

		firstMouse = false;
	}

	float xoffset = xpos - xcenter;
	float yoffset = ycenter - ypos; // reversed since y-coordinates go from bottom to top

	camera.ProcessMouseMovement(xoffset, yoffset);
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

int loadGLAD(bool& retFlag)
{
	retFlag = true;
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Failed to initialize GLAD" << std::endl;
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
		cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

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

//void createVertexAndIndexData(vec3 boxSize, std::vector<float>& vertices, std::vector<unsigned int>& indices) {
//	float size[3] = { (float)boxSize.x, (float)boxSize.y, (float)boxSize.z };
//	float boundingBoxVert[] = {
//		// positions                                            // colors
//		 size[0],  size[1], -size[2],     0, 0, 0,    // Top right
//		 size[0], -size[1], -size[2],     0, 0, 0,    // Bottom right
//		-size[0],  size[1], -size[2],     0, 0, 0,    // Top left
//		-size[0], -size[1], -size[2],     0, 0, 0     // Bottom left
//	};
//	unsigned int boundingBoxInd[] = {
//		0, 1, 2,
//		2, 1, 3
//	};
//
//	vertices.clear();
//	indices.clear();
//
//	for (float f : boundingBoxVert) {
//		vertices.push_back(f);
//	}
//	for (unsigned int i : boundingBoxInd) {
//		indices.push_back(i);
//	}
//}
