#pragma once

#include "SystemGLMath.h"

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
	*
	* @ensures this->position = position, this->velocity = velocity, this->mass = mass
	*/
	Particle(vec3 position, vec3 velocity, long double mass, long double t = 0.0l, long double dt = 0.001l);

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
	* @return position
	*/
	vec3 getPosition();
	/*
	* fetches the velocity of the particle
	*
	* @return velocity
	*/
	vec3 getVelocity();
	/*
	* fetches the mass of the particle
	*
	* @return mass
	*/
	long double getMass();
	/*
	* fetches the time of the particle
	*
	* @return time
	*/
	long double getTime();
	/*
	* fetches the dt of the particle
	*
	* return dt
	*/
	long double getDt();

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

	/*
	* iterates time by dt
	*
	* @ensures time = #time + dt
	*/
	void pushTime();

	/*
	* calculates the KE from velocity and mass
	*
	* @return 0.5 * MASS * |VELOCITY| * |VELOCITY|
	*/
	long double calcKE();

	virtual bool operator==(const Particle&) const = default;

protected:
	vec3 position = vec3(0); // [m]
	vec3 velocity = vec3(0); // [m/s]
	long double mass = 1.0l; // [kg]

	long double time = 0.0l; // [s]
	const long double dt = 0.001l; // [s]
};