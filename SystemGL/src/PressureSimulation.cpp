#include "PressureSimulation.h"
#include "PressureSystem.h"
#include "Simulation.h"
#include "SystemGLMath.h"
#include "SystemGLStringUtil.h"
#include <cmath>
#include <fstream>
#include <limits>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

PressureSimulation::PressureSimulation(PressureSystem& s, std::string posFile, std::string impFile, long double ts, long double ss) : Simulation(s, posFile, ts, ss), impulseFile(impFile), system(s) {}

void PressureSimulation::run(long double frameTime, long double endTime, std::ofstream& action) {
	std::ofstream o;
	std::ofstream imp;
	o.open(file);
	imp.open(impulseFile);
	assert(imp.is_open());

	long double targetTime = getScaledTime()/* + frameTime*/;
	unsigned frameCount = 0;

	std::cout << "Simulation progress:\n";

	/*std::cout << "|";
	for (int i = 0; i < 100; i++) {
		std::cout << "-";
	}
	std::cout << "| 0 %";*/

	while (getScaledTime() < endTime) {
		std::vector<vec3> initVelocities;
		if (getScaledTime() < targetTime) {
			//initVelocities.clear();
			//for (int i = 0; i < system->numberOfParticles(); i++) {
			//	Particle& p = system->getParticle(i);
			//	initVelocities.push_back(p.getVelocity());
			//}
			//system->evolve();
			//vec3 totalStepImpulse = vec3(0);
			//for (int i = 0; i < system->numberOfParticles(); i++) {
			//	// DOESN'T ACCOUNT FOR INTERACTIONS WITHIN THE SYSTEM
			//	Particle& p = system->getParticle(i);
			//	vec3 deltaV = p.getVelocity() - initVelocities[i];
			//	vec3 impulse = deltaV * p.getMass();
			//	totalStepImpulse.x += abs(impulse.x);
			//	totalStepImpulse.y += abs(impulse.y);
			//	totalStepImpulse.z += abs(impulse.z);
			//}
			//imp << "i" << frameCount << ";t" << system->getTime() << ";J" << totalStepImpulse.x << "," << totalStepImpulse.y << "," << totalStepImpulse.z << ";\n";

			vec3 impulse = system.impulseEvolve();

			/*volatile*/ long double tx = impulse.x;
			/*volatile*/ long double ty = impulse.y;
			/*volatile*/ long double tz = impulse.z;

			//imp << "i:" << frameCount << ";t:" << system.getTime() << ";J:" << impulse.x << "," << impulse.y << "," << impulse.z << ";\n";
			imp << "i:" << frameCount << ";t:" << system.getTime() << ";J:" << tx << "," << ty << "," << tz << ";\n";
		}
		else {
			targetTime = getScaledTime() + frameTime;

			printLine2File(frameCount, o);

			int percentComplete = (int)(100.0l - (endTime - getScaledTime()) * 100.0l / endTime);

			if (fmod(percentComplete, 1.0l) < frameTime || fmod(percentComplete, 1.0l) > 1.0l - frameTime) {
				std::cout << "\r|";
				for (int i = 0; i < percentComplete; i++) {
					std::cout << "*";
				}
				for (int i = 0; i < 100 - percentComplete; i++) {
					std::cout << "-";
				}
				std::cout << "| " << percentComplete << " %";
			}

			/*if (fmod(100.0l - (endTime - getScaledTime()) * 100.0l / endTime, 1.0l) < frameTime) {
				ios oldState(nullptr);
				oldState.copyfmt(std::cout);

				std::cout << setprecision(0) << fixed << "Simulation " << 100.0l - (endTime - getScaledTime()) * 100.0l / endTime << " % complete." << endl;

				std::cout.copyfmt(oldState);
			}*/
		}
	}

	std::cout << "\r|";
	for (int i = 0; i < 100; i++) {
		std::cout << "*";
	}
	std::cout << "| " << 100 << " %";

	std::cout << std::endl;

	printLine2File(frameCount, o);

	o.close();
	imp.close();
}

vec3 PressureSimulation::readNextImpulse(long double& time) {
	openImpIn();

	std::string line;
	line = trim(line);

	vec3 impulse = vec3(0);

	if (getline(impIn, line)) {
		/*impulse.x = stold(extract_between(line, "J", ","));
		impulse.y = stold(extract_between(line, ",", ","));
		impulse.z = stold(extract_between(line, ",", ";"));*/

		std::string part = extract_between(line, "J:", ";");
		std::stringstream ss0(part);

		char discard;

		ss0 >> impulse.x >> discard >> impulse.y >> discard >> impulse.z;

		part = extract_between(line, "t:", ";");

		std::stringstream ss1(part);

		ss1 >> time;
	}
	else {
		impulse = vec3(std::numeric_limits<long double>::min());
	}

	return impulse;
}

void PressureSimulation::openImpIn() {
	if (!this->impIn.is_open()) {
		this->impIn.open(impulseFile);
	}
}
void PressureSimulation::closeImpIn() {
	if (this->impIn.is_open()) {
		this->impIn.close();
	}
}

PressureSimulation::~PressureSimulation() {
	closeImpIn();
}