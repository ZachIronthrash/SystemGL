#pragma once
#include <fstream>

#include <string>

#include <sstream>

#include "System.h"

class Simulation {
public:
	Simulation(System* s, std::string f) : system(s), file(f) {}

	void run(long double frameTime, long double endTime) {
		std::ofstream o;
		o.open(file);

		long double targetTime = system->getTime()/* + frameTime*/;
		unsigned frameCount = 0;

		while (system->getTime() < endTime) {

			if (system->getTime() < targetTime) {
				system->evolve();
			}
			else {
				targetTime = system->getTime() + frameTime;
				o << "i" << frameCount++ << ";t" << system->getTime();
				for (int i = 0; i < system->numberOfParticles(); i++) {
					Particle& p = system->getParticle(i);
					vec3 position = p.getPosition();
					o << ";" << "p" << i << "," << position.x << "," << position.y << "," << position.z;
				}
				o << ";\n";
			}
		}

		o << "i" << frameCount++ << ";t" << system->getTime();
		for (int i = 0; i < system->numberOfParticles(); i++) {
			Particle& p = system->getParticle(i);
			vec3 position = p.getPosition();
			o << ";" << "p" << i << "," << position.x << "," << position.y << "," << position.z;
		}
		o << ";\n";

		o.close();
	}

	unsigned readNext() {
		auto trim = [](std::string s) {
			s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char c) { return !std::isspace(c); }));
			s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char c) { return !std::isspace(c); }).base(), s.end());
			return s;
			};
		auto extract_between = [](const std::string& s, const std::string& open, const std::string& close) -> std::string {
			size_t a = s.find(open);
			if (a == std::string::npos) return "";
			a += open.size();
			size_t b = s.find(close, a);
			if (b == std::string::npos) return "";
			return s.substr(a, b - a);
			};

		openInput();

		std::string line;
		std::getline(in, line);
		line = trim(line);
		
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

		return iteration;
	}

	void openInput() {
		if (!in.is_open()) {
			in.open(file);
		}
	}
	void closeInput() {
		if (in.is_open()) {
			in.close();
		}
	}
private:
	System* system;
	std::string file;

	std::ifstream in;
	unsigned iteration = 0;
};