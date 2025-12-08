#pragma once

#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <map>

#include <memory>

#include <utility> // <--- add this for std::move
#include <random>

#include <chrono>
#include <cassert>

#include <sstream>
#include <algorithm>
#include <cctype>

/*
* constants
*/
// const long double BIG_G = 1; //6.67430e-11; <-- Gravitational constant is one for simplicity
const long double BOLTZMANN = 1.380649e-23l;
const long double PI = 3.14159265358979323846l;
const long double MOLE = 6.02214076e23;
const long double R_GAS = 8.314462618l; // universal: other one will be added when needed

/*
* A simple 3D vector class for basic vector operations
* Technically obselete due to glm::vec3, but used to learn about structs and operator overloading
*/
struct vec3 {
	long double x, y, z;

	/*
	* default constructor
	* 
	* @ensures this.x = 0, this.y = 0, this.z = 0
	*/
	vec3();
	/*
	* constant constructor
	* 
	* @param val - initial value to set all components to
	* 
	* @ensures this.x = val, this.y = val, this.z = val
	*/
	vec3(long double val);
	/*
	* component constructor
	* 
	* @param x - initial x component
	* @param y - initial y component
	* @param z - initial z component
	* 
	* @ensures this.x = x, this.y = y, this.z = z
	*/
	vec3(long double x, long double y, long double z);

	/*
	* reverse the direction of the vector
	* 
	* @returns a new vec3 s.t. result = -this
	*/
	vec3 operator-() const;

	/*
	* add other to this and return this
	* 
	* @param other - vec3 to add
	* 
	* @ensures this = #this + other
	* @returns this
	*/
	vec3& operator+=(const vec3& other);
	/*
	* substract other from this and return this
	* 
	* @param other - vec3 to subtract
	* 
	* @ensures this = #this - other
	* @returns this
	*/
	vec3& operator-=(const vec3& other);
	/*
	* multiply this by scalar and return this
	* 
	* @param scalar - scalar to multiply by
	* 
	* @ensures this = #this * scalar
	* @returns this
	*/
	vec3& operator*=(long double scalar);
	/*
	* divide this by scalar and return this
	* 
	* @param scalar - scalar to divide by
	* 
	* @requires scalar != 0
	* 
	* @ensures this = #this / scalar
	* @returns this
	*/
	vec3& operator/=(long double scalar);

	/*
	* scales vector to unit length and returns the result
	* 
	* @restores this
	* 
	* @ensures result has length 1, unless this has length 0, in which case result is vec3(0)
	* @returns normalized vector
	*/
	vec3 normalized() const;

	long double dot(vec3 avec) const { return x * avec.x + y * avec.y + z * avec.z; };

	/*
	* helper function for printing vec3's
	* outputs in the format "x, y, z"
	* does not add an endl
	* 
	* @param os - output stream to write to
	* @param vec - vec3 to output
	* 
	* @ensures os contains the string representation of vec
	* @returns os
	*/
	friend std::ostream& operator<<(std::ostream& os, vec3 vec) {
		os << vec.x << ", " << vec.y << ", " << vec.z;
		return os;
	}
};

/*
* vec3 operator overloads for basic vector arithmetic
*/

/*
* adds two vec3's component-wise
* 
* @param a - first vec3
* @param b - second vec3
* 
* @ensures result = a + b
* @returns result
*/
vec3 operator+(vec3 a, vec3 b);
/*
* subtracts two vec3's component-wise
* 
* @param a - first vec3
* @param b - second vec3
* 
* @ensures result = a - b
* @returns result
*/
vec3 operator-(vec3 a, vec3 b);
/*
* multiplies vec3 by scalar component-wise
* this method handles the vec3 * constant case
* 
* @param lhs - vec3
* @param rhs - scalar
* 
* @ensures result = lhs * rhs
* @returns result
*/
vec3 operator*(vec3 lhs, long double rhs);
/*
* multiplies vec3 by scalar component-wise
* this method handles the constant * vec3 case
* 
* @param lhs - scalar
* @param rhs - vec3
*
* @ensures result = lhs * rhs
* @returns result
*/
vec3 operator*(long double lhs, vec3 rhs);
/*
* divides vec3 by scalar component-wise
* this method handles the vec3 / constant case
* 
* @param lhs - vec3
* @param rhs - scalar
* 
* @requires rhs != 0
* 
* @ensures result = lhs / rhs
* @returns result
*/
vec3 operator/(vec3 lhs, long double rhs);
/*
* divides vec3 by scalar component-wise
* this method handles the constant / vec3 case
* 
* @param lhs - scalar
* @param rhs - vec3
* 
* @requires rhs.x, rhs.y, rhs.z != 0
* 
* @ensures result = lhs / rhs
* @returns result
*/
vec3 operator/(long double lhs, vec3 rhs);

// LAGRANGIAN SOLVER DEFINITIONS ---------------

/*
* A simple particle class representing a point mass in 3D space
* Particles are the only objects in this model with mass, position, and velocity
* 
* Currently time is handled at the System level, not the Particle level but this is non-physical
* Special relativity makes implementing time at the particle level difficult, so for now we will ignore 
* relativistic effects and assume all particles share the same universal time under a given system
* Similarly, all systems should use the same time until properly handled, but this is not enforced
*/
class Particle {
public:
	/*
	* default constructor
	* 
	* @ensures position = vec3(0), velocity = vec3(0), mass = 1.0
	*/
	Particle();
	/*
	* initital conditions constructor
	* 
	* @param position - initial position of the particle
	* @param velocity - initial velocity of the particle
	* @param mass - mass of the particle
	*/
	Particle(vec3 position, vec3 velocity, long double mass/*, long double dt = 0.001*/);

	// setters
	/*
	* sets the position of the particle
	* 
	* @param newPosition - new position of the particle
	* 
	* @ensures position = newPosition
	*/
	void setPosition(vec3 newPosition);
	/*
	* sets the velocity of the particle
	* 
	* @param newVelocity - new velocity of the particle
	* 
	* @ensures velocity = newVelocity
	*/
	void setVelocity(vec3 newVelocity);
	/*
	* sets the mass of the particle
	* 
	* @param newMass - new mass of the particle
	* 
	* @ensures mass = newMass
	*/
	void setMass(long double newMass);

	// getters
	/*
	* fetches the position of the particle
	* 
	* @returns position
	*/
	vec3 getPosition();
	/*
	* fetches the velocity of the particle
	* 
	* @returns velocity
	*/
	vec3 getVelocity();
	/*
	* fetches the mass of the particle
	* 
	* @returns mass
	*/
	long double getMass();
	///*
	//* fetches the time of the particle
	//* 
	//* @returns time
	//*/
	//long double getTime() { return time; };
	///*
	//* fetches the dt of the particle
	//* 
	//* returns dt
	//*/
	//long double getDt() { return dt; };

	// transformations
	/*
	* translates the particle by delta
	* 
	* @param delta - translation vector
	* 
	* @ensures position = #position + delta
	*/
	void translateBy(vec3 delta);
	/*
	* accelerates the particle by delta
	* 
	* @param delta - acceleration vector
	* 
	* @ensures velocity = #velocity + delta
	*/
	void accelerateBy(vec3 delta);

	///*
	//* iterates time by dt
	//* 
	//* @ensures time = #time + dt
	//*/
	//void pushTime() { time += dt; };

	long double calcKE() { 
		return 0.5 * mass * velocity.dot(velocity); 
	};

private:
	vec3 position = vec3(0); // [m]
	vec3 velocity = vec3(0); // [m/s]
	long double mass = 1.0l; // [kg]

	//long double time = 0; // [s]
	//const long double dt; // [s]
};

class System {
public:

	System(long double dt = 0.001) : dt(dt) {};

	virtual void addParticle(Particle& particle) {
		particles.push_back(particle);
	};

	virtual size_t numberOfParticles() {
		return particles.size();
	}

	virtual std::vector<Particle>& getParticles() {
		return particles;
	}

	virtual Particle& getParticle(int i) {
		return particles[i];
	}
	virtual long double getTime() {
		return time;
	}
	virtual long double getDt() {
		return dt;
	}

	virtual void setTime(long double time) {
		time = time;
	}

	virtual void timeStep() {
		step++;
		time += dt;
	};

	virtual void evolve() {
		// Evolve particles
		for (Particle& particle : particles) {
			particle.translateBy(dt * particle.getVelocity());
		}

		// step time once after all evolutions are complete
		timeStep();
	}

	std::vector<Particle> particles;

	long double boxSize[2] = { 1, 1 };

	unsigned int step = 0; // redundant but will be used later when particles have their own time
	long double time = 0;
	long double dt = 0.001;
};

class PressureSystem : public System {
public:
	// 2d only for now
	PressureSystem(int numParticles, long double sysmass, long double molarMass, long double targetTemp, std::mt19937& generator, long double* boxSize = new long double[3]{ 1, 1, 1 }, long double dt = 0.001) {
		this->boxSize[0] = boxSize[0];
		this->boxSize[1] = boxSize[1];
		this->boxSize[2] = boxSize[2];
		this->dt = dt;

		//long double temp = 292l;

		//double moles = sysmass / molarMass; 

		long double scale_parameter = BOLTZMANN * targetTemp * MOLE / molarMass;

		std::gamma_distribution<long double> maxwell_dist(3.0 / 2.0, scale_parameter);

		std::uniform_real_distribution<long double> uniform_dist(-1.0, 1.0);

		for (int i = 0; i < numParticles; i++) {
			vec3 randomPos(
				uniform_dist(generator) * boxSize[0],
				uniform_dist(generator) * boxSize[1],
				uniform_dist(generator) * boxSize[2]
			);
			long double randomSpeed = std::sqrt(2.0l * maxwell_dist(generator));

			vec3 randomVel(
				uniform_dist(generator),
				uniform_dist(generator),
				uniform_dist(generator)
			);

			randomVel = randomSpeed * randomVel.normalized();

			/*long double scalingFactor = 0;
			bool correction = false;
			if (randomVel.x > 2 * boxSize[0] / dt) {
				double scale = boxSize[0] / randomVel.x;
				scalingFactor = scale;
				correction = true;
			}
			if (randomVel.y > 2 * boxSize[1] / dt) {
				double scale = boxSize[1] / randomVel.y;
				scalingFactor = (scale < scalingFactor) ? scale : scalingFactor;
				correction = true;
			}
			if (randomVel.z > 2 * boxSize[2] / dt) {
				double scale = boxSize[2] / randomVel.z;
				scalingFactor = (scale < scalingFactor) ? scale : scalingFactor;
				correction = true;
			}

			if (correction) {
				randomVel *= scalingFactor;
			}*/

			Particle particle(randomPos, randomVel, sysmass / numParticles);
			addParticle(particle);
		}

		/*vec3 systemVel = vec3(0);
		long double massSum = 0l;
		for (Particle& particle : getParticles()) {
			massSum += particle.getMass();
			systemVel += particle.getVelocity() * particle.getMass();
		}
		systemVel /= massSum;

		for (Particle& particle : getParticles()) {
			particle.accelerateBy(-systemVel);
		}*/
	}
	/*
	* @returns a 4 element array with the impulse on each wall of the box: +x, -x, +y, -y (, +z, -z... eventually)
	*  ^ should return a 6 element array when 3rd dimension is added
	*/
	std::array<long double, 6> reflectParticles() {
		long double impPlusX = 0;
		long double impMinusX = 0;
		long double impPlusY = 0;
		long double impMinusY = 0;
		long double impPlusZ = 0;
		long double impMinusZ = 0;

		for (Particle& particle : getParticles()) {
			vec3 position = particle.getPosition();
			long double mass = particle.getMass();

			vec3 newP = position;
			vec3 newV = particle.getVelocity();

			if (position.x > boxSize[0]) {
				long double difference = position.x - boxSize[0];

				newP.x = boxSize[0] - difference;
				newV.x = -newV.x;
				impPlusX += abs(2 * newV.x * mass);
			} 
			else if (position.x < -boxSize[0]) {
				long double difference = -position.x - boxSize[0];

				newP.x = -boxSize[0] + difference;
				newV.x = -newV.x;
				impMinusX += abs(2 * newV.x * mass);
			}

			if (position.y > boxSize[1]) {
				long double difference = position.y - boxSize[1];

				newP.y = boxSize[1] - difference;
				newV.y = -newV.y;
				impPlusY += abs(2 * newV.y * mass);
			}
			else if (position.y < -boxSize[1]) {
				long double difference = -position.y - boxSize[1];

				newP.y = -boxSize[1] + difference;
				newV.y = -newV.y;
				impMinusY += abs(2 * newV.y * mass);
			}

			if (position.z > boxSize[2]) {
				long double difference = position.z - boxSize[2];

				newP.z = boxSize[2] - difference;
				newV.z = -newV.z;
				impPlusZ += abs(2 * newV.z * mass);
			}
			else if (position.z < -boxSize[2]) {
				long double difference = -position.z - boxSize[2];

				newP.z = -boxSize[2] + difference;
				newV.z = -newV.z;
				impMinusZ += abs(2 * newV.z * mass);
			}

			particle.setPosition(newP);
			particle.setVelocity(newV);
		}

		return { impPlusX, impMinusX, impPlusY, impMinusY, impPlusZ, impMinusZ };
	}

	//void evolve() {
	//	System::evolve();
	//	reflectParticles();
	//}
	std::array<long double, 6> impulseEvolve() {
		System::evolve();

		return reflectParticles();
	}

private:
	long double boxSize[3] = { 1, 1, 1 }; // [m] : halfwidth, halfheight
};

///*
//* Essentially a wrapper class with an empty force function
//*/
//class Interaction {
//public:
//	/*
//	* default constructor
//	* declared inline for simplicity
//	*/
//	Interaction() {}
//
//	/*
//	* empty force function to be overridden by subclasses
//	* declared inline for simplicity
//	* 
//	* all forces should be functions of displacement, velocity, and time (unless I'm doing some wacky stuff)
//	* some subclasses may not use all parameters, but this set should include all necessary parameters for all forces
//	* 
//	* @param displacement - generalized displacement vector
//	* @param velocity - generalized velocity vector
//	* @param time - current time
//	*/
//	virtual vec3 force(vec3 displacement, vec3 velocity, double time) const { return vec3(0); }
//
//	std::string getLabelXML() {
//		return "null";
//	}
//};
//
///*
//* Interaction modeling a spring force with no damping or driving
//*/
//class Spring : public Interaction {
//public:
//	/*
//	* default constructor
//	* 
//	* @requires k >= 0
//	*/
//	Spring(double k);
//
//	/*
//	* spring force function
//	* 
//	* @param displacement - displacement vector from equilibrium position
//	* @param velocity - velocity vector (not used in this model)
//	* @param time - current time (not used in this model)
//	*/
//	vec3 force(vec3 displacement, vec3 velocity, double time) const override;
//	
//	std::string getLabelXML();
//private:
//	double k = 1.0; // spring constant [N/m]
//};
//
///*
//* A system class representing a collection of particles and subsystems
//*/
//class System {
//public:
//	/*
//	* default constructor
//	*/
//	System();
//	/*
//	* initial conditions constructor
//	* 
//	* @param subsystems - vector of subsystems
//	* @param particles - vector of particles
//	* @param interactions - vector of interactions
//	* 
//	* @ensures this.subsystems = subsystems, this.particles = particles, this.interactions = interactions
//	*/
//	System(std::vector<std::unique_ptr<System>> subsystems, std::vector<Particle> particles, std::vector<Interaction> interactions);
//
//	virtual ~System() = default;
//
//	// setters
//	/*
//	* overwrites the subsystems of the system
//	* 
//	* @param subsystems - new vector of subsystems
//	* 
//	* @ensures this.subsystems = subsystems
//	*/
//	virtual void setSubsystems(std::vector<std::unique_ptr<System>> subsystems);
//	/*
//	* overwrites the particles of the system
//	* 
//	* @param particles - new vector of particles
//	* 
//	* @ensures this.particles = particles
//	*/
//	virtual void setParticles(std::vector<Particle> particles);
//	/*
//	* overwrites the interactions of the system
//	* 
//	* @param interactions - new vector of interactions
//	* 
//	* @ensures this.interactions = interactions
//	*/
//	virtual void setInteractions(std::vector<Interaction> interactions);
//	/*
//	* sets the current time of the system
//	* 
//	* @param newTime - new current time
//	* 
//	* @ensures this.time = newTime
//	*/
//	virtual void setTime(double newTime);
//	/*
//	* sets the time step deltaT of the system
//	* 
//	* @param newDeltaT - new time step
//	* 
//	* @ensures this.deltaT = newDeltaT
//	*/
//	virtual void setDeltaT(double newDeltaT);
//
//	// adders
//	/*
//	* adds a subsystem to the system
//	* 
//	* @param subsystem - subsystem to add
//	* 
//	* @ensures subsystem is appended to this.subsystems
//	*/
//	virtual void addSubsystem(std::unique_ptr<System> subsystem);
//	/*
//	* adds a particle to the system
//	* 
//	* @param particle - particle to add
//	* 
//	* @ensures particle is appended to this.particles
//	*/
//	virtual void addParticle(Particle particle);
//	/*
//	* adds an interaction to the system
//	* 
//	* @param interaction - interaction to add
//	* 
//	* @ensures interaction is appended to this.interactions
//	*/
//	virtual void addInteraction(Interaction interaction);
//
//	// getters
//	/*
//	* gets a reference to the subsystems vector
//	* 
//	* @returns reference to this.subsystems
//	*/
//	virtual std::vector<std::unique_ptr<System>>& getSubsystems();
//	/*
//	* gets a reference to the particles vector
//	* 
//	* @returns reference to this.particles
//	*/
//	virtual std::vector<Particle>& getParticles();
//	/*
//	* gets a reference to the interactions vector
//	* 
//	* @returns reference to this.interactions
//	*/
//	virtual std::vector<Interaction>& getInteractions();
//	/*
//	* gets a reference to a particle in particles
//	* 
//	* @param k - index to retriece
//	* 
//	* @requires 0 <= k < |particles|
//	* 
//	* @returns reference to this.particles[k]
//	*/
//	virtual Particle& getParticle(size_t k);
//	/*
//	* gets a reference to a system in subsystems
//	*
//	* @param k - index to retriece
//	*
//	* @requires 0 <= k < |subsystem|
//	*
//	* @returns reference to this.subsystems[k]
//	*/
//	virtual System* getSubsystem(size_t k);
//	/*
//	* gets a reference to a interaction in interactions
//	*
//	* @param k - index to retriece
//	*
//	* @requires 0 <= k < |interactions|
//	*
//	* @returns reference to this.interactions[k]
//	*/
//	virtual Interaction& getInteraction(size_t k);
//	/*
//	* gets the current time of the system
//	* 
//	* @returns this.time
//	*/
//	virtual double getTime();
//	/*
//	* gets the time step deltaT of the system
//	* 
//	* @returns this.deltaT
//	*/
//	virtual double getDeltaT();
//
//	// number of elements
//	/*
//	* gets the number of subsystems in the system
//	* 
//	* @returns |subsystems|
//	*/
//	virtual size_t numberOfSubsystems();
//	/*
//	* gets the number of particles in the system
//	* 
//	* @returns |particles|
//	*/
//	virtual size_t numberOfParticles();
//	/*
//	* gets the number of interactions in the system
//	* 
//	* @returns |interactions|
//	*/
//	virtual size_t numberOfInteractions();
//
//	// time handling
//	/*
//	* steps the system forward in time by deltaT
//	* DOES NOT RECURSIVELY STEP SUBSYSTEMS B/C EVOLVE() HANDLES THAT
//	* 
//	* @ensures time = #time + deltaT
//	*/
//	virtual void timeStep();
//
//	// force application
//	/*
//	* modifies particle velocities based on interactions
//	* NOT IMPLMENTED YET
//	*/
//	virtual void apply();
//
//	// evolution
//	/*
//	* update positions and velocities of particles based on deltaT
//	* recusively evolves subsystems
//	* intended to be ran immediately after apply()
//	* 
//	* @ensures position = #position + velocity * deltaT for all particles
//	* @ensures time = #time + deltaT
//	* @ensures all subsystems are evolved
//	*/
//	virtual void evolve();
//
//	virtual std::string getLabelXML() {
//		return "S";
//	}
//private:
//	std::vector<std::unique_ptr<System>> subsystems;
//	std::vector<Particle> particles;
//	std::vector<Interaction> interactions;
//
//	double time = 0.0; // [s]
//	double deltaT = 0.001; // [s]
//};
//
//class BoundSystem : public System {
//public:
//	/*
//	* default constructor
//	* 
//	* @ensures bounds = {1, 1}
//	*/
//	BoundSystem() {}
//	/*
//	* initial conditions constructor
//	* 
//	* @param bounds - array of two doubles representing halfWidth and halfHeight
//	* @param systems - vector of subsystems
//	* @param particles - vector of particles
//	* @param interactions - vector of interactions
//	* 
//	* @ensures this.bounds = bounds, this.subsystems = systems, this.particles = particles, this.interactions = interactions
//	*/
//	BoundSystem(double bounds[3], std::vector<std::unique_ptr<System>> subsystems, std::vector<Particle> particles, std::vector<Interaction> interactions) : System(std::move(subsystems), particles, interactions) {
//		this->bounds[0] = bounds[0];
//		this->bounds[1] = bounds[1];
//		this->bounds[2] = bounds[2];
//	}
//
//	BoundSystem(System* system) {
//		setSubsystems(std::move(system->getSubsystems()));
//		setParticles(system->getParticles());
//		setInteractions(system->getInteractions());
//	}
//
//	/*
//	* update positions and velocities of particles based on deltaT
//	* recusively evolves subsystems
//	* reflects particles off boundaries after movement push
//	* reflections conserve energy
//	* 
//	* @ensures position = #position + velocity * deltaT for all particles or abs(position.component) = bound.component if a reflection occurs
//	* @ensures time = #time + deltaT
//	* @ensures all subsystems are evolved
//	*/
//	void evolve() override;
//
//	std::string getLabelXML() override {
//		return "BS";
//	}
//private:
//	//std::vector<BoundSystem> subsystems;
//
//	double bounds[3] = { 1, 1, 1 }; // halfWidth, halfHeight; for simple math
//
//	/*
//	* reflects the particle off the boundaries if it is out of bounds
//	* reflections conserve energy and maintain momentum and displacement across the boundary normal
//	* 
//	* @param particle - particle to reflect
//	* 
//	* @ensures particle is within bounds after reflection
//	* @ensures energy is conserved: |v| = |#v|
//	* @ensures momentum is reflected accross the boundary normal
//	* @ensures displacement is reflected accross the boundary normal
//	*/
//	void reflectParticle(Particle& particle) {
//		vec3 pos = particle.getPosition();
//		vec3 vel = particle.getVelocity();
//
//		// for these reflections we need to make sure we conserve energy and momentum
//		// we can ensure conservation by reflecting the position and inverting the velocity component normal to the wall
//			
//		// check x bounds
//		if (pos.x > bounds[0]) {
//			double dif = pos.x - bounds[0];
//			pos.x = bounds[0] - dif;
//			vel.x = -vel.x;
//		} 
//		else if (pos.x < -bounds[0]) {
//			double dif = -bounds[0] - pos.x;
//			pos.x = -bounds[0] + dif;
//			vel.x = -vel.x;
//		}
//		// check y bounds
//		if (pos.y > bounds[1]) {
//			double dif = pos.y - bounds[1];
//			pos.y = bounds[1] - dif;
//			vel.y = -vel.y;
//		}
//		else if (pos.y < -bounds[1]) {
//			double dif = -bounds[1] - pos.y;
//			pos.y = -bounds[1] + dif;
//			vel.y = -vel.y;
//		}
//
//		particle.setPosition(pos);
//		particle.setVelocity(vel);
//	}
//};
//
//class Simulation {
//public:
//	Simulation(System& system, std::string systemFile, std::string outputFile);
//
//	void evolveSimulation();
//
//private:
//	std::string systemFile = "systemData.txt"; // initial system conditions in xml-like format
//	std::string outputFile = "simulationOutput.txt"; 
//
//	System& system;
//
//	/*
//	* Opens
//	* 
//	* @requires a properly formatted system file
//	* @requres an open input file stream
//	*/
//	void outputSystemState(System& system, std::ofstream& output, int depth = 0, int systemID = 0) {
//		outputDepthSeparator(output, depth);
//		output << "<" << system.getLabelXML() << systemID << " t = \"" << system.getTime() << "\" dt = \"" << system.getDeltaT() << "\">\n";
//
//		// output particles
//		for (size_t i = 0; i < system.numberOfParticles(); i++) {
//			Particle particle = system.getParticle(i);
//			vec3 pos = particle.getPosition();
//			vec3 vel = particle.getVelocity();
//			double mass = particle.getMass();
//
//			outputDepthSeparator(output, depth + 1);
//			output << "<P" << i << ">\n";
//			outputDepthSeparator(output, depth + 1);
//			output << "  " << "<p>\n";
//			outputDepthSeparator(output, depth + 1);
//			output << "    " << "<x> " << pos.x << " </x>\n";
//			outputDepthSeparator(output, depth + 1);
//			output << "    " << "<y> " << pos.y << " </y>\n";
//			outputDepthSeparator(output, depth + 1);
//			output << "    " << "<z> " << pos.z << " </z>\n";
//			outputDepthSeparator(output, depth + 1);
//			output << "  " << "</p>\n";
//			outputDepthSeparator(output, depth + 1);
//			output << "  " << "<v>\n";
//			outputDepthSeparator(output, depth + 1);
//			output << "    " << "<x> " << vel.x << " </x>\n";
//			outputDepthSeparator(output, depth + 1);
//			output << "    " << "<y> " << vel.y << " </y>\n";
//			outputDepthSeparator(output, depth + 1);
//			output << "    " << "<z> " << vel.z << " </z>\n";
//			outputDepthSeparator(output, depth + 1);
//			output << "  " << "</v>\n";
//			outputDepthSeparator(output, depth + 1);
//			output << "  " << "<m> " << mass << " </m>\n";
//			outputDepthSeparator(output, depth + 1);
//			output << "</P" << i << ">\n";
//		}
//		// output interactions
//		for (size_t i = 0; i < system.numberOfInteractions(); i++) {
//			Interaction interaction = system.getInteraction(i);
//
//			outputDepthSeparator(output, depth + 1);
//			output << "<I> " << interaction.getLabelXML() << " </I>\n";
//		}
//		int k = 0;
//		for (size_t i = 0; i < system.numberOfSubsystems(); i++) {
//			outputSystemState(*system.getSubsystem(i), output, depth + 1, k++);
//		}
//
//		outputDepthSeparator(output, depth);
//		output << "</S" << systemID << ">\n";
//	}
//	std::unique_ptr<System> loadSystemFromStream(std::istream& input, std::string firstLine) {
//
//		std::unique_ptr<System> system;
//
//		// helpers
//		auto trim = [](std::string s) {
//			s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char c) { return !std::isspace(c); }));
//			s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char c) { return !std::isspace(c); }).base(), s.end());
//			return s;
//			};
//		auto extract_between = [](const std::string& s, const std::string& open, const std::string& close) -> std::string {
//			size_t a = s.find(open);
//			if (a == std::string::npos) return "";
//			a += open.size();
//			size_t b = s.find(close, a);
//			if (b == std::string::npos) return "";
//			return s.substr(a, b - a);
//			};
//		auto extract_t_and_dt = [extract_between, trim](const std::string& t, System* system) {
//			if (t.rfind("t = ") == 0) {
//				double time;
//				std::string s = extract_between(t, "t = \"", "\"");
//				if (!s.empty()) time = std::stod(trim(s));
//
//				system->setTime(time);
//			}
//			if (t.rfind("dt = ") == 0) {
//				double deltaT;
//				std::string s = extract_between(t, "dt = \"", "\"");
//				if (!s.empty()) deltaT = std::stod(trim(s));
//
//
//				system->setDeltaT(deltaT);
//			}
//			};
//
//
//		// consume and stop; the next getline will read the first content line
//		if (firstLine.rfind("<S", 0) == 0) {
//			std::unique_ptr<System> tempSystem;
//
//			/*if (t.rfind("t = ") == 0) {
//				double time;
//				std::string s = extract_between(line, "t = \"", "\"");
//				if (!s.empty()) time = std::stod(trim(s));
//
//				tempSystem->setTime(time);
//			}
//			if (t.rfind("dt = ") == 0) {
//				double deltaT;
//				std::string s = extract_between(line, "dt = \"", "\"");
//				if (!s.empty()) deltaT = std::stod(trim(s));
//						
//
//				tempSystem->setDeltaT(deltaT);
//			}*/
//
//			extract_t_and_dt(firstLine, tempSystem.get());
//
//			system = std::move(tempSystem);
//		}
//		else if (firstLine.rfind("<BS", 0) == 0) {
//			std::unique_ptr<BoundSystem> tempSystem;
//
//			extract_t_and_dt(firstLine, tempSystem.get());
//
//			system = std::move(tempSystem);
//		}
//
//		std::string line;
//
//		while (std::getline(input, line)) {
//			std::string t = trim(line);
//			if (t.empty()) continue;
//
//			// closing system -> return to caller
//			if (t.rfind("</S", 0) == 0) {
//				/*std::wstring preMessage = std::to_wstring(i) + L"\n";
//				const wchar_t* message = preMessage.c_str();
//				Microsoft::VisualStudio::CppUnitTestFramework::Logger::WriteMessage(message);*/
//				break;
//			}
//
//			if (t.find("<S", 0) == 0) {
//				auto subsystem = loadSystemFromStream(input, t);
//
//				extract_t_and_dt(t, subsystem.get());
//
//				system->addSubsystem(std::move(subsystem));
//			}
//			if (t.find("<BS", 0) == 0) {
//				auto subsystem = loadSystemFromStream(input, t);
//
//				extract_t_and_dt(t, subsystem.get());
//
//				system->addSubsystem(std::move(subsystem));
//			}
//
//			//// nested subsystem: opening tag "<S..."
//			//if (t.rfind("<S", 0) == 0) {
//
//			//	
//			//	//std::unique_ptr<System> sub = std::make_unique<System>();
//			//	//system.addSubsystem(std::move(sub));
//			//	//size_t size = system.numberOfSubsystems(); // need to read in time data as well eventually
//
//			//	/*if (t.rfind("t = ") == 0) {
//			//		double time;
//			//		std::string s = extract_between(line, "t = \"", "\"");
//			//		if (!s.empty()) time = std::stod(trim(s));
//
//			//		system.setTime(time);
//			//	}
//			//	if (t.rfind("dt = ") == 0) {
//			//		double deltaT;
//			//		std::string s = extract_between(line, "dt = \"", "\"");
//			//		if (!s.empty()) deltaT = std::stod(trim(s));
//
//
//			//		system.setDeltaT(deltaT);
//			//	}*/
//
//			//	extract_t_and_dt(t, system);
//
//			//	// create a new subsystem and recurse
//			//	System* subsystem = loadSystemFromStream(input, false); // will *not* consume until matching </S...>
//			//	
//			//	system->addSubsystem(std::make_unique<System>(subsystem));
//
//			//	continue;
//			//}
//			//if (t.find("<BS") == 0) {
//			//	extract_t_and_dt(t, system);
//
//			//	// create a new subsystem and recurse
//			//	System* subsystem = loadSystemFromStream(input, false);
//			//	BoundSystem* bsubsystem = BoundSystem(subsystem);
//
//			//	system->addSubsystem(std::make_unique<System>(subsystem));
//
//			//	continue;
//			//}
//
//			// particle block
//			if (t.find("<P") != std::string::npos) {
//				std::string tmp;
//				// read "<p>"
//				if (!std::getline(input, tmp)) break;
//
//				// position x,y,z
//				double px = 0.0, py = 0.0, pz = 0.0;
//				if (std::getline(input, tmp)) {
//					std::string s = extract_between(tmp, "<x>", "</x>");
//					if (!s.empty()) px = std::stod(trim(s));
//				}
//
//				if (std::getline(input, tmp)) {
//					std::string s = extract_between(tmp, "<y>", "</y>");
//					if (!s.empty()) py = std::stod(trim(s));
//				}
//
//				if (std::getline(input, tmp)) {
//					std::string s = extract_between(tmp, "<z>", "</z>");
//					if (!s.empty()) pz = std::stod(trim(s));
//				}
//
//				// skip "</p>"
//				std::getline(input, tmp);
//				// skip "<v>"
//				std::getline(input, tmp);
//
//
//				// velocity x,y,z
//				double vx = 0.0, vy = 0.0, vz = 0.0;
//				if (std::getline(input, tmp)) {
//					std::string s = extract_between(tmp, "<x>", "</x>");
//					if (!s.empty()) vx = std::stod(trim(s));
//				}
//
//				if (std::getline(input, tmp)) {
//					std::string s = extract_between(tmp, "<y>", "</y>");
//					if (!s.empty()) vy = std::stod(trim(s));
//				}
//
//				if (std::getline(input, tmp)) {
//					std::string s = extract_between(tmp, "<z>", "</z>");
//					if (!s.empty()) vz = std::stod(trim(s));
//				}
//
//				// skip "</v>"
//				std::getline(input, tmp);
//
//				// mass line
//				double mass = 1.0;
//				if (std::getline(input, tmp)) {
//					std::string s = extract_between(tmp, "<m>", "</m>");
//					if (!s.empty()) mass = std::stod(trim(s));
//				}
//
//				// skip closing "</P#>"
//				std::getline(input, tmp);
//
//
//				/*std::wstring preMessage = std::to_wstring(px) + L"\n";
//				const wchar_t* message = preMessage.c_str();
//				Microsoft::VisualStudio::CppUnitTestFramework::Logger::WriteMessage(message);*/
//
//				Particle p(vec3(px, py, pz), vec3(vx, vy, vz), mass);
//				system.get()->addParticle(p);
//				continue;
//			}
//
//			// interactions ignored for now
//			if (t.find("<I>") != std::string::npos) {
//				continue;
//			}
//
//			// Unknown line: skip
//		}
//
//		return std::move(system);
//	}
//	void outputDepthSeparator(std::ofstream& output, int depth) {
//		for (int i = 0; i < depth; i++) {
//			output << "  ";
//		}
//	}
//};

//class System {
//public:
//	System();
//	System(std::vector<System> subsystems);
//	System(vec3 position, vec3 velocity, double mass, double time, double deltaT);
//
//	void addSubsystem(System system);
//	/*
//	* @requires 0 <= k < |subsytems|
//	*/
//	void removeSubsystem(int k);
//
//	void clearSubsystems();
//	void setSubsystems(std::vector<System> subsystems);
//
//	std::vector<System> getSubsystems();
//
//	/*
//	* @requires 0 <= k < |subsytems|
//	* @requires 0 < |subsystems|
//	*/
//	System& subsystem(int k);
//	int numberOfSubsystems();
//	int numberOfParticles();
//
//	/*
//	* systems with empty subsytems are particles
//	* systems which have subsystems are complex, and are not particles
//	* (which are the only objects with mass, position, and velocity; all other properties are emergent)
//	*/
//	bool isParticle();
//
//	/*
//	* If the system is a particle, then this function just returns this->position
//	* If the system is complex, then this function updates the system's position from
//	*	its subsytems, then returns this->position
//	*/
//	vec3 derivePosition();
//	vec3 deriveVelocity();
//	double deriveMass();
//
//	// TODO: BOTH OF THESE NAMES ARE WRONG
//	/*
//	* Similar to the derive methods: 
//	* for particles this function translates the position and returns it; 
//	* for complex systems, we must go through each child and translate those, then derive the position of 
//	* the complex system from its parts using recursion and the COM formula.
//	*/
//	vec3 translateBy(vec3 delta);
//	void translateTo(vec3 target);
//
//	/*
//	* directly adds delta to velocity in the same manner as translateBy
//	*/
//	vec3 accelerateBy(vec3 delta);
//	void accelerateTo(vec3 target);
//
//	void overrideMass(double mass);
//
//	double getTime();
//	void setTime(double time);
//	double getDeltaT();
//	void setDeltaT(double deltaT);
//
//	void timeStep();
//
//	void evolve();
//
//	void outputLineParticlesOnly(std::ofstream& output);
//	void outputParticlePositions(std::ofstream& output);
//
//private:
//	std::vector<System> subsystems;
//
//	vec3 position = vec3(0); // [m]
//	vec3 velocity = vec3(0); // [m/s]
//	double mass = 1.0; // [kg]
//	
//	double time = 0.0; // [s]
//	double deltaT = 0.001; // [s]
//};
//
//class BoundSystem : System {
//public:
//	BoundSystem() {}
//	BoundSystem(std::vector<System> systems) {
//		setSubsystems(systems);
//	}
//	BoundSystem(double bounds[2], vec3 position, vec3 velocity, double mass, double time, double deltaT) {
//		bounds[0] = bounds[0];
//		bounds[1] = bounds[1];
//		translateTo(position);
//		accelerateTo(velocity);
//		overrideMass(mass);
//		setTime(time);
//		setDeltaT(deltaT);
//	}
//
//	void evolve() {
//		translateBy(deriveVelocity() * getDeltaT());
//		
//		reflect();
//
//		timeStep();
//
//		int size = numberOfSubsystems();
//		for (int i = 0; i < size; i++) {
//			subsystem(i).evolve();
//		}
//	}
//
//	// handle collision with boundaries
//		// for now the boundary condition is immovable impenetrable walls
//		// (non-rigorously: infinite mass walls, completely elastic collisions, & conservation of energy)
//		// TODO: Other boundary conditions (different temp surfaces, letting particles in & out, etc.)
//	void reflect() {
//		vec3 startPos = derivePosition();
//
//		// need to check this math for conservation
//		if (startPos.x > bounds[0]) {
//			
//		}
//		else if (startPos.x < -bounds[0]) {
//			
//		}
//	}
//
//private:
//	double bounds[2] = { 1, 1 }; // halfWidth, halfHeight; for simple math
//};
//
//class Interaction {
//public:
//	Interaction() {};
//
//	vec3 force(vec3 position, vec3 velocity, double time) {};
//};
//
//class Spring : Interaction {
//public:
//	Spring(double k) : k(k) {};
//
//	vec3 force(vec3 displacement, vec3 velocity, double time) {
//		return - displacement * k;
//	};
//
//private:
//	double k = 1.0;
//};
//
//System::System() {}
//System::System(std::vector<System> subsystems) {
//	this->subsystems = subsystems;
//
//}
//System::System(vec3 position, vec3 velocity, double mass, double time, double deltaT) : position(position), velocity(velocity), mass(mass), time(time), deltaT(deltaT) {}
//
//void System::addSubsystem(System system) {
//	subsystems.push_back(system);
//}
///*
//* @requires 0 <= k < |subsytems|
//*/
//void System::removeSubsystem(int k) {
//	subsystems.erase(subsystems.begin() + k);
//}
//
//void System::clearSubsystems() {
//	subsystems.clear();
//}
//void System::setSubsystems(std::vector<System> subsystems) {
//	this->subsystems = subsystems;
//}
//
//std::vector<System> System::getSubsystems() {
//	return this->subsystems;
//}
//
///*
//* @requires 0 <= k < |subsytems|
//* @requires subsystems
//*/
//System& System::subsystem(int k) {
//	return subsystems[k];
//}
//int System::numberOfSubsystems() {
//	return subsystems.size();
//}
//int System::numberOfParticles() {
//	int count = 0;
//	for (System subsystem : subsystems) {
//		if (subsystem.isParticle()) {
//			count++;
//		}
//		else {
//			count += subsystem.numberOfParticles();
//		}
//	}
//	return count;
//}
//
///*
//* systems with empty subsytems are particles
//* systems which have subsystems are complex, and are not particles
//* (which are the only objects with mass, position, and velocity; all other properties are emergent)
//*/
//bool System::isParticle() {
//	if (numberOfSubsystems() == 0) {
//		return true;
//	}
//	else {
//		return false;
//	}
//}
//
///*
//* If the system is a particle, then this function just returns this->position
//* If the system is complex, then this function updates the system's position from
//*	its subsytems, then returns this->position
//*/
//vec3 System::derivePosition() {
//	if (!isParticle()) {
//		vec3 massPositionSum = vec3(0);
//		double massSum = 0.0;
//
//		for (System subsystem : subsystems) {
//			double mass = subsystem.deriveMass();
//			massPositionSum += subsystem.derivePosition() * mass;
//			massSum += mass;
//		}
//
//		this->position = massPositionSum / massSum;
//	}
//
//	return this->position;
//}
//vec3 System::deriveVelocity() {
//	if (!isParticle()) {
//		vec3 massVelocitySum = vec3(0);
//		double massSum = 0.0;;
//
//		for (System subsystem : subsystems) {
//			double mass = subsystem.deriveMass();
//			massVelocitySum += subsystem.derivePosition() * mass;
//			massSum += mass;
//		}
//
//		this->velocity = massVelocitySum / massSum;
//	}
//
//	return this->velocity;
//}
//double System::deriveMass() {
//	if (!isParticle()) {
//		double massSum = 0.0;
//
//		for (System subsystem : subsystems) {
//			massSum += subsystem.deriveMass();
//		}
//
//		this->mass = massSum;
//	}
//
//	return this->mass;
//}
//
//// TODO: BOTH OF THESE NAMES ARE WRONG
///*
//* Similar to the derive methods:
//* for particles this function translates the position and returns it;
//* for complex systems, we must go through each child and translate those, then derive the position of
//* the complex system from its parts using recursion and the COM formula.
//*/
//vec3 System::translateBy(vec3 delta) {
//	if (!isParticle()) {
//		vec3 massPositionSum = vec3(0);
//		double massSum = 0.0;
//
//		for (System subsystem : subsystems) {
//			double mass = subsystem.deriveMass();
//			massPositionSum += subsystem.translateBy(delta) * mass;
//			massSum += mass;
//		}
//
//		this->position = massPositionSum / massSum;
//	}
//	else { this->position += delta; }
//
//	return this->position;
//}
//void System::translateTo(vec3 target) {
//	this->position = target;
//}
//
///*
//* directly adds delta to velocity in the same manner as translateBy
//*/
//vec3 System::accelerateBy(vec3 delta) {
//	if (!isParticle()) {
//		vec3 massVelocitySum = vec3(0);
//		double massSum = 0.0;
//
//		for (System subsystem : subsystems) {
//			double mass = subsystem.deriveMass();
//			massVelocitySum += subsystem.accelerateBy(delta) * mass;
//			massSum += mass;
//		}
//
//		this->velocity = massVelocitySum / massSum;
//	}
//	else { this->velocity += delta; }
//
//	return this->velocity;
//}
//void System::accelerateTo(vec3 target) {
//	this->velocity = target;
//}
//
//void System::overrideMass(double mass) {
//	if (!isParticle()) {
//		for (System& subsystem : subsystems) {
//			subsystem.overrideMass(mass / numberOfSubsystems());
//		}
//	}
//
//	this->mass = mass;
//}
//
//double System::getTime() {
//	return this->time;
//}
//void System::setTime(double time) {
//	this->time = time;
//
//	for (System& subsystem : subsystems) {
//		subsystem.setTime(time);
//	}
//}
//double System::getDeltaT() {
//	return this->deltaT;
//}
//void System::setDeltaT(double deltaT) {
//	this->deltaT = deltaT;
//}
//
//void System::timeStep() {
//	this->time += this->deltaT;
//
//	for (System subsystem : subsystems) {
//		subsystem.time += subsystem.getDeltaT();
//	}
//}
//
//void System::evolve() {
//	translateBy(deriveVelocity() * deltaT);
//	timeStep();
//
//	for (System& subsystem : subsystems) {
//		subsystem.evolve();
//	}
//}
//
//void System::outputLineParticlesOnly(std::ofstream& output) {
//	output << "t" << getTime();
//
//	outputParticlePositions(output);
//
//	output << std::endl;
//}
//void System::outputParticlePositions(std::ofstream& output) {
//	for (int i = 0; i < numberOfSubsystems(); i++) {
//		if (this->subsystem(i).isParticle()) {
//			System& subsystem = this->subsystem(i);
//			output << "; " << "p" << (i + 1) << ", " << subsystem.derivePosition().x << ", " << subsystem.derivePosition().y << ", " << subsystem.derivePosition().z;
//		}
//		else {
//			this->subsystem(i).outputParticlePositions(output);
//		}
//	}
//}

//void pressureApproximation(System particles, double T, std::ofstream output) {
//	const int mass = 1;
//
//	double startTime = particles.getTime();;
//
//	if (output.is_open()) {
//		std::cerr << "output stream is open";
//	}
//	output.open("simulation.txt");
//
//	srand(time(0)); // Seed random number generator
//
//	for (int i = 0; i < particles.numberOfSubsystems(); i++) {
//		vec3 randomPos(
//			((double)rand() / RAND_MAX) * 2.0f - 1.0f,
//			((double)rand() / RAND_MAX) * 2.0f - 1.0f,
//			0 /*((double)rand() / RAND_MAX) * 2.0f - 1.0f*/
//		);
//		vec3 randomVel(
//			((double)rand() / RAND_MAX) * 2.0f - 1.0f,
//			((double)rand() / RAND_MAX) * 2.0f - 1.0f,
//			0 /*((double)rand() / RAND_MAX) * 2.0f - 1.0f*/
//		);
//
//		particles.subsystem(i).translateBy(randomPos);
//		particles.subsystem(i).accelerateBy(randomVel);
//	}
//
//	//for (System& particle : particles) {
//	//	vec3 randomPos(
//	//		((double)rand() / RAND_MAX) * 2.0f - 1.0f,
//	//		((double)rand() / RAND_MAX) * 2.0f - 1.0f,
//	//		0 /*((double)rand() / RAND_MAX) * 2.0f - 1.0f*/
//	//	);
//	//	vec3 randomVel(
//	//		((double)rand() / RAND_MAX) * 2.0f - 1.0f,
//	//		((double)rand() / RAND_MAX) * 2.0f - 1.0f,
//	//		0 /*((double)rand() / RAND_MAX) * 2.0f - 1.0f*/
//	//	);
//
//	//	std::cout << "Random Position: (" << randomPos << ")" << std::endl;
//
//	//	particle.set(randomPos, randomVel, mass);
//	//}
//
//	/*output << "t" << particles.getTime();
//
//	for (int i = 0; i < particles.numberOfSubsystems(); i++) {
//		System subsystem = particles.subsystem(i);
//		output << "; " << "p" << (i + 1) << ", " << subsystem.derivePosition().x << ", " << subsystem.derivePosition().y << ", " << subsystem.derivePosition().z;
//	}
//	output << std::endl;*/
//
//	particles.outputLineParticlesOnly(output);
//
//	std::cout << "Starting Pressure Approximation Simulation for " << T << " seconds..." << std::endl;
//	std::cout << "Particle System Start Time: " << particles.getTime() << std::endl;
//	while (particles.getTime() < T - startTime) {
//		std::cout << "Evolving particle system..." << std::endl;
//
//		particles.evolve();
//
//		/*std::vector<System*> particlePtrs;
//		for (int i = 0; i < particles.numberOfSubsystems(); i++) {
//			particlePtrs.push_back(&particles.subsystems[i]);
//		}*/
//		// BoxCollider boxCollider(particlePtrs, vec3(1.0f, 1.0f, 1.0f));
//
//		// boxCollider.apply();
//
//		std::cout << "Particle System Time: " << particles.getTime() << std::endl;
//
//		particles.outputLineParticlesOnly(output);
//	}
//}

//class System {
//public:
//	System() {};
//	System(std::vector<System> subsystems) : subsystems(subsystems) {
//		this->position = vec3(0);
//		this->velocity = vec3(0);
//		this->mass = 1;
//		this->time = 0.0f;
//		this->deltaTime = 0.001f;
//	}
//	System(vec3 position, vec3 velocity, double mass, double time, double deltaTime)
//		: position(position), velocity(velocity), mass(mass), time(time), deltaTime(deltaTime) {
//	}
//
//	void set(System system) {
//		this->position = system.position;
//		this->velocity = system.velocity;
//		this->mass = system.mass;
//		this->time = system.time;
//		this->deltaTime = system.deltaTime;
//
//		this->subsystems = system.subsystems;
//	}
//	void set(vec3 position = vec3(0), vec3 velocity = vec3(0), double mass = 1, double time = 0.0f, double deltaTime = 0.001f)
//	{
//		this->position = position;
//		this->velocity = velocity;
//		this->mass = mass;
//		this->time = time;
//		this->deltaTime = deltaTime;
//	}
//
//	void evolve() {
//		position += velocity * deltaTime;
//		time += deltaTime;
//
//		vec3 massWeightedPositionSum = vec3(0);
//		vec3 massWeightedVelocitySum = vec3(0);
//		double massSum = 0.0f;
//
//		for (System& subsystem : subsystems) {
//			subsystem.evolve();
//
//			// Account for CM motion
//			massWeightedPositionSum += subsystem.position * subsystem.mass;
//			massWeightedVelocitySum += subsystem.velocity * subsystem.mass;
//			massSum += subsystem.mass;
//		}
//
//		massWeightedPositionSum /= massSum;
//		massWeightedVelocitySum /= massSum;
//	}
//
//	std::vector<System> subsystems;
//
//	vec3 position = vec3(0); vec3 velocity = vec3(0);
//	double mass = 1.0f;
//	double time = 0.0f;
//	double deltaTime = 0.001f;
//};
//
//class Interaction {
//public:
//	System null = System(vec3(1), vec3(1), 1.0, 1.0, 0.001);
//	//System* nullStar = &null;
//	std::vector<System*> systems/* = { &null }*/;
//
//	Interaction() {};
//	Interaction(std::vector<System*> systems) {
//		systems.clear();
//		for (System* system : systems) {
//			this->systems.push_back(system);
//		}
//	};
//};
//
//class GravityDeprecated : Interaction {
//public:
//	GravityDeprecated(std::vector<System*> systems) {
//		for (System* system : systems) {
//			this->systems.push_back(system);
//		}
//	};
//
//	void apply(System* systemA, System* systemB) {
//		vec3 aR = systemB->position - systemA->position;
//		double rSquared = aR.x * aR.x + aR.y * aR.y + aR.z * aR.z;
//		double forceMag = (BIG_G * systemA->mass * systemB->mass) / rSquared;
//		vec3 forceA = aR.normalized() * forceMag;
//		systemA->velocity += forceA * (1.0f / systemA->mass) * systemA->deltaTime;
//		systemB->velocity -= forceA * (1.0f / systemB->mass) * systemB->deltaTime;
//	}
//};
//
//class BoxCollider : Interaction {
//public:
//	vec3 bounds; // full box is 2 * bounds in each dimension
//	vec3 boxPos;
//
//	BoxCollider(std::vector<System*>& systems, vec3 bounds, vec3 boxPos = vec3(0)) {
//		for (System* system : systems) {
//			this->systems.push_back(system);
//		}
//		this->bounds = bounds;
//	};
//
//	void apply() {
//		for (System* system : systems) {
//			double xPosLimitPlus = bounds.x + boxPos.x;
//			double xPosLimitMinus = -bounds.x + boxPos.x;
//			if (system->position.x > xPosLimitPlus || system->position.x < xPosLimitMinus) {
//				//std::cout << "bounce x" << std::endl;
//
//				system->velocity.x = -system->velocity.x;
//				if (system->position.x > xPosLimitPlus) {
//					double deltaX = abs(system->position.x - xPosLimitPlus);
//					system->position.x = xPosLimitPlus - deltaX;
//				}
//				else {
//					double deltaX = abs(system->position.x - xPosLimitMinus);
//					system->position.x = xPosLimitMinus + deltaX;
//				}
//			}
//			double yPosLimitPlus = bounds.y + boxPos.y;
//			double yPosLimitMinus = -bounds.y + boxPos.y;
//			if (system->position.y > yPosLimitPlus || system->position.y < yPosLimitMinus) {
//				//std::cout << "bounce y" << std::endl;
//
//				system->velocity.y = -system->velocity.y;
//				if (system->position.y > xPosLimitPlus) {
//					double deltaY = abs(system->position.y - yPosLimitPlus);
//					system->position.y = yPosLimitPlus - deltaY;
//				}
//				else {
//					double deltaY = abs(system->position.y - yPosLimitMinus);
//					system->position.y = yPosLimitMinus + deltaY;
//				}
//			}
//			double zPosLimitPlus = bounds.z + boxPos.z;
//			double zPosLimitMinus = -bounds.z + boxPos.z;
//			if (system->position.z > zPosLimitPlus || system->position.z < zPosLimitMinus) {
//				//std::cout << "bounce z" << std::endl;
//
//				system->velocity.z = -system->velocity.z;
//				if (system->position.z > xPosLimitPlus) {
//					double deltaZ = abs(system->position.z - zPosLimitPlus);
//					system->position.z = zPosLimitPlus - deltaZ;
//				}
//				else {
//					double deltaZ = abs(system->position.z - zPosLimitMinus);
//					system->position.y = zPosLimitMinus + deltaZ;
//				}
//			}
//		}
//	}
//};


//void simpleGravitationalOrbitSim(System simpleSystem, std::ofstream output) {
//	// define gravitational interaction between particles
//	//System& subsystems = simpleSystem.subsystems;
//	std::vector<System*> subsystemPtrs;
//	for (int i = 0; i < simpleSystem.subsystems.size(); i++) {
//		subsystemPtrs.push_back(&simpleSystem.subsystems[i]);
//	}
//	GravityDeprecated gravityBigG(subsystemPtrs);
//
//	if (output.is_open()) {
//		std::cerr << "output stream is open";
//	}
//	output.open("simulation.txt");
//
//	output << "t" << simpleSystem.time;
//	
//	for (int i = 0; i < simpleSystem.subsystems.size(); i++) {
//		System subsystem = simpleSystem.subsystems[i];
//		output << "; " << "p" << (i + 1) << ", " << subsystem.position.x << ", " << subsystem.position.y << ", " << subsystem.position.z;
//	}
//	output << std::endl;
//
//	const int orbits = 10;
//
//	// evolve the system over a single full orbit period    
//	while (simpleSystem.time < orbits * 2 * PI / sqrt(2)) {
//		for (int i = 0; i < simpleSystem.subsystems.size(); i++) {
//			for (int j = i + 1; j < simpleSystem.subsystems.size(); j++) {
//				gravityBigG.apply(&simpleSystem.subsystems[i], &simpleSystem.subsystems[j]);
//			}
//		}
//		simpleSystem.evolve();
//
//		output << "t" << simpleSystem.time;
//
//		for (int i = 0; i < simpleSystem.subsystems.size(); i++) {
//			System subsystem = simpleSystem.subsystems[i];
//			output << "; " << "p" << (i + 1) << ", " << subsystem.position.x << ", " << subsystem.position.y << ", " << subsystem.position.z;
//		}
//		output << std::endl;
//	}
//
//	// close files
//	output.close();
//};