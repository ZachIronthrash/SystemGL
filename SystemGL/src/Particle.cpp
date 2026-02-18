
#include "Particle.h"
#include "SystemGLMath.h"

Particle::Particle() {}
Particle::Particle(vec3 position, vec3 velocity, long double mass, long double t, long double dt) : position(position), velocity(velocity), mass(mass), time(t), dt(dt) {}

// setters
void Particle::setPosition(vec3 newPosition) {
	this->position = newPosition;
}
void Particle::setVelocity(vec3 newVelocity) {
	this->velocity = newVelocity;
}
void Particle::setMass(long double newMass) {
	this->mass = newMass;
}

// getters
vec3 Particle::getPosition() {
	return this->position;
}
vec3 Particle::getVelocity() {
	return this->velocity;
}
long double Particle::getMass() {
	return this->mass;
}
long double Particle::getTime() {
	return this->time;
}
long double Particle::getDt() {
	return this->dt;
}

// transformations
void Particle::translateBy(vec3 delta) {
	this->position += delta;
}
void Particle::accelerateBy(vec3 delta) {
	this->velocity += delta;
}

void Particle::pushTime() {
	this->time += this->dt;
}

long double Particle::calcKE() {
	return 0.5 * mass * velocity.dot(velocity);
};