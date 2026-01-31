#pragma once
#include "System.h"
#include <fstream>
#include <ostream>
#include <string>

#include <map>
#include <sstream>
#include <utility>

using namespace std;

string trim(string s);
string extract_between(const string& s, const string& open, const string& close);

struct stateData {
	map<string, unsigned> uState;
	map<string, long double> ldState;
	map<string, vec3> vState;

	stateData() {};
	stateData(map<string, unsigned> uState,
		map<string, long double> ldState,
		map<string, vec3> vState) : uState(uState), ldState(ldState), vState(vState) {
	}

	stateData operator+(const pair<string, unsigned>& uPair) const {
		stateData result = *this;
		result.uState[uPair.first] += uPair.second;
		return result;
	}
	stateData operator+(const pair<string, long double>& ldPair) const {
		stateData result = *this;
		result.ldState[ldPair.first] += ldPair.second;
		return result;
	}
	stateData operator+(const pair<string, vec3>& vPair) const {
		stateData result = *this;
		result.vState[vPair.first] += vPair.second;
		return result;
	}

	stateData& operator+=(const pair<string, unsigned>& uPair) {
		uState[uPair.first] += uPair.second;
		return *this;
	}
	stateData& operator+=(const pair<string, long double>& ldPair) {
		ldState[ldPair.first] += ldPair.second;
		return *this;
	}
	stateData& operator+=(const pair<string, vec3>& vPair) {
		vState[vPair.first] += vPair.second;
		return *this;
	}
};

class SimState {
public:
	SimState(string f, map<string, unsigned> uStates = map<string, unsigned>(),
		map<string, long double> ldStates = map<string, long double>(),
		map<string, vec3> vStates = map<string, vec3>()) : states(stateData(uStates, ldStates, vStates)) {
	}

	stateData getStates() {
		return states;
	}

	void addUnsignedState(string name, unsigned value) {
		states.uState[name] = value;
	}
	void addLongDoubleState(string name, long double value) {
		states.ldState[name] = value;
	}
	void addVec3State(string name, vec3 value) {
		states.vState[name] = value;
	}

	unsigned pullUnsignedState(string name) {
		return states.uState[name];
	}
	long double pullLongDoubleState(string name) {
		return states.ldState[name];
	}
	vec3 pullVec3State(string name) {
		return states.vState[name];
	}

	unsigned removeUnsignedState(string name) {
		unsigned state = states.uState[name];
		states.uState.erase(name);
		return state;
	}
	long double removeLongDoubleState(string name) {
		long double state = states.ldState[name];
		states.ldState.erase(name);
		return state;
	}
	vec3 removeVec3State(string name) {
		vec3 state = states.vState[name];
		states.vState.erase(name);
		return state;
	}

	void outputStatesToFile() {
		ofstream o(file);

		for (const auto& [key, value] : states.uState) {
			o << "Unsigned - " << key << ": " << value << ";" << endl;
		}
		for (const auto& [key, value] : states.ldState) {
			o << "Long Double - " << key << ": " << value << ";" << endl;
		}
		for (const auto& [key, value] : states.vState) {
			o << "Vec3 - " << key << ": " << value << ";" << endl;
		}

		o.close();
	}
	void pullStatesFromFile() {
		ifstream i(file);
		string line;
		while (getline(i, line)) {
			if (line.find("Unsigned - ") != string::npos) {
				string key = extract_between(line, "Unsigned - ", ": ");
				unsigned value = stoi(extract_between(line, ": ", ";"));
				states.uState[key] = value;
			}
			else if (line.find("Long Double - ") != string::npos) {
				string key = extract_between(line, "Long Double - ", ": ");
				long double value = stold(extract_between(line, ": ", ";"));
				states.ldState[key] = value;
			}
			else if (line.find("Vec3 - ") != string::npos) {
				string key = extract_between(line, "Vec3 - ", ": ");
				string vecStr = extract_between(line, ": ", ";");
				stringstream ss(vecStr);
				char discard;
				long double x, y, z;
				ss >> x >> discard >> y >> discard >> z;

				states.vState[key] = vec3(x, y, z);
			}
		}
		i.close();
	}

	void clearStates() {
		states.uState.clear();
		states.ldState.clear();
		states.vState.clear();
	}

	~SimState() {}
private:
	string file;

	stateData states;
};

class Simulation {
public:
	Simulation(System* s, string f, long double ts, long double ss);

	System* getSystemPtr();

	virtual void run(long double frameTime, long double endTime, ostream& debug);

	virtual unsigned readNext();

	virtual void resetIn();

	virtual void openIn();
	virtual void closeIn();

	virtual ~Simulation();

protected:
	System* systemPtr;

	string file;

	ifstream in;
	unsigned iteration = 0;

	// simulation_time * timeScale = output_time
	// simulation_coordinate(x,y,z) * spaceScale = output_coordinate
	long double timeScale = 1000;
	long double spaceScale = 1e9;

	long double getScaledTime();
	vec3 getScaledPosition(const vec3& position);
	void printIntToFile(string prefix, unsigned i, string suffix, ostream& o);
	void printLongDoubleToFile(string prefix, long double ld, string suffix, ostream& o);
	void printVec3ToFile(string prefix, vec3 v, string suffix, ostream& o);

	void printParticlePositions(ostream& o);

	/*void printFrameCount(unsigned& i, ostream& o);
	void printFrameTime(ostream& o);
	void printParticlePositions(ostream& o);*/
	void printLine2File(unsigned& i, ostream& o);
};

class PressureSimulation : public Simulation {
public:
	PressureSimulation(PressureSystem* s, string posFile, string impFile, long double ts, long double ss);
	/*PressureSimulation(PressureSystem* s, string posFile, string impFile, long double ts, long double ss) : Simulation(s, posFile, ts, ss) {

	}*/

	void run(long double frameTime, long double endTime, ostream& debug) override;

	vec3 readNextImpulse(long double& time);

	void openImpIn();

	void closeImpIn();

	~PressureSimulation() override;
protected:
	string impulseFile;

	ifstream impIn;

	PressureSystem* pSystemPtr;
};