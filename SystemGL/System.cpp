#include "System.h"

vec3::vec3() : x(0), y(0), z(0) {};
vec3::vec3(long double val) : x(val), y(val), z(val) {}
vec3::vec3(long double x, long double y, long double z) : x(x), y(y), z(z) {}

vec3 vec3::operator-() const {
	return vec3(-x, -y, -z);
}

vec3& vec3::operator+=(const vec3& other) {
	x += other.x; y += other.y; z += other.z;
	return *this;
}
vec3& vec3::operator-=(const vec3& other) {
	x -= other.x; y -= other.y; z -= other.z;
	return *this;
}
vec3& vec3::operator*=(long double scalar) {
	x *= scalar; y *= scalar; z *= scalar;
	return *this;
}
vec3& vec3::operator/=(long double scalar) {
	assert(scalar != 0/*, "division by zero"*/);

	x /= scalar; y /= scalar; z /= scalar;
	return *this;
}

vec3 vec3::normalized() const {
	long double mag = sqrt(x * x + y * y + z * z);
	if (mag == 0) return vec3(0.0f);
	else return vec3(x / mag, y / mag, z / mag);
}

vec3 operator+(vec3 a, vec3 b) {
	return vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}
vec3 operator-(vec3 a, vec3 b) {
	return vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}
vec3 operator*(vec3 lhs, long double rhs) {
	return vec3(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs);
}
vec3 operator*(long double lhs, vec3 rhs) {
	return vec3(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z);
}
vec3 operator/(vec3 lhs, long double rhs) {
	assert(rhs != 0/*, "division by zero"*/);

	return vec3(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs);
}
vec3 operator/(long double lhs, vec3 rhs) {
	assert(rhs.x != 0 && rhs.y != 0 && rhs.z != 0/*, "division by zero"*/);

	return vec3(lhs / rhs.x, lhs / rhs.y, lhs / rhs.z);
}

Particle::Particle() {}
Particle::Particle(vec3 position, vec3 velocity, long double mass/*, long double dt*/) : position(position), velocity(velocity), mass(mass)/*, dt(dt)*/ {}

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

// transformations
void Particle::translateBy(vec3 delta) {
	this->position += delta;
}
void Particle::accelerateBy(vec3 delta) {
	this->velocity += delta;
}

//Spring::Spring(double k) : k(k) {}
//
//vec3 Spring::force(vec3 displacement, vec3 velocity, double time) const {
//	return -displacement * k;
//}
//
//std::string Spring::getLabelXML() {
//	return "spring";
//}
//
//System::System() {}
//System::System(std::vector<std::unique_ptr<System>> subsystems, std::vector<Particle> particles, std::vector<Interaction> interactions) : subsystems(std::move(subsystems)), particles(particles), interactions(interactions) {}
//
//void System::setSubsystems(std::vector<std::unique_ptr<System>> subsystems) {
//	this->subsystems = std::move(subsystems);
//}
//void System::setParticles(std::vector<Particle> particles) {
//	this->particles = particles;
//}
//void System::setInteractions(std::vector<Interaction> interactions) {
//	this->interactions = interactions;
//}
//void System::setTime(double newTime) {
//	this->time = newTime;
//}
//void System::setDeltaT(double newDeltaT) {
//	this->deltaT = newDeltaT;
//}
//
//void System::addSubsystem(std::unique_ptr<System> subsystem) {
//	subsystems.push_back(std::move(subsystem));
//}
//void System::addParticle(Particle particle) {
//	this->particles.push_back(particle);
//}
//void System::addInteraction(Interaction interaction) {
//	this->interactions.push_back(interaction);
//}
//
//std::vector<std::unique_ptr<System>>& System::getSubsystems() {
//	return this->subsystems;
//}
//std::vector<Particle>& System::getParticles() {
//	return this->particles;
//}
//std::vector<Interaction>& System::getInteractions() {
//	return this->interactions;
//}
//Particle& System::getParticle(size_t k) {
//	return particles.at(k);
//}
//
//System* System::getSubsystem(size_t k) {
//	return subsystems.at(k).get();
//}
//Interaction& System::getInteraction(size_t k) {
//	return interactions.at(k);
//}
//double System::getTime() {
//	return this->time;
//}
//double System::getDeltaT() {
//	return this->deltaT;
//}
//
//size_t System::numberOfSubsystems() {
//	return subsystems.size();
//}
//size_t System::numberOfParticles() {
//	return particles.size();
//}
//size_t System::numberOfInteractions() {
//	return interactions.size();
//}
//
//// time handling
//void System::timeStep() {
//	// per contract: don't recurse into subsystems
//	this->time += this->deltaT;
//}
//
//void System::apply() {
//	// I will soon get to this
//}
//
//void System::evolve() {
//	// Evolve particles
//	for (size_t i = 0; i < numberOfParticles(); i++) {
//		getParticle(i).translateBy(getDeltaT() * getParticle(i).getVelocity());
//	}
//	// Evolve subsystems
//	for (size_t i = 0; i < numberOfSubsystems(); i++) {
//		getSubsystem(i)->evolve();
//	}
//
//	// step time once after all evolutions are complete
//	timeStep();
//}
//
//void BoundSystem::evolve() {
//	// Evolve particles
//	for (size_t i = 0; i < numberOfParticles(); i++) {
//		getParticle(i).translateBy(getDeltaT() * getParticle(i).getVelocity());
//
//		// only boundary condition is reflection for now
//		// (perfectly elastic collisions with immovable walls)
//		reflectParticle(getParticle(i));
//
//		//std::cout << getParticle(i).getPosition() << std::endl;
//	}
//	// Evolve subsystems
//	for (size_t i = 0; i < numberOfSubsystems(); i++) {
//		getSubsystem(i)->evolve();
//	}
//
//	//std::cout << std::endl;
//
//	// step time once after all evolutions are complete
//	timeStep();
//}
//
//Simulation::Simulation(System& system, std::string systemFile, std::string outputFile) : system(system), systemFile(systemFile), outputFile(outputFile) {
//	std::ofstream systemState(systemFile);
//
//	outputSystemState(system, systemState);
//
//	systemState.close();
//}