#pragma once
#include <fstream>

#include <string>

#include <sstream>

#include <ios>

#include "System.h"

const long double PRINT_EPSILON = 1e-2; // for use in printing and other imprecise applications

class Simulation {
public:
	Simulation(System* s, std::string f, long double ts, long double ss) : system(s), file(f), timeScale(ts), spaceScale(ss) {}

	System* systemPtr() {
		return system;
	}

	virtual void run(long double frameTime, long double endTime, std::ostream& debug) {
		std::ofstream o;
		o.open(file);

		long double targetTime = getScaledTime()/* + frameTime*/;
		unsigned frameCount = 0;

		while (getScaledTime() < endTime) {

			if (getScaledTime() < targetTime) {
				system->evolve();
			}
			else {
				targetTime = getScaledTime() + frameTime;
				o << "i" << frameCount++ << ";t" << getScaledTime();
				for (int i = 0; i < system->numberOfParticles(); i++) {
					Particle& p = system->getParticle(i);
					vec3 position = getScaledPosition(p.getPosition());
					o << ";" << "p" << i << "," << position.x << "," << position.y << "," << position.z;
				}
				o << ";\n";

				debug << "Simulation " << 100.0l - (endTime - getScaledTime()) * 100.0l / endTime << " % complete." << std::endl;
			}
		}

		o << "i" << frameCount++ << ";t" << getScaledTime();
		for (int i = 0; i < system->numberOfParticles(); i++) {
			Particle& p = system->getParticle(i);
			vec3 position = getScaledPosition(p.getPosition());
			o << ";" << "p" << i << "," << position.x << "," << position.y << "," << position.z;
		}
		o << ";\n";

		o.close();
	}

	virtual unsigned readNext() {
		openIn();

		std::string line;
		std::getline(in, line);
		line = trim(line);
		
		if (!line.empty()) {
			iteration = std::stoi(extract_between(line, "i", ";"));

			system->setTime(std::stold(extract_between(line, "t", ";")));

			for (int i = 0; i < system->numberOfParticles(); i++) {
				Particle& particle = system->getParticle(i);

				std::string part = extract_between(line, "p" + std::to_string(i) + ",", ";");
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

	virtual void resetIn() {
		closeIn();
		iteration = 0;
	}

	virtual void openIn() {
		if (!in.is_open()) {
			in.open(file);
		}
	}
	virtual void closeIn() {
		if (in.is_open()) {
			in.close();
		}
	}

	virtual ~Simulation() {
		closeIn();
	}

protected:
	System* system;
	std::string file;

	std::ifstream in;
	unsigned iteration = 0;

	// simulation_time * timeScale = output_time
	// simulation_coordinate(x,y,z) * spaceScale = output_coordinate
	long double timeScale = 1000;
	long double spaceScale = 1e9;

	long double getScaledTime() {
		return system->getTime() * timeScale;
	}
	vec3 getScaledPosition(const vec3& position) {
		return vec3(position.x * spaceScale, position.y * spaceScale, position.z * spaceScale);
	}
	std::string trim(std::string s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char c) { return !std::isspace(c); }));
		s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char c) { return !std::isspace(c); }).base(), s.end());
		return s;
	};
	std::string extract_between(const std::string& s, const std::string& open, const std::string& close) {
		size_t a = s.find(open);
		if (a == std::string::npos) return "";
		a += open.size();
		size_t b = s.find(close, a);
		if (b == std::string::npos) return "";
		return s.substr(a, b - a);
	};
};

class PressureSimulation : public Simulation {
public:
	PressureSimulation(PressureSystem* s, std::string posFile, std::string impFile, long double ts, long double ss) : Simulation(s, posFile, ts, ss) {
		impulseFile = impFile;
		pSystemPtr = s;
	};
	/*PressureSimulation(PressureSystem* s, std::string posFile, std::string impFile, long double ts, long double ss) : Simulation(s, posFile, ts, ss) {

	}*/

	void run(long double frameTime, long double endTime, std::ostream& debug) override {
		std::ofstream o;
		std::ofstream imp;
		o.open(file);
		imp.open(impulseFile);

		long double targetTime = getScaledTime()/* + frameTime*/;
		unsigned frameCount = 0;

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

				imp << "i" << frameCount << ";t" << system->getTime() << ";J," << impulse.x << "," << impulse.y << "," << impulse.z << ";\n";
			}
			else {
				targetTime = getScaledTime() + frameTime;
				o << "i" << frameCount++ << ";t" << getScaledTime();
				for (int i = 0; i < system->numberOfParticles(); i++) {
					Particle& p = system->getParticle(i);
					vec3 position = getScaledPosition(p.getPosition());
					o << ";" << "p" << i << "," << position.x << "," << position.y << "," << position.z;
				}
				o << ";\n";

				if (std::fmod(100.0l - (endTime - getScaledTime()) * 100.0l / endTime, 1.0l) <= PRINT_EPSILON) {
					std::ios oldState(nullptr);
					oldState.copyfmt(debug);

					debug << std::setprecision(0) << std::fixed << "Simulation " << 100.0l - (endTime - getScaledTime()) * 100.0l / endTime << " % complete." << std::endl;

					debug.copyfmt(oldState);
				}
			}
		}

		o << "i" << frameCount++ << ";t" << getScaledTime();
		for (int i = 0; i < system->numberOfParticles(); i++) {
			Particle& p = system->getParticle(i);
			vec3 position = getScaledPosition(p.getPosition());
			o << ";" << "p" << i << "," << position.x << "," << position.y << "," << position.z;
		}
		o << ";\n";

		o.close();
		imp.close();
	}

	vec3 readNextImpulse() {
		openImpIn();

		std::string line;
		line = trim(line);

		vec3 impulse = vec3(0);
		
		if (std::getline(impIn, line)) {
			/*impulse.x = std::stold(extract_between(line, "J", ","));
			impulse.y = std::stold(extract_between(line, ",", ","));
			impulse.z = std::stold(extract_between(line, ",", ";"));*/

			std::string part = extract_between(line, "J,", ";");
			std::stringstream ss(part);

			char discard;

			ss >> impulse.x >> discard >> impulse.y >> discard >> impulse.z;
		}
		else {
			impulse = vec3(std::numeric_limits<long double>::min());
		}

		return impulse;
	}

	void openImpIn() {
		if (!impIn.is_open()) {
			impIn.open(impulseFile);
		}
	}
	void closeImpIn() {
		if (impIn.is_open()) {
			impIn.close();
		}
	}

	~PressureSimulation () override {
		closeImpIn();
		closeIn();
	}
protected:
	std::string impulseFile;

	std::ifstream impIn;

	PressureSystem* pSystemPtr;
};