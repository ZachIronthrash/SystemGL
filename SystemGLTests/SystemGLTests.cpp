#include "pch.h"
#include "../SystemGL/include/Simulation.h"
#include "../SystemGL/include/System.h"
#include "CppUnitTest.h"
#include <algorithm>
#include <string>
#include <vector>
#include <map>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace StateTest {
	TEST_CLASS(StateTest) {
private:
	void addUnsignedArgsToIO(StateIO& io, std::vector<unsigned> u) {
		for (size_t i = 0; i < u.size(); i++) {
			Logger::WriteMessage(("Adding u" + to_string(i) + " with value " + to_string(u.at(i)) + "\n").c_str());
			io.states.add("u" + to_string(i), u.at(i));
			//state.addUnsignedState("u" + to_string(i), u.at(i));
		}
	}
	void addLongDoubleArgsToIO(StateIO& io, std::vector<long double> ld) {
		for (size_t i = 0; i < ld.size(); i++) {
			Logger::WriteMessage(("Adding ld" + to_string(i) + " with value " + to_string(ld.at(i)) + "\n").c_str());
			io.states.add("ld" + to_string(i), ld.at(i));
		}
	}
	void addVec3ArgsToIO(StateIO& io, std::vector<vec3> v) {
		for (size_t i = 0; i < v.size(); i++) {
			Logger::WriteMessage(("Adding v" + to_string(i) + " with value " + to_string(v.at(i).x) + ", " + to_string(v.at(i).y) + ", " + to_string(v.at(i).z) + "\n").c_str());
			io.states.add("v" + to_string(i), v.at(i));
		}
	}

	template<typename T>
	void copyMapFromData(stateData data, std::map<string, T>& m) {
		Logger::WriteMessage("Copying map from stateData\n");
		if constexpr (is_same_v<T, unsigned>) {
			Logger::WriteMessage("Copying unsigned map\n");
			while (data.size<unsigned>() > 0) {
				auto pair = data.removeAny<unsigned>();
				m[pair.first] = pair.second;
			}
		}
		else if constexpr (is_same_v<T, long double>) {
			while (data.size<long double>() > 0) {
				auto pair = data.removeAny<long double>();
				m[pair.first] = pair.second;
			}
		}
		else if constexpr (is_same_v<T, vec3>) {
			while (data.size<vec3>() > 0) {
				auto pair = data.removeAny<vec3>();
				m[pair.first] = pair.second;
			}
		}
	}

	template<typename T>
	std::vector<T> extractVectorFromMap(std::map<string, T>& m) {
		Logger::WriteMessage("Extracting vector from map\n");
		std::vector<T> v;

		v.reserve(m.size());

		for (pair<string, T> p : m) {
			v.push_back(p.second);
		}

		return v;
	}

	/*template<typename T>
	std::vector<T> extractVectorFromStateIO(StateIO& io) {
		if constexpr (is_same_v<T, unsigned>) {
			return extractVectorFromMap<unsigned>(io.states.copyMap<unsigned>());
		}
		else if constexpr (is_same_v<T, long double>) {
			return extractVectorFromMap<long double>(io.states.copyMap<long double>());
		}
		else if constexpr (is_same_v<T, vec3>) {
			return extractVectorFromMap<vec3>(io.states.copyMap<vec3>());
		}
	}*/

	template<typename T>
	std::vector<T> extractVectorFromData(stateData data) {
		Logger::WriteMessage("Extracting vector from stateData\n");
		map<string, T> map;
		copyMapFromData<T>(data, map);
		Logger::WriteMessage(("MAP SIZE: " + to_string(map.size()) + "\n").c_str());
		return extractVectorFromMap<T>(map);
	}
public:
	TEST_METHOD(BaselineStateDataCheck)
	{
		stateData data;
		std::vector<unsigned> uStateExpected = { 5, 10, 15 };
		std::vector<long double> ldStateExpected = { 1.5l, 2.5l, 3.5l };
		std::vector<vec3> vStateExpected = { vec3(1.0l, 2.0l, 3.0l), vec3(4.0l, 5.0l, 6.0l), vec3(7.0l, 8.0l, 9.0l) };

		data.add("u0", uStateExpected[0]);
		data.add("u1", uStateExpected[1]);
		data.add("u2", uStateExpected[2]);
		data.add("ld0", ldStateExpected[0]);
		data.add("ld1", ldStateExpected[1]);
		data.add("ld2", ldStateExpected[2]);
		data.add("v0", vStateExpected[0]);
		data.add("v1", vStateExpected[1]);
		data.add("v2", vStateExpected[2]);

		/*data.uState["u0"] = uStateExpected[0];
		data.uState["u1"] = uStateExpected[1];
		data.uState["u2"] = uStateExpected[2];

		data.ldState["ld0"] = ldStateExpected[0];
		data.ldState["ld1"] = ldStateExpected[1];
		data.ldState["ld2"] = ldStateExpected[2];

		data.vState["v0"] = vStateExpected[0];
		data.vState["v1"] = vStateExpected[1];
		data.vState["v2"] = vStateExpected[2];*/

		/*std::map<string, unsigned> uMap;
		std::map<string, long double> ldMap;
		std::map<string, vec3> vMap;*/

		/*data.copyMap<unsigned>(uMap);
		data.copyMap<long double>(ldMap);
		data.copyMap<vec3>(vMap);*/

		/*map<string, unsigned> uMap = copyMapFromData<unsigned>(data);
		map<string, long double> ldMap = copyMapFromData<long double>(data);
		map<string, vec3> vMap = copyMapFromData<vec3>(data);*/

		std::vector<unsigned> uState = extractVectorFromData<unsigned>(data);
		std::vector<long double> ldState = extractVectorFromData<long double>(data);
		std::vector<vec3> vState = extractVectorFromData<vec3>(data);

		/*while (uMap.size() > 0) {
			auto it = uMap.begin();
			uState.push_back(it->second);
			uMap.erase(it);
		}
		while (ldMap.size() > 0) {
			auto it = ldMap.begin();
			ldState.push_back(it->second);
			ldMap.erase(it);
		}
		while (vMap.size() > 0) {
			auto it = vMap.begin();
			vState.push_back(it->second);
			vMap.erase(it);
		}*/
		std::sort(uState.begin(), uState.end());
		std::sort(ldState.begin(), ldState.end());
		std::sort(vState.begin(), vState.end(), [](const vec3& a, const vec3& b) {
			if (a.x != b.x) return a.x < b.x;
			if (a.y != b.y) return a.y < b.y;
			return a.z < b.z;
			});
		std::sort(uStateExpected.begin(), uStateExpected.end());
		std::sort(ldStateExpected.begin(), ldStateExpected.end());
		std::sort(vStateExpected.begin(), vStateExpected.end(), [](const vec3& a, const vec3& b) {
			if (a.x != b.x) return a.x < b.x;
			if (a.y != b.y) return a.y < b.y;
			return a.z < b.z;
			});
		Assert::IsTrue(std::equal(uStateExpected.begin(), uStateExpected.end(), uState.begin(), uState.end()));
		Assert::IsTrue(std::equal(ldStateExpected.begin(), ldStateExpected.end(), ldState.begin(), ldState.end()));
		Assert::IsTrue(std::equal(vStateExpected.begin(), vStateExpected.end(), vState.begin(), vState.end()));
	}
	TEST_METHOD(SimStateAddAndRetrieveStates)
	{
		StateIO io("SimStateAddAndRetrieveStates.txt");
		std::vector<unsigned> uStateExpected = { 5, 10, 15 };
		std::vector<long double> ldStateExpected = { 1.5l, 2.5l, 3.5l };
		std::vector<vec3> vStateExpected = { vec3(1.0l, 2.0l, 3.0l), vec3(4.0l, 5.0l, 6.0l), vec3(7.0l, 8.0l, 9.0l) };
		addUnsignedArgsToIO(io, uStateExpected);
		addLongDoubleArgsToIO(io, ldStateExpected);
		addVec3ArgsToIO(io, vStateExpected);
		std::vector<unsigned> uState = extractVectorFromData<unsigned>(io.states);
		std::vector<long double> ldState = extractVectorFromData<long double>(io.states);
		std::vector<vec3> vState = extractVectorFromData<vec3>(io.states);
		/*for (size_t i = 0; i < uStateExpected.size(); i++) {
			uState.push_back(state.states.fetch<unsigned>("u" + to_string(i)));
		}
		for (size_t i = 0; i < ldStateExpected.size(); i++) {
			ldState.push_back(state.states.fetch<long double>("ld" + to_string(i)));
		}
		for (size_t i = 0; i < vStateExpected.size(); i++) {
			vState.push_back(state.states.fetch<vec3>("v" + to_string(i)));
		}*/
		std::sort(uState.begin(), uState.end());
		std::sort(ldState.begin(), ldState.end());
		std::sort(vState.begin(), vState.end(), [](const vec3& a, const vec3& b) {
			if (a.x != b.x) return a.x < b.x;
			if (a.y != b.y) return a.y < b.y;
			return a.z < b.z;
			});
		std::sort(uStateExpected.begin(), uStateExpected.end());
		std::sort(ldStateExpected.begin(), ldStateExpected.end());
		std::sort(vStateExpected.begin(), vStateExpected.end(), [](const vec3& a, const vec3& b) {
			if (a.x != b.x) return a.x < b.x;
			if (a.y != b.y) return a.y < b.y;
			return a.z < b.z;
			});
		Assert::IsTrue(std::equal(uStateExpected.begin(), uStateExpected.end(), uState.begin(), uState.end()));
		Assert::IsTrue(std::equal(ldStateExpected.begin(), ldStateExpected.end(), ldState.begin(), ldState.end()));
		Assert::IsTrue(std::equal(vStateExpected.begin(), vStateExpected.end(), vState.begin(), vState.end()));
	}

	/*
	* Tests outputting SimState to file and reading it back in. 
	* Test method SimStateAddAndRetrieveStates must pass to ensure correctness of add and pull functions.
	*/
	TEST_METHOD(SimStateOutputAndReadIn) 
	{
		StateIO stateExpected("SimStateOutputAndReadIn.txt");
		std::vector<unsigned> uStateExpected = { 5, 10, 15 };
		std::vector<long double> ldStateExpected = { 1.5l, 2.5l, 3.5l };
		std::vector<vec3> vStateExpected = { vec3(1.0l, 2.0l, 3.0l), vec3(4.0l, 5.0l, 6.0l), vec3(7.0l, 8.0l, 9.0l) };

		addUnsignedArgsToIO(stateExpected, uStateExpected);
		addLongDoubleArgsToIO(stateExpected, ldStateExpected);
		addVec3ArgsToIO(stateExpected, vStateExpected);

		stateExpected.outputStatesToFile();

		Logger::WriteMessage(("UI SIZE: " + to_string(stateExpected.states.size<unsigned int>()) + "\n").c_str());

		StateIO io("SimStateOutputAndReadIn.txt");

		io.readStatesFromFile();

		std::vector<unsigned> uState = extractVectorFromData<unsigned>(io.states);
		std::vector<long double> ldState = extractVectorFromData<long double>(io.states);
		std::vector<vec3> vState = extractVectorFromData<vec3>(io.states);

		Logger::WriteMessage(("UI SIZE: " + to_string(uState.size()) + "\n").c_str());

		for (unsigned i : uState) {
			Logger::WriteMessage(("uState value: " + to_string(i) + "\n").c_str());
		}

		std::sort(uState.begin(), uState.end());
		std::sort(ldState.begin(), ldState.end());
		std::sort(vState.begin(), vState.end(), [](const vec3& a, const vec3& b) {
			if (a.x != b.x) return a.x < b.x;
			if (a.y != b.y) return a.y < b.y;
			return a.z < b.z;
			});
		std::sort(uStateExpected.begin(), uStateExpected.end());
		std::sort(ldStateExpected.begin(), ldStateExpected.end());
		std::sort(vStateExpected.begin(), vStateExpected.end(), [](const vec3& a, const vec3& b) {
			if (a.x != b.x) return a.x < b.x;
			if (a.y != b.y) return a.y < b.y;
			return a.z < b.z;
			});
		Assert::IsTrue(std::equal(uStateExpected.begin(), uStateExpected.end(), uState.begin(), uState.end()));
		Assert::IsTrue(std::equal(ldStateExpected.begin(), ldStateExpected.end(), ldState.begin(), ldState.end()));
		Assert::IsTrue(std::equal(vStateExpected.begin(), vStateExpected.end(), vState.begin(), vState.end()));
	}
		
	};
}