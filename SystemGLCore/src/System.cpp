#include "pch.h"
#include "Mesh.h"
#include "Particle.h"
#include "Shader.h"
#include "System.h"
#include "SystemGLMath.h"

System::System() {}

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

void System::createParticle(vec3 position, vec3 velocity, long double mass, long double t, long double dt) {
	particles.push_back(*new Particle(position, velocity, mass, t, dt));
}

void System::addSubsystem(std::shared_ptr<System> sub) {
	subsystems.push_back(sub);
}

void System::createInteraction(Potential V, Particle& p1, Particle& p2) {
	if (interactionExists(V, p1, p2)) {
		throw std::invalid_argument("Interaction with particle pair already exists");
	}

	interactions.push_back(Interaction(p1, p2, V));
}
void System::createUniversalInteraction(Potential V) {
	if (universalInteractionExists(V)) {
		throw std::invalid_argument("Universal interaction already contained in system");
	}

	universalInteractions.push_back(UniversalInteraction(particles, V));
	/*for (size_t i = 0; i < particles.size(); i++) {
		universalInteractions.push_back(std::ref(*particles.back()[i]));
	}*/
}

void System::interconnectWithPotential(Potential V) {
	for (size_t i = 0; i < particles.size(); i++) {
		for (size_t j = i + 1; j < particles.size(); j++) {
			createInteraction(V, particles[i], particles[j]);
		}
	}
}

void System::addParticle2Universal(int uIndex, int pIndex) {
	universalInteractions[uIndex].addPart(particles[pIndex]);
}

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

bool System::interactionExists(Potential V, Particle& p1, Particle& p2) {
	for (Interaction interaction : interactions) {
		if (interaction.isInteraction(V, p1, p2)) {
			//std::cout << "TRUE" << std::endl;
			return true;
		}
	}
	//std::cout << "FALSE" << std::endl;
	return false;
}
bool System::universalInteractionExists(Potential V) {
	for (UniversalInteraction interaction : universalInteractions) {
		if (interaction.hasPotential(V)) {
			return true;
		}
	}
	return false;
}

void System::updateUniversalInteractions() {
	updateUniversalInteractions(universalInteractions);
}

bool System::hasParticle(Particle& ref) {
	for (Particle& p : particles) {
		if (ref == p) {
			return true;
		}
	}
	return false;
}

bool System::hasParticleInSubsystems(Particle& ref) {
	for (const std::shared_ptr<System> sub : subsystems) {
		if (sub->hasParticle(ref) || sub->hasParticleInSubsystems(ref)) {
			return true;
		}
	}
	return false;
}

void System::evolve() {
	// Evolve particles
	try {
		for (Interaction& i : interactions) {
			i.recordDisplacement();
			i.recordRelativeVelocity();
		}
		for (Interaction& i : interactions) {
			i.apply();
		}
		for (UniversalInteraction& u : universalInteractions) {
			u.apply();
		}

		//for (/*std::unique_ptr<Particle>*/Particle& particle : particles) {
		//	particle.get().translateBy(dt * particle.get().getVelocity());
		//}
		for (size_t i = 0; i < particles.size(); i++) {
			Particle& p = particles[i].get();
			particles[i].get().translateBy(p.getDt() * p.getVelocity());

			if (subsystems.size() == 0) {
				p.pushTime();
			}
		}

		for (std::shared_ptr<System> sub : subsystems) {
			sub->evolve();
		}
	}
	catch (std::exception& e) {
		std::cerr << "Error during particle evolution: " << e.what() << std::endl;
	}

	//// Evolve particles
	//try {
	//	//for (/*std::unique_ptr<Particle>*/Particle& particle : particles) {
	//	//	particle.get().translateBy(dt * particle.get().getVelocity());
	//	//}
	//	for (size_t i = 0; i < particles.size(); i++) {
	//		particles[i].get().translateBy(dt * particles[i].get().getVelocity());
	//	}
	//}
	//catch (std::exception& e) {
	//	std::cerr << "Error during particle evolution: " << e.what() << std::endl;
	//}

	//// step time once after all evolutions are complete
	//timeStep();
}

//void System::drawSystemParticles(Shader& shader, Mesh& mesh, unsigned fidelity, vec3 offset) {
//	//int i = 0;
//	/*for (Particle particle : particles) {*/
//	for (size_t i = 0; i < particles.size(); i++) {
// 		if (i % fidelity == 0) {
//			glm::mat4 model = glm::mat4(1.0f);
//			vec3 particlePos = particles[i].get().getPosition();
//			model = glm::translate(model, glm::vec3(particlePos.x + offset.x, particlePos.y + offset.y, particlePos.z + offset.z));
//			shader.setMat4("model", model);
//			mesh.draw(shader);
//			//std::cout << i << std::endl;
//		}
//		//i++;
//	}
//}

void System::drawSystemParticles(Shader& objectShader, Shader& lightingShader, Mesh& mesh, float particleScale, unsigned fidelity, vec3 offset) {
	//int i = 0;
	/*for (Particle particle : particles) {*/
	for (size_t i = 0; i < particles.size(); i++) {
	 	if (i % fidelity == 0) {
			glm::mat4 model = glm::mat4(1.0f);
			vec3 particlePos = particles[i].get().getPosition();
			model = glm::translate(model, glm::vec3(particlePos.x + offset.x, particlePos.y + offset.y, particlePos.z + offset.z));
			model = glm::scale(model, glm::vec3(particleScale));
			model = glm::rotate(model, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			//shader.setMat4("model", model);
			//mesh.draw(shader);
			objectShader.setMat4("model", model);
			mesh.draw(objectShader);
			//std::cout << i << std::endl;
		}
		//i++;
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