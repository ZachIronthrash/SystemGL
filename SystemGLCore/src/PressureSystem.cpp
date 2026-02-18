#include "pch.h"
#include "Particle.h"
#include "PressureSystem.h"
#include "System.h"
#include "SystemGLMath.h"

PressureSystem::PressureSystem(int numParticles, long double sysmass, long double molarMass, long double targetTemp, std::mt19937& generator, vec3 boxSize, long double dt) : boxSize(boxSize) {
	/*setDt(dt);*/

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

		//Particle particle(randomPos, randomVel, sysmass / numParticles);
		//addParticle(*new Particle(randomPos, randomVel, sysmass / numParticles));
		createParticle(randomPos, randomVel, sysmass, numParticles, dt);
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

	for (std::reference_wrapper<Particle> particle : getParticles()) {
		vec3 position = particle.get().getPosition();
		long double mass = particle.get().getMass();

		vec3 newP = position;
		vec3 newV = particle.get().getVelocity();

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

		particle.get().setPosition(newP);
		particle.get().setVelocity(newV);
	}

	return { impX, impY, impZ };
}

/*std::array<long double, 6>*/ vec3 PressureSystem::impulseEvolve() {
	System::evolve();

	return reflectParticles();
}