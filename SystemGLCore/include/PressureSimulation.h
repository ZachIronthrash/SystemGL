#pragma once

#include "PressureSystem.h"
#include "Simulation.h"
#include "SystemGLMath.h"
#include <fstream>
#include <ostream>
#include <string>

class PressureSimulation : public Simulation {
public:
	PressureSimulation(PressureSystem& s, std::string posFile, std::string impFile, long double ts, long double ss);
	/*PressureSimulation(PressureSystem* s, string posFile, string impFile, long double ts, long double ss) : Simulation(s, posFile, ts, ss) {

	}*/

	void run(long double frameTime, long double endTime, std::ofstream& action) override;

	vec3 readNextImpulse(long double& time);

	void openImpIn();

	void closeImpIn();

	~PressureSimulation() override;
protected:
	std::string impulseFile;

	std::ifstream impIn;

	PressureSystem& system;
};