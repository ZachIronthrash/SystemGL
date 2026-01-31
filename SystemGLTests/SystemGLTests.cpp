#include "pch.h"
#include "../SystemGL/include/Simulation.h"
#include "../SystemGL/include/System.h"
#include "CppUnitTest.h"
#include <algorithm>
#include <string>
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace StateTest {
	TEST_CLASS(StateTest) {
private:
	void addUnsignedArgsToSimState(SimState& state, std::vector<unsigned> u) {
		for (size_t i = 0; i < u.size(); i++) {
			state.addUnsignedState("u" + to_string(i), u.at(i));
		}
	}
	void addLongDoubleArgsToSimState(SimState& state, size_t S, std::vector<long double> ld) {
		for (size_t i = 0; i < ld.size(); i++) {
			state.addLongDoubleState("ld" + to_string(i), ld.at(i));
		}
	}
	void addVec3ArgsToSimState(SimState& state, size_t S, std::vector<vec3> v) {
		for (size_t i = 0; i < v.size(); i++) {
			state.addVec3State("v" + to_string(i), v.at(i));
		}
	}
public:
	TEST_METHOD(BaselineStateDataCheck)
	{
		stateData data;
		std::vector<unsigned> uStateExpected = { 5, 10, 15 };
		std::vector<long double> ldStateExpected = { 1.5l, 2.5l, 3.5l };
		std::vector<vec3> vStateExpected = { vec3(1.0l, 2.0l, 3.0l), vec3(4.0l, 5.0l, 6.0l), vec3(7.0l, 8.0l, 9.0l) };

		data.uState["u0"] = uStateExpected[0];
		data.uState["u1"] = uStateExpected[1];
		data.uState["u2"] = uStateExpected[2];

		data.ldState["ld0"] = ldStateExpected[0];
		data.ldState["ld1"] = ldStateExpected[1];
		data.ldState["ld2"] = ldStateExpected[2];

		data.vState["v0"] = vStateExpected[0];
		data.vState["v1"] = vStateExpected[1];
		data.vState["v2"] = vStateExpected[2];

		std::vector<unsigned> uState = { data.uState["u0"], data.uState["u1"], data.uState["u2"] };
		std::vector<long double> ldState = { data.ldState["ld0"], data.ldState["ld1"], data.ldState["ld2"] };
		std::vector<vec3> vState = { data.vState["v0"], data.vState["v1"], data.vState["v2"] };

		/*Assert::AreEqual(uStateExpected, uState);
		Assert::AreEqual(ldStateExpected, ldState);
		Assert::AreEqual(vStateExpected, vState);*/
		Assert::IsTrue(std::equal(uStateExpected.begin(), uStateExpected.end(), uState.begin()));
		Assert::IsTrue(std::equal(ldStateExpected.begin(), ldStateExpected.end(), ldState.begin()));
		Assert::IsTrue(std::equal(vStateExpected.begin(), vStateExpected.end(), vState.begin()));
	}
	};
}