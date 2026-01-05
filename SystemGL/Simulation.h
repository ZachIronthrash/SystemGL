#pragma once
#include <istream>

#include <fstream>

#include <string>

#include "System.h"

class Simulation {
public:
	Simulation(System* s, std::string f) : system(s), file(f) {}

	void run(long double frameTime, long double endTime) {
		std::ofstream o;

		long double targetTime = system->getTime() + frameTime;

		while (system->getTime() < endTime) {

			if (system->getTime() < targetTime) {
				system->evolve();
			}
			else {
				targetTime = system->getTime() + frameTime;
			}
		}
		}
private:
	System* system;
	std::string file;
};