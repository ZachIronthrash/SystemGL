
#include "Mesh.h"
#include "Particle.h"
#include "Shader.h"
#include "System.h"
#include "SystemGLMath.h"
#include <exception>
#include <functional>
#include <glm/ext/matrix_transform.inl>
#include <glm/fwd.hpp>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

System::System(long double dt) : dt(dt) {}

// particles
void System::addParticle(Particle& particle) {
	// copy particle into owned storage, then store a stable reference
	//particles.push_back(std::make_unique<Particle>(particle));
	//particles.push_back(std::ref(*ownedParticles.back()));
	particles.push_back(particle);
}

//void System::addParticle(std::unique_ptr<Particle> particle) {
//	// transfer ownership into owned storage, then store a stable reference
//	particles.push_back(std::move(particle));
//	//particles.push_back(std::ref(*ownedParticles.back()));
//}

size_t System::numberOfParticles() {
	return particles.size();
}
//std::vector<std::unique_ptr<Particle>>& System::getParticles() {
//	return particles;
//}
std::vector<std::reference_wrapper<Particle>> System::getParticles() {
	return particles;
}

// getters
Particle& System::getParticle(int i) {
	//return std::ref(*particles[i]);
	return particles[i];
}
//std::unique_ptr<Particle> System::getParticlePtr(int i) {
//	return std::move(particles[i]);
//}
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
	try {
		//for (/*std::unique_ptr<Particle>*/Particle& particle : particles) {
		//	particle.get().translateBy(dt * particle.get().getVelocity());
		//}
		for (size_t i = 0; i < particles.size(); i++) {
			particles[i].get().translateBy(dt * particles[i].get().getVelocity());
		}
	}
	catch (std::exception& e) {
		std::cerr << "Error during particle evolution: " << e.what() << std::endl;
	}

	// step time once after all evolutions are complete
	timeStep();
}

void System::drawSystemParticles(Shader& shader, Mesh& mesh, unsigned fidelity, vec3 offset) {
	int i = 0;
	/*for (Particle particle : particles) {*/
	for (size_t i = 0; i < particles.size(); i++) {
		if (i % fidelity == 0) {
			glm::mat4 model = glm::mat4(1.0f);
			vec3 particlePos = particles[i].get().getPosition();
			model = glm::translate(model, glm::vec3(particlePos.x + offset.x, particlePos.y + offset.y, particlePos.z + offset.z));
			shader.setMat4("model", model);
			mesh.draw(shader);
		}
		i++;
	}
}

//#include "Mesh.h"
//#include "Particle.h"
//#include "Shader.h"
//#include "System.h"
//#include "SystemGLMath.h"
//#include <glm/ext/matrix_transform.inl>
//#include <glm/fwd.hpp>
//#include <vector>
//#include <exception>
//#include <functional>
//#include <iostream>
//
//System::System(long double dt) : dt(dt) {}
//
//// particles
//void System::addParticle(Particle& particle) {
//	particles.push_back(particle);
//}
//size_t System::numberOfParticles() {
//	return particles.size();
//}
//std::vector<std::reference_wrapper<Particle>>& System::getParticles() {
//	return particles;
//}
//
//// getters
//Particle& System::getParticle(int i) {
//	return particles[i];
//}
//long double System::getTime() {
//	return time;
//}
//long double System::getDt() {
//	return dt;
//}
//
//// setters
//void System::setTime(long double time) {
//	time = time;
//}
//void System::setDt(long double dt) {
//	this->dt = dt;
//}
//
//void System::timeStep() {
//	step++;
//	time += dt;
//};
//
//void System::evolve() {
//	// Evolve particles
//	try {
//		for (std::reference_wrapper<Particle> particle : particles) {
//			particle.get().translateBy(dt * particle.get().getVelocity());
//		}
//	}
//	catch (std::exception& e) {
//		std::cerr << "Error during particle evolution: " << e.what() << std::endl;
//	}
//
//	// step time once after all evolutions are complete
//	timeStep();
//}
//
//void System::drawSystemParticles(Shader& shader, Mesh& mesh, unsigned fidelity, vec3 offset) {
//	int i = 0;
//	for (Particle particle : particles) {
//		if (i % fidelity == 0) {
//			glm::mat4 model = glm::mat4(1.0f);
//			vec3 particlePos = particle.getPosition();
//			model = glm::translate(model, glm::vec3(particlePos.x + offset.x, particlePos.y + offset.y, particlePos.z + offset.z));
//			shader.setMat4("model", model);
//			mesh.draw(shader);
//		}
//		i++;
//	}
//}

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