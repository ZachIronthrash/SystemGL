#pragma once

#include "SystemGLMath.h"
#include <fstream>
#include <map>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

using namespace std;

string trim(string s);
string extract_between(const string& s, const string& open, const string& close);

struct stateData {
private:
	/* internal representation as a map
	* string name -> value
	* abstract representation as a set of states
	* as such, the names must be unique across all types
	* furthermore, the user should not rely on the order of states being preserved
	*/
	map<string, unsigned> uState;
	map<string, long double> ldState;
	map<string, vec3> vState;
public:
	stateData() {};
	stateData(map<string, unsigned> uState,
		map<string, long double> ldState,
		map<string, vec3> vState) : uState(uState), ldState(ldState), vState(vState) {
	}

	void add(const string& name, auto value) {
		/*if (uState.find(name) != uState.end() ||
			ldState.find(name) != ldState.end() ||
			vState.find(name) != vState.end()) {
			throw runtime_error("State with name '" + name + "' already exists.");
		}*/

		if constexpr (is_same_v<decltype(value), unsigned>) {
			uState[name] = value;
		}
		else if constexpr (is_same_v<decltype(value), long double>) {
			ldState[name] = value;
		}
		else if constexpr (is_same_v<decltype(value), vec3>) {
			vState[name] = value;
		}
	}

	template<typename T>
	pair<string, T> removeAny() {
		pair<string, T> pair = make_pair("", T());

		if constexpr (is_same_v<T, unsigned>) {
			if (uState.size()) {
				auto it = uState.begin();
				if (it != uState.end()) {
					pair.first = it->first;
					pair.second = it->second;
					uState.erase(it);
				}
			}
		}
		else if constexpr (is_same_v<T, long double>) {
			if (ldState.size()) {
				auto it = ldState.begin();
				if (it != ldState.end()) {
					pair.first = it->first;
					pair.second = it->second;
					ldState.erase(it);
				}
			}
		}
		else if constexpr (is_same_v<T, vec3>) {
			if (vState.size()) {
				auto it = vState.begin();
				if (it != vState.end()) {
					pair.first = it->first;
					pair.second = it->second;
					vState.erase(it);
				}
			}
		}

		return pair;
	}

	template<typename T>
	size_t size() {
		if constexpr (is_same_v<T, unsigned>) {
			return uState.size();
		}
		else if constexpr (is_same_v<T, long double>) {
			return ldState.size();
		}
		else if constexpr (is_same_v<T, vec3>) {
			return vState.size();
		}
	}

	template<typename T>
	void remove(const string& name, T& out) {
		if constexpr (is_same_v<T, unsigned>) {
			if (uState.find(name) != uState.end()) {
				unsigned state = uState[name];
				uState.erase(name);
				out = state;
			}
		}
		else if constexpr (is_same_v<T, long double>) {
			if (ldState.find(name) != ldState.end()) {
				long double state = ldState[name];
				ldState.erase(name);
				out = state;
			}
		}
		else if constexpr (is_same_v<T, vec3>) {
			if (vState.find(name) != vState.end()) {
				vec3 state = vState[name];
				vState.erase(name);
				out = state;
			}
		}
	}

	bool contains(const string& name) {
		for (pair<string, unsigned> state : uState) {
			if (state.first == name) return true;
		}
		for (pair<string, long double> state : ldState) {
			if (state.first == name) return true;
		}
		for (pair<string, vec3> state : vState) {
			if (state.first == name) return true;
		}
		return false;
	}

	template<typename T>
	void clear() {
		if constexpr (is_same_v<T, unsigned>) {
			uState.clear();
		}
		else if constexpr (is_same_v<T, long double>) {
			ldState.clear();
		}
		else if constexpr (is_same_v<T, vec3>) {
			vState.clear();
		}
	}

	/*template<typename T>
	void fetch(const string& name, T& out) {
		try {
			if constexpr (is_same_v<T, unsigned>) {
				if (uState.find(name) != uState.end()) {
					unsigned u = uState[name];
					out = u;
				}
			}
			else if constexpr (is_same_v<T, long double>) {
				if (ldState.find(name) != ldState.end()) {
					long double ld = ldState[name];
					out = ld;
				}
			}
			else if constexpr (is_same_v<T, vec3>) {
				if (vState.find(name) != vState.end()) {
					vec3 v = vState[name];
					out = v;
				}
			}
		}
		catch (exception& e) {
			std::cerr << e.what();
		}

		throw runtime_error("State with name '" + name + "' does not exist.");
	}*/
	unsigned fetchU(const string& name) {
		if (uState.find(name) != uState.end()) {
			return uState[name];
		}
		throw runtime_error("Unsigned state with name '" + name + "' does not exist.");
	}
	long double fetchLD(const string& name) {
		if (ldState.find(name) != ldState.end()) {
			return ldState[name];
		}
		throw runtime_error("Long double state with name '" + name + "' does not exist.");
	}
	vec3 fetchV(const string& name) {
		if (vState.find(name) != vState.end()) {
			return vState[name];
		}
		throw runtime_error("Vec3 state with name '" + name + "' does not exist.");
	}

	// friend class SimState;
};

class StateIO {
public:

	stateData states;

	StateIO(string f, map<string, unsigned> uStates = map<string, unsigned>(),
		map<string, long double> ldStates = map<string, long double>(),
		map<string, vec3> vStates = map<string, vec3>()) : file(f), states(stateData(uStates, ldStates, vStates)) {
	}

	void outputStatesToFile() {
		ofstream o(file);

		if (!o.is_open()) {
			throw runtime_error("Could not open state file '" + file + "' for writing.");
		}

		stateData states = this->states;

		while (states.size<unsigned>() > 0) {
			pair<string, unsigned> state = states.removeAny<unsigned>();
			o << "Unsigned - " << state.first << ": " << state.second << ";" << endl;
		}
		while (states.size<long double>() > 0) {
			pair<string, long double> state = states.removeAny<long double>();
			o << "Long Double - " << state.first << ": " << state.second << ";" << endl;
		}
		while (states.size<vec3>() > 0) {
			pair<string, vec3> state = states.removeAny<vec3>();
			o << "Vec3 - " << state.first << ": " << state.second << ";" << endl;
		}

		/*for (const auto& [key, value] : states.uState) {
			o << "Unsigned - " << key << ": " << value << ";" << endl;
		}
		for (const auto& [key, value] : states.ldState) {
			o << "Long Double - " << key << ": " << value << ";" << endl;
		}
		for (const auto& [key, value] : states.vState) {
			o << "Vec3 - " << key << ": " << value << ";" << endl;
		}*/

		o.close();
	}
	void readStatesFromFile() {
		ifstream i(file);

		if (!i.is_open()) {
			throw runtime_error("Could not open state file '" + file + "' for reading.");
		}

		string line;
		while (getline(i, line)) {
			if (line.find("Unsigned - ") != string::npos) {
				string key = extract_between(line, "Unsigned - ", ": ");
				unsigned value = stoi(extract_between(line, ": ", ";"));
				states.add(key, value);
			}
			else if (line.find("Long Double - ") != string::npos) {
				string key = extract_between(line, "Long Double - ", ": ");
				long double value = stold(extract_between(line, ": ", ";"));
				states.add(key, value);
			}
			else if (line.find("Vec3 - ") != string::npos) {
				string key = extract_between(line, "Vec3 - ", ": ");
				string vecStr = extract_between(line, ": ", ";");
				stringstream ss(vecStr);
				char discard;
				long double x, y, z;
				ss >> x >> discard >> y >> discard >> z;

				states.add(key, vec3(x, y, z));
			}
		}
		i.close();
	}

	void clearStates() {
		states.clear<unsigned>();
		states.clear<long double>();
		states.clear<vec3>();
	}

	~StateIO() {}

private:
	string file;
};