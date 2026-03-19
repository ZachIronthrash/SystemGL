#pragma once
//#include "System.h"
#include "Temp.h"
#include "SystemGLMath.h"
#include <glm/fwd.hpp>
#include <vector>
#include <fstream>
#include <iostream>
#include <ostream>
#include <string>

class Simulation {
public:
	std::vector<glm::vec3> positions;

	Simulation(/*System& s,*/std::string f, long double ts, long double ss);

	//System& getSystem();

	virtual void run(System& s, long double frameTime, long double endTime, std::ofstream& energyDebt);

	virtual unsigned readNext();

	virtual void resetIn();

	virtual void openIn();
	virtual void closeIn();

	virtual ~Simulation();

protected:
	//System& system;

	int particleCount = 0;

	std::string file;

	std::ifstream in;
	unsigned iteration = 0;

	// simulation_time * timeScale = output_time
	// simulation_coordinate(x,y,z) * spaceScale = output_coordinate
	long double timeScale = 1000;
	long double spaceScale = 1e9;

	long double getScaledTime(System& s);
	vec3 getScaledPosition(const vec3& position);
	void printIntToFile(std::string prefix, unsigned i, std::string suffix, std::ostream& o);
	void printLongDoubleToFile(std::string prefix, long double ld, std::string suffix, std::ostream& o);
	void printVec3ToFile(std::string prefix, vec3 v, std::string suffix, std::ostream& o);

	void printParticlePositions(System& s, std::ostream& o);

	/*void printFrameCount(unsigned& i, ostream& o);
	void printFrameTime(ostream& o);
	void printParticlePositions(ostream& o);*/
	void printLine2File(System& s, unsigned& i, std::ostream& o);
};

//auto fetch(string name) {
	//	stateData s = states;


	//	return -1; // int is a type that will never be stored (yet) so we can use it to indicate failure
	//}

	////void addUnsignedState(string name, unsigned value) {
	////	//states.uState[name] = value;
	////	states.add(name, value);
	////}
	////void addLongDoubleState(string name, long double value) {
	////	//states.ldState[name] = value;
	////	states.add(name, value);
	////}
	////void addVec3State(string name, vec3 value) {
	////	//states.vState[name] = value;
	////	states.add(name, value);
	////}
	//void addState(string name, auto value) {
	//	states.add(name, value);
	//}

	///*unsigned removeUnsignedState(string name) {
	//	unsigned state = states.uState[name];
	//	states.uState.erase(name);
	//	return state;
	//}
	//long double removeLongDoubleState(string name) {
	//	long double state = states.ldState[name];
	//	states.ldState.erase(name);
	//	return state;
	//}
	//vec3 removeVec3State(string name) {
	//	vec3 state = states.vState[name];
	//	states.vState.erase(name);
	//	return state;
	//}*/

	//void removeState(string name, auto object) {
	//	object = states.remove(name);
	//}
	//void removeAnyState(auto object) {
	//	object = states.removeAny();
	//}

	///*unsigned removeAnyUnsignedState() {
	//	auto it = states.uState.begin();
	//	unsigned state = it->second;
	//	states.uState.erase(it);
	//	return state;
	//}
	//long double removeAnyLongDoubleState() {
	//	auto it = states.ldState.begin();
	//	long double state = it->second;
	//	states.ldState.erase(it);
	//	return state;
	//}
	//vec3 removeAnyVec3State() {
	//	auto it = states.vState.begin();
	//	vec3 state = it->second;
	//	states.vState.erase(it);
	//	return state;
	//}*/#pragma once
