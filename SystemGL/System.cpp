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

long double Particle::calcKE() {
	return 0.5 * mass * velocity.dot(velocity);
};

System::System(long double dt) : dt(dt) {}

// particles
void System::addParticle(Particle& particle) {
	particles.push_back(particle);
}
size_t System::numberOfParticles() {
	return particles.size();
}
std::vector<Particle>& System::getParticles() {
	return particles;
}

// getters
Particle& System::getParticle(int i) {
	return particles[i];
}
long double System::getTime() {
	return time;
}
long double System::getDt() {
	return dt;
}

// setters
void System::setTime(long double time) {
	time = time;
}
void System::setDt(long double dt) {
	this->dt = dt;
}

void System::timeStep() {
	step++;
	time += dt;
};

void System::evolve() {
	// Evolve particles
	for (Particle& particle : particles) {
		particle.translateBy(dt * particle.getVelocity());
	}

	// step time once after all evolutions are complete
	timeStep();
}

void System::drawSystemParticles(Shader& shader, Mesh& mesh, unsigned fidelity, vec3 offset) {
	int i = 0;
	for (Particle particle : particles) {
		if (i % fidelity == 0) {
			glm::mat4 model = glm::mat4(1.0f);
			vec3 particlePos = particle.getPosition();
			model = glm::translate(model, glm::vec3(particlePos.x + offset.x, particlePos.y + offset.y, particlePos.z + offset.z));
			shader.setMat4("model", model);
			mesh.draw(shader);
		}
		i++;
	}
}

PressureSystem::PressureSystem(int numParticles, long double sysmass, long double molarMass, long double targetTemp, std::mt19937& generator, vec3 boxSize, long double dt) : boxSize(boxSize) {
	setDt(dt);

	//long double temp = 292l;

	//double moles = sysmass / molarMass; 

	long double scale_parameter = BOLTZMANN * targetTemp * MOLE / molarMass;

	std::gamma_distribution<long double> maxwell_dist(3.0 / 2.0, scale_parameter);

	std::uniform_real_distribution<long double> uniform_dist(-1.0, 1.0);

	for (int i = 0; i < numParticles; i++) {
		vec3 randomPos(
			uniform_dist(generator) * boxSize.x,
			uniform_dist(generator) * boxSize.y,
			uniform_dist(generator) * boxSize.z
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
		if (randomVel.x > 2 * boxSize.x / dt) {
			double scale = boxSize.x / (randomVel.x * dt);
			scalingFactor = scale;
			correction = true;
		}
		if (randomVel.y > 2 * boxSize.y / dt) {
			double scale = boxSize.y / (randomVel.y * dt);
			scalingFactor = (scale < scalingFactor) ? scale : scalingFactor;
			correction = true;
		}
		if (randomVel.z > 2 * boxSize.z / dt) {
			double scale = boxSize.z / (randomVel.z * dt);
			scalingFactor = (scale < scalingFactor) ? scale : scalingFactor;
			correction = true;
		}

		if (correction) {
			randomVel *= scalingFactor;
		}*/

		Particle particle(randomPos, randomVel, sysmass / numParticles);
		addParticle(particle);
	}

	/*std::vector<float> vertices;
	std::vector<unsigned int> indices;

	createVertexAndIndexData(vertices, indices);

	this->boxMesh = Mesh(vertices, indices);*/

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
* @returns a 6 element array with the impulse on each wall of the box: +x, -x, +y, -y, +z, -z
*/
/*std::array<long double, 6>*/ vec3 PressureSystem::reflectParticles() {
	/*long double impPlusX = 0;
	long double impMinusX = 0;
	long double impPlusY = 0;
	long double impMinusY = 0;
	long double impPlusZ = 0;
	long double impMinusZ = 0;*/
	long double impX = 0l;
	long double impY = 0l;
	long double impZ = 0l;

	for (Particle& particle : getParticles()) {
		vec3 position = particle.getPosition();
		long double mass = particle.getMass();

		vec3 newP = position;
		vec3 newV = particle.getVelocity();

		if (position.x > boxSize.x) {
			long double difference = position.x - boxSize.x;

			newP.x = boxSize.x - difference;
			newV.x = -newV.x;
			impX += abs(2 * newV.x * mass);
		}
		else if (position.x < -boxSize.x) {
			long double difference = -position.x - boxSize.x;

			newP.x = -boxSize.x + difference;
			newV.x = -newV.x;
			impX += abs(2 * newV.x * mass);
		}

		if (position.y > boxSize.y) {
			long double difference = position.y - boxSize.y;

			newP.y = boxSize.y - difference;
			newV.y = -newV.y;
			impY += abs(2 * newV.y * mass);
		}
		else if (position.y < -boxSize.y) {
			long double difference = -position.y - boxSize.y;

			newP.y = -boxSize.y + difference;
			newV.y = -newV.y;
			impY += abs(2 * newV.y * mass);
		}

		if (position.z > boxSize.z) {
			long double difference = position.z - boxSize.z;

			newP.z = boxSize.z - difference;
			newV.z = -newV.z;
			impZ += abs(2 * newV.z * mass);
		}
		else if (position.z < -boxSize.z) {
			long double difference = -position.z - boxSize.z;

			newP.z = -boxSize.z + difference;
			newV.z = -newV.z;
			impZ += abs(2 * newV.z * mass);
		}

		particle.setPosition(newP);
		particle.setVelocity(newV);
	}

	return { impX, impY, impZ };
}

/*std::array<long double, 6>*/ vec3 PressureSystem::impulseEvolve() {
	System::evolve();

	return reflectParticles();
}

// ARCHIVAL CODE
// -------------

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