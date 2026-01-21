#pragma once
#include "System.h"
#include <fstream>
#include <ostream>
#include <string>

class Simulation {
public:
	Simulation(System* s, std::string f, long double ts, long double ss);

	System* getSystemPtr();

	virtual void run(long double frameTime, long double endTime, std::ostream& debug);

	virtual unsigned readNext();

	virtual void resetIn();

	virtual void openIn();
	virtual void closeIn();

	virtual ~Simulation();

protected:
	System* systemPtr;

	std::string file;

	std::ifstream in;
	unsigned iteration = 0;

	// simulation_time * timeScale = output_time
	// simulation_coordinate(x,y,z) * spaceScale = output_coordinate
	long double timeScale = 1000;
	long double spaceScale = 1e9;

	long double getScaledTime();
	vec3 getScaledPosition(const vec3& position);
	std::string trim(std::string s);
	std::string extract_between(const std::string& s, const std::string& open, const std::string& close);
	void printFrameCount(unsigned& i, std::ostream& o);
	void printFrameTime(std::ostream& o);
	void printParticlePositions(std::ostream& o);
	void printLine2File(unsigned& i, std::ostream& o);
};

class PressureSimulation : public Simulation {
public:
	PressureSimulation(PressureSystem* s, std::string posFile, std::string impFile, long double ts, long double ss);
	/*PressureSimulation(PressureSystem* s, std::string posFile, std::string impFile, long double ts, long double ss) : Simulation(s, posFile, ts, ss) {

	}*/

	void run(long double frameTime, long double endTime, std::ostream& debug) override;

	vec3 readNextImpulse(long double& time);

	void openImpIn();

	void closeImpIn();

	~PressureSimulation() override;
protected:
	std::string impulseFile;

	std::ifstream impIn;

	PressureSystem* pSystemPtr;
};
