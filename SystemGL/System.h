#pragma once

#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib> // For rand() and srand()
#include <ctime>   // For time()

const double BIG_G = 1; //6.67430e-11f; <-- Gravitational constant is one for simplicity
const double PI = 3.14159265358979323846f;

struct vec3 {
	vec3(double val = 0.0f) : x(val), y(val), z(val) {}
	vec3(double x, double y, double z) : x(x), y(y), z(z) {}

	vec3 operator+(const vec3& other) const {
		return vec3(x + other.x, y + other.y, z + other.z);
	}
	vec3 operator*(double scalar) const {
		return vec3(x * scalar, y * scalar, z * scalar);
	}
	vec3 operator-(const vec3& other) const {
		return vec3(x - other.x, y - other.y, z - other.z);
	}
	vec3 operator/(double scalar) const {
		return vec3(x / scalar, y / scalar, z / scalar);
	}
	vec3 operator-() const {
		return vec3(-x, -y, -z);
	}

	vec3& operator+=(const vec3& other) {
		x += other.x; y += other.y; z += other.z;
		return *this;
	}
	vec3& operator-=(const vec3& other) {
		x -= other.x; y -= other.y; z -= other.z;
		return *this;
	}
	vec3& operator*=(double scalar) {
		x *= scalar; y *= scalar; z *= scalar;
		return *this;
	}
	vec3& operator/=(double scalar) {
		x /= scalar; y /= scalar; z /= scalar;
		return *this;
	}

	vec3 normalized() const {
		double mag = sqrt(x * x + y * y + z * z);
		if (mag == 0) return vec3(0.0f);
		else return vec3(x / mag, y / mag, z / mag);
	}

	friend std::ostream& operator<<(std::ostream& os, vec3 vec) {
		os << vec.x << ", " << vec.y << ", " << vec.z;
		return os;
	}


	double x, y, z;
};

//ostream& operator<<(ostream& os, vec3 vec) override {
//	os << vec.x << ", " << vec.y << ", " << vec.z;
//	return os;
//}

class System {
public:
	System() {};
	System(std::vector<System> subsystems) : subsystems(subsystems) {
		this->position = vec3(0);
		this->velocity = vec3(0);
		this->mass = 1;
		this->time = 0.0f;
		this->deltaTime = 0.001f;
	}
	System(vec3 position, vec3 velocity, double mass, double time, double deltaTime)
		: position(position), velocity(velocity), mass(mass), time(time), deltaTime(deltaTime) {
	}

	void set(System system) {
		this->position = system.position;
		this->velocity = system.velocity;
		this->mass = system.mass;
		this->time = system.time;
		this->deltaTime = system.deltaTime;

		this->subsystems = system.subsystems;
	}
	void set(vec3 position = vec3(0), vec3 velocity = vec3(0), double mass = 1, double time = 0.0f, double deltaTime = 0.001f)
	{
		this->position = position;
		this->velocity = velocity;
		this->mass = mass;
		this->time = time;
		this->deltaTime = deltaTime;
	}

	void evolve() {
		position += velocity * deltaTime;
		time += deltaTime;

		for (System& subsystem : subsystems) {
			subsystem.evolve();
		}
	}

	std::vector<System> subsystems;

	vec3 position = vec3(0); vec3 velocity = vec3(0);
	double mass = 1.0f;
	double time = 0.0f;
	double deltaTime = 0.001f;
};

class Interaction {
public:
	System null = System(vec3(1), vec3(1), 1.0, 1.0, 0.001);
	//System* nullStar = &null;
	std::vector<System*> systems/* = { &null }*/;

	Interaction() {};
	Interaction(std::vector<System*> systems) {
		systems.clear();
		for (System* system : systems) {
			this->systems.push_back(system);
		}
	};
};

class GravityDeprecated : Interaction {
public:
	GravityDeprecated(std::vector<System*> systems) {
		for (System* system : systems) {
			this->systems.push_back(system);
		}
	};

	void apply(System* systemA, System* systemB) {
		vec3 aR = systemB->position - systemA->position;
		double rSquared = aR.x * aR.x + aR.y * aR.y + aR.z * aR.z;
		double forceMag = (BIG_G * systemA->mass * systemB->mass) / rSquared;
		vec3 forceA = aR.normalized() * forceMag;
		systemA->velocity += forceA * (1.0f / systemA->mass) * systemA->deltaTime;
		systemB->velocity -= forceA * (1.0f / systemB->mass) * systemB->deltaTime;
	}
};

class BoxCollider : Interaction {
public:
	vec3 bounds; // full box is 2 * bounds in each dimension
	vec3 boxPos;

	BoxCollider(std::vector<System*>& systems, vec3 bounds, vec3 boxPos = vec3(0)) {
		for (System* system : systems) {
			this->systems.push_back(system);
		}
		this->bounds = bounds;
	};

	void apply() {
		for (System* system : systems) {
			double xPosLimitPlus = bounds.x + boxPos.x;
			double xPosLimitMinus = -bounds.x + boxPos.x;
			if (system->position.x > xPosLimitPlus || system->position.x < xPosLimitMinus) {
				//std::cout << "bounce x" << std::endl;

				system->velocity.x = -system->velocity.x;
				if (system->position.x > xPosLimitPlus) {
					double deltaX = abs(system->position.x - xPosLimitPlus);
					system->position.x = xPosLimitPlus - deltaX;
				}
				else {
					double deltaX = abs(system->position.x - xPosLimitMinus);
					system->position.x = xPosLimitMinus + deltaX;
				}
			}
			double yPosLimitPlus = bounds.y + boxPos.y;
			double yPosLimitMinus = -bounds.y + boxPos.y;
			if (system->position.y > yPosLimitPlus || system->position.y < yPosLimitMinus) {
				//std::cout << "bounce y" << std::endl;

				system->velocity.y = -system->velocity.y;
				if (system->position.y > xPosLimitPlus) {
					double deltaY = abs(system->position.y - yPosLimitPlus);
					system->position.y = yPosLimitPlus - deltaY;
				}
				else {
					double deltaY = abs(system->position.y - yPosLimitMinus);
					system->position.y = yPosLimitMinus + deltaY;
				}
			}
			double zPosLimitPlus = bounds.z + boxPos.z;
			double zPosLimitMinus = -bounds.z + boxPos.z;
			if (system->position.z > zPosLimitPlus || system->position.z < zPosLimitMinus) {
				//std::cout << "bounce z" << std::endl;

				system->velocity.z = -system->velocity.z;
				if (system->position.z > xPosLimitPlus) {
					double deltaZ = abs(system->position.z - zPosLimitPlus);
					system->position.z = zPosLimitPlus - deltaZ;
				}
				else {
					double deltaZ = abs(system->position.z - zPosLimitMinus);
					system->position.y = zPosLimitMinus + deltaZ;
				}
			}
		}
	}
};


void simpleGravitationalOrbitSim(System simpleSystem, std::ofstream output) {
	// define gravitational interaction between particles
	//System& subsystems = simpleSystem.subsystems;
	std::vector<System*> subsystemPtrs;
	for (int i = 0; i < simpleSystem.subsystems.size(); i++) {
		subsystemPtrs.push_back(&simpleSystem.subsystems[i]);
	}
	GravityDeprecated gravityBigG(subsystemPtrs);

	if (output.is_open()) {
		std::cerr << "output stream is open";
	}
	output.open("simulation.txt");

	output << "t" << simpleSystem.time;
	
	for (int i = 0; i < simpleSystem.subsystems.size(); i++) {
		System subsystem = simpleSystem.subsystems[i];
		output << "; " << "p" << (i + 1) << ", " << subsystem.position.x << ", " << subsystem.position.y << ", " << subsystem.position.z;
	}
	output << std::endl;

	const int orbits = 10;

	// evolve the system over a single full orbit period    
	while (simpleSystem.time < orbits * 2 * PI / sqrt(2)) {
		for (int i = 0; i < simpleSystem.subsystems.size(); i++) {
			for (int j = i + 1; j < simpleSystem.subsystems.size(); j++) {
				gravityBigG.apply(&simpleSystem.subsystems[i], &simpleSystem.subsystems[j]);
			}
		}
		simpleSystem.evolve();

		output << "t" << simpleSystem.time;

		for (int i = 0; i < simpleSystem.subsystems.size(); i++) {
			System subsystem = simpleSystem.subsystems[i];
			output << "; " << "p" << (i + 1) << ", " << subsystem.position.x << ", " << subsystem.position.y << ", " << subsystem.position.z;
		}
		output << std::endl;
	}

	// close files
	output.close();
};

void pressureApproximation(System particles, double T, std::ofstream output) {
	const int mass = 1;

	double startTime = particles.time;

	if (output.is_open()) {
		std::cerr << "output stream is open";
	}
	output.open("simulation.txt");

	srand(time(0)); // Seed random number generator

	for (System& particle : particles.subsystems) {
		vec3 randomPos(
			((double)rand() / RAND_MAX) * 2.0f - 1.0f,
			((double)rand() / RAND_MAX) * 2.0f - 1.0f,
			0 /*((double)rand() / RAND_MAX) * 2.0f - 1.0f*/
		);
		vec3 randomVel(
			((double)rand() / RAND_MAX) * 2.0f - 1.0f,
			((double)rand() / RAND_MAX) * 2.0f - 1.0f,
			0 /*((double)rand() / RAND_MAX) * 2.0f - 1.0f*/
		);

		std::cout << "Random Position: (" << randomPos << ")" << std::endl;

		particle.set(randomPos, randomVel, mass);
	}

	output << "t" << particles.time;

	for (int i = 0; i < particles.subsystems.size(); i++) {
		System subsystem = particles.subsystems[i];
		output << "; " << "p" << (i + 1) << ", " << subsystem.position.x << ", " << subsystem.position.y << ", " << subsystem.position.z;
	}
	output << std::endl;

	std::cout << "Starting Pressure Approximation Simulation for " << T << " seconds..." << std::endl;
	std::cout << "Particle System Start Time: " << particles.time << std::endl;
	while (particles.time < T - startTime) {
		std::cout << "Evolving particle system..." << std::endl;

		particles.evolve();

		std::vector<System*> particlePtrs;
		for (int i = 0; i < particles.subsystems.size(); i++) {
			particlePtrs.push_back(&particles.subsystems[i]);
		}
		BoxCollider boxCollider(particlePtrs, vec3(1.0f, 1.0f, 1.0f));

		boxCollider.apply();

		std::cout << "Particle System Time: " << particles.time << std::endl;

		output << "t" << particles.time;

		for (int i = 0; i < particles.subsystems.size(); i++) {
			System subsystem = particles.subsystems[i];
			output << "; " << "p" << (i + 1) << ", " << subsystem.position.x << ", " << subsystem.position.y << ", " << subsystem.position.z;
		}
		output << std::endl;
	}
}