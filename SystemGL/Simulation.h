#pragma once
#include <fstream>

#include <string>

#include <sstream>

#include "System.h"

class Simulation {
public:
	Simulation(System* s, std::string f, long double ts, long double ss) : system(s), file(f), timeScale(ts), spaceScale(ss) {}

	System* systemPtr() {
		return system;
	}

	virtual void run(long double frameTime, long double endTime) {
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

class PressureSimulation : private Simulation {
public:
	PressureSimulation(System* s, std::string posFile, std::string impFile, long double ts, long double ss) : Simulation(s, posFile, ts, ss) {
		
	}

	void run(long double frameTime, long double endTime) override {
		std::ofstream o;
		std::ofstream imp;
		o.open(file);
		imp.open(impulseFile);

		long double targetTime = getScaledTime()/* + frameTime*/;
		unsigned frameCount = 0;

		while (getScaledTime() < endTime) {
			std::vector<vec3> initVelocities;
			if (getScaledTime() < targetTime) {
				initVelocities.clear();
				for (int i = 0; i < system->numberOfParticles(); i++) {
					Particle& p = system->getParticle(i);
					initVelocities.push_back(p.getVelocity());
				}
				system->evolve();
				vec3 totalStepImpulse = vec3(0);
				for (int i = 0; i < system->numberOfParticles(); i++) {
					Particle& p = system->getParticle(i);
					vec3 deltaV = p.getVelocity() - initVelocities[i];
					vec3 impulse = deltaV * p.getMass();
					totalStepImpulse += impulse;
				}
				imp << "i" << frameCount << ";t" << getScaledTime() << ";J" << totalStepImpulse.x << "," << totalStepImpulse.y << "," << totalStepImpulse.z << ";\n";
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
protected:
	std::string impulseFile;

	std::ifstream impIn;
};