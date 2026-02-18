#pragma once

#include "System.h"
#include "SystemGLMath.h"
#include <random>

class PressureSystem : public System {
public:
	vec3 boxSize; // [m] : halfwidth, halfheight

	PressureSystem() : System() {}

	/*
	* default constructor
	*
	* @param numParticles	- the number of particles in the system
	* @param sysmass		- the total mass of the particles to approximate
	* @param molarMass		- the molar mass of the particles to approximate
	* @param targetTemp		- the target temperature of the randomly generated system
	* @param generator		- the rng to sample
	* @param boxSize		- the size of the bounding box
	* @param dt				- initial value of dt for the system
	*
	* @ensures for all particles mass = sysmass / numParticles
	* @ensures this->dt = dt
	* @ensures particle velocities are randomly assigned using generator and the correct distribution is used for supplied targetTemp & molarMass
	* @ensures this->boxSize = boxSize
	* @ensures a working mesh is created for the system boundaries with vertices according to boxSize
	*/
	PressureSystem(int numParticles, long double sysmass, long double molarMass, long double targetTemp, std::mt19937& generator, vec3 boxSize = vec3(1, 1, 1), long double dt = 0.001l);

	/*
	* reflects the particles which exited the box and returns the total change in impulse
	*
	* @ensures |particle.position| < boxSize
	*	|particle.position.x| = 2 * boxSize.x - |#particle.position.x| if |particle.position.x| > boxSize
	*	|particle.position.y| = 2 * boxSize.y - |#particle.position.y| if |particle.position.y| > boxSize
	*	|particle.position.z| = 2 * boxSize.z - |#particle.position.z| if |particle.position.z| > boxSize
	* @ensures |DISTANCE_TRAVELED| = |#DISTANCE_TRAVELED| for each particle
	* @ensures particle.velocity is flipped according to reflection direction for each particle
	* @return sum(MASS * ΔVELOCITY for each particle)
	*/
	vec3 reflectParticles();

	/*
	* calls System::evolve() then reflectParticles() returning the impulses
	*
	* @return reflectParticles()
	*/
	vec3 impulseEvolve();

	void evolve() override {
		impulseEvolve();
	}

	/*
	* draws boxMesh to screen with the supplied shader and offset
	*
	* @param shader		- the shader to use when drawing
	* @param offset		- a vector describing the offset to apply to the mesh before drawing
	*
	* @ensures mesh.positions = #mesh.positions + offset
	* @ensures mesh is drawn to screen at correct position with the supplied shader
	*/
	//void drawSystemBounds(Shader& shader, vec3 offset = 0.0l);

private:
	//Mesh boxMesh;

	/*
	* creates vertex and index data for the bounding box
	*
	* @param vertices	- the container for the vertex data
	* @param indices	- the container for the index data
	*
	* @updates vertices and indices with data for the bounding box of the system
	* @clears vertices
	* @clears indices
	*
	* @ensures vertices describe a square behind all particles in the system
	* @ensures indices correctly order vertices to draw desired structure
	*/
};