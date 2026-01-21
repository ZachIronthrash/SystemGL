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

using namespace std;

string trim(string s) {
	s.erase(s.begin(), find_if(s.begin(), s.end(), [](unsigned char c) { return !isspace(c); }));
	s.erase(find_if(s.rbegin(), s.rend(), [](unsigned char c) { return !isspace(c); }).base(), s.end());
	return s;
}
string extract_between(const string& s, const string& open, const string& close) {
	size_t a = s.find(open);
	if (a == string::npos) return "";
	a += open.size();
	size_t b = s.find(close, a);
	if (b == string::npos) return "";
	return s.substr(a, b - a);
}

Simulation::Simulation(System* s, string f, long double ts, long double ss) : systemPtr(s), file(f), timeScale(ts), spaceScale(ss) {}

System* Simulation::getSystemPtr() {
	return systemPtr;
}

void Simulation::run(long double frameTime, long double endTime, ostream& debug) {
	ofstream o;
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

	string line;
	getline(in, line);
	line = trim(line);

	if (!line.empty()) {
		iteration = stoi(extract_between(line, "i:", ";"));

		systemPtr->setTime(stold(extract_between(line, "t:", ";")));

		for (int i = 0; i < systemPtr->numberOfParticles(); i++) {
			Particle& particle = systemPtr->getParticle(i);

			string part = extract_between(line, "p" + to_string(i) + ":", ";");
			stringstream ss(part);

			char discard;

			vec3 pos = vec3(0);

			ss >> pos.x >> discard >> pos.y >> discard >> pos.z;

			particle.setPosition(pos);
		}
	}
	else {
		iteration = numeric_limits<unsigned int>::max();
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
void Simulation::printIntToFile(string prefix, unsigned i, string suffix, ostream& o) {
	o << prefix << i << suffix;
}
void Simulation::printLongDoubleToFile(string prefix, long double ld, string suffix, ostream& o) {
	o << prefix << ld << suffix;
}
void Simulation::printVec3ToFile(string prefix, vec3 v, string suffix, ostream& o) {
	o << prefix << v.x << "," << v.y << "," << v.z << suffix;
}

void Simulation::printParticlePositions(ostream& o) {
	for (int i = 0; i < systemPtr->numberOfParticles(); i++) {
		Particle& p = systemPtr->getParticle(i);
		vec3 position = getScaledPosition(p.getPosition());
		printVec3ToFile("p" + to_string(i) + ":", position, ";", o);
	}
}

void Simulation::printLine2File(unsigned& i, ostream& o) {
	/*printFrameCount(i, o);
	printFrameTime(o);
	printParticlePositions(o);*/

	printIntToFile("i:", i, ";", o);
	i++;
	printLongDoubleToFile("t:", getScaledTime(), ";", o);
	printParticlePositions(o);

	o << endl;
}


PressureSimulation::PressureSimulation(PressureSystem* s, string posFile, string impFile, long double ts, long double ss) : Simulation(s, posFile, ts, ss) {
	impulseFile = impFile;
	pSystemPtr = s;
}

void PressureSimulation::run(long double frameTime, long double endTime, ostream& debug) {
	ofstream o;
	ofstream imp;
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
		vector<vec3> initVelocities;
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

			/*if (fmod(100.0l - (endTime - getScaledTime()) * 100.0l / endTime, 1.0l) < frameTime) {
				ios oldState(nullptr);
				oldState.copyfmt(debug);

				debug << setprecision(0) << fixed << "Simulation " << 100.0l - (endTime - getScaledTime()) * 100.0l / endTime << " % complete." << endl;

				debug.copyfmt(oldState);
			}*/
		}
	}

	debug << "\r|";
	for (int i = 0; i < 100; i++) {
		debug << "*";
	}
	debug << "| " << 100 << " %";

	debug << endl;

	printLine2File(frameCount, o);

	o.close();
	imp.close();
}

vec3 PressureSimulation::readNextImpulse(long double& time) {
	openImpIn();

	string line;
	line = trim(line);

	vec3 impulse = vec3(0);

	if (getline(impIn, line)) {
		/*impulse.x = stold(extract_between(line, "J", ","));
		impulse.y = stold(extract_between(line, ",", ","));
		impulse.z = stold(extract_between(line, ",", ";"));*/

		string part = extract_between(line, "J:", ";");
		stringstream ss0(part);

		char discard;

		ss0 >> impulse.x >> discard >> impulse.y >> discard >> impulse.z;

		part = extract_between(line, "t:", ";");

		stringstream ss1(part);

		ss1 >> time;
	}
	else {
		impulse = vec3(numeric_limits<long double>::min());
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