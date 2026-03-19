#include "pch.h"
#include "Simulation2.h"

// #inculde "System.h"
#include "Temp.h"
#include "SystemGLStringUtil.h"
#include "SystemGLMath.h"
#include "Particle.h"
#include <iomanip>
#include <limits>

const long double BAR_EPSILON = 0.00000000000001l;

Simulation::Simulation(std::string f, long double ts, long double ss) : file(f), timeScale(ts), spaceScale(ss) {}

//System& Simulation::getSystem() {
//	return system;
//}

void Simulation::run(System& s, long double frameTime, long double endTime, std::ofstream& action) {
	particleCount = s.nodeCount();

	std::ofstream o;
	o.open(file);

	long double targetTime = frameTime;
	unsigned frameCount = 0;

	long double actionSum = 0.0l;

	action << std::setprecision(std::numeric_limits<long double>::max_digits10);

	printLine2File(s, frameCount, o);

	long double firstAction = 0.0l;
	bool first = true;

	int count = 0;

	while (getScaledTime(s) < endTime) {
		if (getScaledTime(s) <= targetTime) {
			//s.evolve();
			s.pushVelocity();
			s.pushPosition();

			// L * dt = ( T - V ) * dt should be constant
			//long double L = s.lagrangianSum();
			long double L = s.kineticEnergy() - s.potentialEnergy();
			//actionSum += L * s.getParticle(0).getDt();
			//action << "Action: " << actionSum << " J * s" << std::endl;
			action << "Lagrangian: " << L << std::endl;

			if (first) {
				first = !first;
				firstAction = actionSum;
			}

			count++;
			//std::cout << system.lagrangianSum() * system.getParticle(0).getDt() << std::endl;
		}
		else {
			//std::cout << count << std::endl;
			count = 0;

			targetTime = getScaledTime(s) + frameTime;

			//o << "i:" << frameCount++ << ";t:" << getScaledTime();
			/*printFrameCount(frameCount, o);
			printFrameTime(o);
			printParticlePositions(o);
			o << endl;*/
			printLine2File(s, frameCount, o);

			//action << actionSum << " J * s" << std::endl;
			actionSum = 0.0l;

			/*int percentComplete = (int)(100.0l - (endTime - getScaledTime()) * 100.0l / endTime);

			if (percentComplete > 100) percentComplete = 100;
			else if (percentComplete < 0) percentComplete = 0;

			if (fmod(percentComplete, 1.0l) < frameTime || fmod(percentComplete, 1.0l) > 1.0l - frameTime) {
				std::cout << "\r|";
				for (int i = 0; i < percentComplete; i++) {
					std::cout << "*";
				}
				for (int i = 0; i < 100 - percentComplete; i++) {
					std::cout << "-";
				}
				std::cout << "| " << percentComplete << " %";
			}*/

			int percentComplete = (int)(100.0l - (endTime - getScaledTime(s)) * 100.0l / endTime);

			if (fmod(percentComplete, 1.0l) < BAR_EPSILON || fmod(percentComplete, 1.0l) > 1.0l - BAR_EPSILON) {
				std::cout << "\r|";
				for (int i = 0; i < percentComplete; i++) {
					std::cout << "*";
				}
				for (int i = 0; i < 100 - percentComplete; i++) {
					std::cout << "-";
				}
				std::cout << "| " << percentComplete << " %";
			}
		}
	}

	//action << actionSum << " J * s" << std::endl;

	action << "Change in action: " << actionSum - firstAction;
	action << std::endl << "Initial action: " << firstAction;
	action << std::endl << "Final action: " << actionSum << std::endl;

	std::cout << "\r|";
	for (int i = 0; i < 100; i++) {
		std::cout << "*";
	}
	std::cout << "| " << 100 << " %";

	std::cout << std::endl;

	printLine2File(s, frameCount, o);

	o.close();
}

unsigned Simulation::readNext() {
	openIn();

	std::string line;
	getline(in, line);
	line = trim(line);

	if (!line.empty()) {
		iteration = stoi(extract_between(line, "i:", ";"));

		//system.setTime(stold(extract_between(line, "t:", ";")));

		positions.clear();

		for (int i = 0; i < particleCount; i++) {
			//Particle& particle = system.getParticle(i);

			//particle.setTime(stold(extract_between(line, "t:", ";")));

			std::string part = extract_between(line, "p" + std::to_string(i) + ":", ";");
			std::stringstream ss(part);

			char discard;

			vec3 pos = vec3(0);

			ss >> pos.x >> discard >> pos.y >> discard >> pos.z;

			//particle.setPosition(pos);

			//std::cout << pos << std::endl;

			positions.push_back({ pos.x, pos.y, pos.z });
		}
	}
	else {
		iteration = std::numeric_limits<unsigned int>::max();

		std::cout << "FUCK" << std::endl;
	}

	return iteration;
}

void Simulation::resetIn() {
	//closeIn();

	// instead of closing every time we want to reset, just clear and seek the start
	//	this is mainly because "r" can reset the program every frame if the user desires it
	//	for which closing and re-opening the input stream every time is just plain stupid
	in.clear();
	in.seekg(0, std::ios::beg);
	iteration = 0;
}

void Simulation::openIn() {
	if (!in.is_open()) {
		in.open(file);
	}
}
void Simulation::closeIn() {
	if (in.is_open()) {
		in.close();
	}
}

Simulation::~Simulation() {
	closeIn();
}

long double Simulation::getScaledTime(System& s) {
	// TERRIBLE SOLUTION FIX BEFORE RELATIVITY
	return s.particles().at(0).get().getTime() * timeScale;
}
vec3 Simulation::getScaledPosition(const vec3& position) {
	return vec3(position.x * spaceScale, position.y * spaceScale, position.z * spaceScale);
}
//void Simulation::printFrameCount(unsigned& i, ostream& o) {
//	o << "i:" << i++ << ";";
//}
//void Simulation::printFrameTime(ostream& o) {
//	o << "t:" << getScaledTime() << ";";
//}
//void Simulation::printParticlePositions(ostream& o) {
//	for (int i = 0; i < systemPtr->numberOfParticles(); i++) {
//		Particle& p = systemPtr->getParticle(i);
//		vec3 position = getScaledPosition(p.getPosition());
//		o << "p" << i << ":" << position.x << "," << position.y << "," << position.z << ";";
//	}
//}
void Simulation::printIntToFile(std::string prefix, unsigned i, std::string suffix, std::ostream& o) {
	o << prefix << i << suffix;
}
void Simulation::printLongDoubleToFile(std::string prefix, long double ld, std::string suffix, std::ostream& o) {
	o << prefix << ld << suffix;
}
void Simulation::printVec3ToFile(std::string prefix, vec3 v, std::string suffix, std::ostream& o) {
	o << prefix << v.x << "," << v.y << "," << v.z << suffix;
}

void Simulation::printParticlePositions(System& s, std::ostream& o) {
	int i = 0;
	for (Particle& p : s.particles()) {
		vec3 position = getScaledPosition(p.getPosition());
		printVec3ToFile("p" + std::to_string(i) + ":", position, ";", o);
		i++;
	}
}

void Simulation::printLine2File(System& s, unsigned& i, std::ostream& o) {
	/*printFrameCount(i, o);
	printFrameTime(o);
	printParticlePositions(o);*/

	printIntToFile("i:", i, ";", o);
	i++;
	printLongDoubleToFile("t:", getScaledTime(s), ";", o);
	printParticlePositions(s, o);

	o << std::endl;
}