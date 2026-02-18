#include "Simulation.h"
#include "System.h"
#include "SystemGLStringUtil.h"
#include <cmath>
#include <fstream>
#include <limits>
#include <ostream>
#include <Particle.h>
#include <sstream>
#include <string>
#include <SystemGLMath.h>

Simulation::Simulation(System& s, std::string f, long double ts, long double ss) : system(s), file(f), timeScale(ts), spaceScale(ss) {}

System& Simulation::getSystem() {
	return system;
}

void Simulation::run(long double frameTime, long double endTime, std::ostream& debug) {
	std::ofstream o;
	o.open(file);

	long double targetTime = getScaledTime()/* + frameTime*/;
	unsigned frameCount = 0;

	while (getScaledTime() < endTime) {

		if (getScaledTime() < targetTime) {
			system.evolve();
		}
		else {
			targetTime = getScaledTime() + frameTime;

			//o << "i:" << frameCount++ << ";t:" << getScaledTime();
			/*printFrameCount(frameCount, o);
			printFrameTime(o);
			printParticlePositions(o);
			o << endl;*/
			printLine2File(frameCount, o);

			int percentComplete = (int)(100.0l - (endTime - getScaledTime()) * 100.0l / endTime);

			if (percentComplete > 100) percentComplete = 100;
			else if (percentComplete < 0) percentComplete = 0;

			if (fmod(percentComplete, 1.0l) < frameTime || fmod(percentComplete, 1.0l) > 1.0l - frameTime) {
				debug << "\r|";
				for (int i = 0; i < percentComplete; i++) {
					debug << "*";
				}
				for (int i = 0; i < 100 - percentComplete; i++) {
					debug << "-";
				}
				debug << "| " << percentComplete << " %";
			}
		}
	}

	printLine2File(frameCount, o);

	o.close();
}

unsigned Simulation::readNext() {
	openIn();

	std::string line;
	getline(in, line);
	line = trim(line);

	if (!line.empty()) {
		iteration = stoi(extract_between(line, "i:", ";"));

		system.setTime(stold(extract_between(line, "t:", ";")));

		for (int i = 0; i < system.numberOfParticles(); i++) {
			Particle& particle = system.getParticle(i);

			std::string part = extract_between(line, "p" + std::to_string(i) + ":", ";");
			std::stringstream ss(part);

			char discard;

			vec3 pos = vec3(0);

			ss >> pos.x >> discard >> pos.y >> discard >> pos.z;

			particle.setPosition(pos);
		}
	}
	else {
		iteration = std::numeric_limits<unsigned int>::max();
	}

	return iteration;
}

void Simulation::resetIn() {
	closeIn();
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

long double Simulation::getScaledTime() {
	return system.getTime() * timeScale;
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

void Simulation::printParticlePositions(std::ostream& o) {
	for (int i = 0; i < system.numberOfParticles(); i++) {
		Particle& p = system.getParticle(i);
		vec3 position = getScaledPosition(p.getPosition());
		printVec3ToFile("p" + std::to_string(i) + ":", position, ";", o);
	}
}

void Simulation::printLine2File(unsigned& i, std::ostream& o) {
	/*printFrameCount(i, o);
	printFrameTime(o);
	printParticlePositions(o);*/

	printIntToFile("i:", i, ";", o);
	i++;
	printLongDoubleToFile("t:", getScaledTime(), ";", o);
	printParticlePositions(o);

	o << std::endl;
}