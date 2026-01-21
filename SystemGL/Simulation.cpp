#include "Simulation.h"
#include "System.h"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <fstream>
#include <limits>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

Simulation::Simulation(System* s, std::string f, long double ts, long double ss) : systemPtr(s), file(f), timeScale(ts), spaceScale(ss) {}

System* Simulation::getSystemPtr() {
	return systemPtr;
}

void Simulation::run(long double frameTime, long double endTime, std::ostream& debug) {
	std::ofstream o;
	o.open(file);

	long double targetTime = getScaledTime()/* + frameTime*/;
	unsigned frameCount = 0;

	while (getScaledTime() < endTime) {

		if (getScaledTime() < targetTime) {
			systemPtr->evolve();
		}
		else {
			targetTime = getScaledTime() + frameTime;

			//o << "i:" << frameCount++ << ";t:" << getScaledTime();
			/*printFrameCount(frameCount, o);
			printFrameTime(o);
			printParticlePositions(o);
			o << std::endl;*/
			printLine2File(frameCount, o);

			int percentComplete = (int)(100.0l - (endTime - getScaledTime()) * 100.0l / endTime);

			if (percentComplete > 100) percentComplete = 100;
			else if (percentComplete < 0) percentComplete = 0;

			if (std::fmod(percentComplete, 1.0l) < frameTime || std::fmod(percentComplete, 1.0l) > 1.0l - frameTime) {
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
	std::getline(in, line);
	line = trim(line);

	if (!line.empty()) {
		iteration = std::stoi(extract_between(line, "i:", ";"));

		systemPtr->setTime(std::stold(extract_between(line, "t:", ";")));

		for (int i = 0; i < systemPtr->numberOfParticles(); i++) {
			Particle& particle = systemPtr->getParticle(i);

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
	return systemPtr->getTime() * timeScale;
}
vec3 Simulation::getScaledPosition(const vec3& position) {
	return vec3(position.x * spaceScale, position.y * spaceScale, position.z * spaceScale);
}
std::string Simulation::trim(std::string s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char c) { return !std::isspace(c); }));
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char c) { return !std::isspace(c); }).base(), s.end());
	return s;
};
std::string Simulation::extract_between(const std::string& s, const std::string& open, const std::string& close) {
	size_t a = s.find(open);
	if (a == std::string::npos) return "";
	a += open.size();
	size_t b = s.find(close, a);
	if (b == std::string::npos) return "";
	return s.substr(a, b - a);
};
void Simulation::printFrameCount(unsigned& i, std::ostream& o) {
	o << "i:" << i++ << ";";
}
void Simulation::printFrameTime(std::ostream& o) {
	o << "t:" << getScaledTime() << ";";
}
void Simulation::printParticlePositions(std::ostream& o) {
	for (int i = 0; i < systemPtr->numberOfParticles(); i++) {
		Particle& p = systemPtr->getParticle(i);
		vec3 position = getScaledPosition(p.getPosition());
		o << "p" << i << ":" << position.x << "," << position.y << "," << position.z << ";";
	}
}
void Simulation::printLine2File(unsigned& i, std::ostream& o) {
	printFrameCount(i, o);
	printFrameTime(o);
	printParticlePositions(o);
	o << std::endl;
}


PressureSimulation::PressureSimulation(PressureSystem* s, std::string posFile, std::string impFile, long double ts, long double ss) : Simulation(s, posFile, ts, ss) {
	impulseFile = impFile;
	pSystemPtr = s;
}

void PressureSimulation::run(long double frameTime, long double endTime, std::ostream& debug) {
	std::ofstream o;
	std::ofstream imp;
	o.open(file);
	imp.open(impulseFile);

	long double targetTime = getScaledTime()/* + frameTime*/;
	unsigned frameCount = 0;

	debug << "Simulation progress:\n";

	/*debug << "|";
	for (int i = 0; i < 100; i++) {
		debug << "-";
	}
	debug << "| 0 %";*/

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

			vec3 impulse = pSystemPtr->impulseEvolve();

			imp << "i:" << frameCount << ";t:" << systemPtr->getTime() << ";J:" << impulse.x << "," << impulse.y << "," << impulse.z << ";\n";
		}
		else {
			targetTime = getScaledTime() + frameTime;

			printLine2File(frameCount, o);

			int percentComplete = (int)(100.0l - (endTime - getScaledTime()) * 100.0l / endTime);

			if (std::fmod(percentComplete, 1.0l) < frameTime || std::fmod(percentComplete, 1.0l) > 1.0l - frameTime) {
				debug << "\r|";
				for (int i = 0; i < percentComplete; i++) {
					debug << "*";
				}
				for (int i = 0; i < 100 - percentComplete; i++) {
					debug << "-";
				}
				debug << "| " << percentComplete << " %";
			}

			/*if (std::fmod(100.0l - (endTime - getScaledTime()) * 100.0l / endTime, 1.0l) < frameTime) {
				std::ios oldState(nullptr);
				oldState.copyfmt(debug);

				debug << std::setprecision(0) << std::fixed << "Simulation " << 100.0l - (endTime - getScaledTime()) * 100.0l / endTime << " % complete." << std::endl;

				debug.copyfmt(oldState);
			}*/
		}
	}

	debug << "\r|";
	for (int i = 0; i < 100; i++) {
		debug << "*";
	}
	debug << "| " << 100 << " %";

	debug << std::endl;

	printLine2File(frameCount, o);

	o.close();
	imp.close();
}

vec3 PressureSimulation::readNextImpulse(long double& time) {
	openImpIn();

	std::string line;
	line = trim(line);

	vec3 impulse = vec3(0);

	if (std::getline(impIn, line)) {
		/*impulse.x = std::stold(extract_between(line, "J", ","));
		impulse.y = std::stold(extract_between(line, ",", ","));
		impulse.z = std::stold(extract_between(line, ",", ";"));*/

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
	Simulation::~Simulation();
	closeImpIn();
}