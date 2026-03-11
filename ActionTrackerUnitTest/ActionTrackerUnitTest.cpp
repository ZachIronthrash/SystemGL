#include "pch.h"
#include "Interaction2.h"
#include "SystemGLMath.h"
#include "CppUnitTest.h"
#include <functional>
#include <string>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ActionTrackerUnitTest
{
	const long double EPSILON = 1e-12l;

	class LongDoubleComparator {
	public:
		LongDoubleComparator(long double epsilon) : epsilon(epsilon) {}
		bool operator()(const long double& a, const long double& b) {
			return a < b;
		}

		bool areEqual(const long double& a, const long double& b) const {
			return fabsl(a - b) < epsilon;
		}
	private:
		long double epsilon;

	};

	TEST_CLASS(ActionTrackerUnitTest)
	{
	public:
		/*
		* SANITY CHECK... MIGHT REMOVE
		*/
		TEST_METHOD(Vec3TestMagnitude) {
			vec3 zero(0.0l);
			vec3 one(1.0l, 0.0l, 0.0l);
			vec3 two(sqrtl(4.0l / 3.0l));
			vec3 three(sqrtl(3.0l), -sqrtl(3.0l), sqrtl(3.0l));
			vec3 four(-sqrtl(16.0l / 3.0l), -sqrtl(16.0l / 3.0l), -sqrtl(16.0l / 3.0l));

			long double zeroExp = 0.0;
			long double oneExp = 1.0;
			long double twoExp = 2.0;
			long double threeExp = 3.0;
			long double fourExp = 4.0;

			long double mag = zero.magnitude();
			//std::wstring msg = L"Expected: " + std::to_wstring(zeroExp) + L"; Actual: " + std::to_wstring(mag);
			//Assert::IsTrue(ldComp.areEqual(mag, zeroExp), msg.c_str());
			assertTrueWithErrorLD(zeroExp, mag);

			mag = one.magnitude();
			//msg = L"Expected: " + std::to_wstring(oneExp) + L"; Actual: " + std::to_wstring(mag);
			//Assert::IsTrue(ldComp.areEqual(mag, oneExp), msg.c_str());
			assertTrueWithErrorLD(oneExp, mag);

			mag = two.magnitude();
			//msg = L"Expected: " + std::to_wstring(twoExp) + L"; Actual: " + std::to_wstring(mag);
			//Assert::IsTrue(ldComp.areEqual(mag, twoExp), msg.c_str());
			assertTrueWithErrorLD(twoExp, mag);

			mag = three.magnitude();
			//msg = L"Expected: " + std::to_wstring(threeExp) + L"; Actual: " + std::to_wstring(mag);
			//Assert::IsTrue(ldComp.areEqual(mag, threeExp), msg.c_str());
			assertTrueWithErrorLD(threeExp, mag);

			mag = four.magnitude();
			//msg = L"Expected: " + std::to_wstring(fourExp) + L"; Actual: " + std::to_wstring(mag);
			//Assert::IsTrue(ldComp.areEqual(mag, fourExp), msg.c_str());
			assertTrueWithErrorLD(fourExp, mag);
		}

		/*
		* Tests the base case (no damping, no displacement, no velocity) of Potential::lagrangian(...) for PotentialType::DampedHarmonicOscillator.
		*/
		TEST_METHOD(LagrangianTestNoDampingZeroDisplacementZeroVelocityHarmonicOscillator)
		{
			long double equ = 0.0l;
			long double k = 1.0;
			long double b = 0.0l;

			Potential V(PotentialType::DampedHarmonicOscillator, { equ, k, b });

			vec3 disp = vec3(0.0l);
			vec3 vel = vec3(0.0l);
			long double time = 0.0l;
			long double mass = 1.0l;

			long double L = V.lagrangian(disp, vel, time, mass);

			long double expected = 0.0l;

			//std::wstring msg = L"Expected: " + std::to_wstring(expected) + L"; Actual: " + std::to_wstring(L);

			//Assert::IsTrue(ldComp.areEqual(L, expected), msg.c_str());
			assertTrueWithErrorLD(expected, L);
		}

		/*
		* Tests the displacement case (no damping, no velocity) of Potential::lagrangian(...) for PotentialType::DampedHarmonicOscillator.
		*/
		TEST_METHOD(LagrangianTestNoDampingNonZeroDisplacementZeroVelocityHarmonicOscillator)
		{
			long double equ = 0.0l;
			long double k = 1.0;
			long double b = 0.0l;

			Potential V(PotentialType::DampedHarmonicOscillator, { equ, k, b });

			vec3 disp = vec3(1.0l, 0.0l, 0.0l);
			vec3 vel = vec3(0.0l);
			long double time = 0.0l;
			long double mass = 1.0l;

			long double L = V.lagrangian(disp, vel, time, mass);

			long double expected = -0.5l;

			//std::wstring msg = L"Expected: " + std::to_wstring(expected) + L"; Actual: " + std::to_wstring(L);

			//Assert::IsTrue(ldComp.areEqual(L, expected), msg.c_str());
			assertTrueWithErrorLD(expected, L);
		}

		/*
		* Tests the velocity case (no damping, no displacement) of Potential::lagrangian(...) for PotentialType::DampedHarmonicOscillator.
		*/
		TEST_METHOD(LagrangianTestNoDampingZeroDisplacementNonZeroVelocityHarmonicOscillator)
		{
			long double equ = 0.0l;
			long double k = 1.0;
			long double b = 0.0l;

			Potential V(PotentialType::DampedHarmonicOscillator, { equ, k, b });

			vec3 disp = vec3(0.0l);
			vec3 vel = vec3(1.0l, 0.0l, 0.0l);
			long double time = 0.0l;
			long double mass = 1.0l;

			long double L = V.lagrangian(disp, vel, time, mass);

			long double expected = 0.5l;

			//std::wstring msg = L"Expected: " + std::to_wstring(expected) + L"; Actual: " + std::to_wstring(L);

			//Assert::IsTrue(ldComp.areEqual(L, expected), msg.c_str());
			assertTrueWithErrorLD(expected, L);
		}

		/*
		* Tests the combined non-damping case of Potential::lagrangian(...) for PotentialType::DampedHarmonicOscillator.
		*/
		TEST_METHOD(LagrangianTestNoDampingNonZeroDisplacementNonZeroVelocityHarmonicOscillator)
		{
			long double equ = 0.0l;
			long double k = 1.0;
			long double b = 0.0l;

			Potential V(PotentialType::DampedHarmonicOscillator, { equ, k, b });

			vec3 disp = vec3(0.0l, 3.0l, 0.0l);
			vec3 vel = vec3(0.0l, 0.0l, 3.0l);
			long double time = 0.0l;
			long double mass = 1.0l;

			long double L = V.lagrangian(disp, vel, time, mass);

			long double expected = 0.0l;
			assertTrueWithErrorLD(expected, L);
		}

		/*
		* Tests the base case with damping (no displacement, no velocity, no time) of Potential::lagrangian(...) for PotentialType::DampedHarmonicOscillator.
		*/
		TEST_METHOD(LagrangianTestDampingZeroDisplacementZeroVelocityZeroTimeHarmonicOscillator)
		{
			long double equ = 0.0l;
			long double k = 1.0;
			long double b = 1.0l;

			Potential V(PotentialType::DampedHarmonicOscillator, { equ, k, b });

			vec3 disp = vec3(0.0l, 0.0l, 0.0l);
			vec3 vel = vec3(0.0l, 0.0l, 0.0l);
			long double time = 0.0l;
			long double mass = 1.0l;

			long double L = V.lagrangian(disp, vel, time, mass);

			long double expected = 0.0l;
			assertTrueWithErrorLD(expected, L);
		}

		/*
		* Tests the displacement case with damping (no velocity, no time) of Potential::lagrangian(...) for PotentialType::DampedHarmonicOscillator.
		*/
		TEST_METHOD(LagrangianTestDampingNonZeroDisplacementZeroVelocityZeroTimeHarmonicOscillator)
		{
			long double equ = 0.0l;
			long double k = 1.0;
			long double b = 1.0l;

			Potential V(PotentialType::DampedHarmonicOscillator, { equ, k, b });

			vec3 disp = vec3(4.0l, 0.0l, 0.0l);
			vec3 vel = vec3(0.0l, 0.0l, 0.0l);
			long double time = 0.0l;
			long double mass = 1.0l;

			long double L = V.lagrangian(disp, vel, time, mass);

			long double expected = -8.0l;
			assertTrueWithErrorLD(expected, L);
		}

		/*
		* Tests the velocity case with damping (no displacement, no time) of Potential::lagrangian(...) for PotentialType::DampedHarmonicOscillator.
		*/
		TEST_METHOD(LagrangianTestDampingZeroDisplacementNonZeroVelocityZeroTimeHarmonicOscillator)
		{
			long double equ = 0.0l;
			long double k = 1.0;
			long double b = 1.0l;

			Potential V(PotentialType::DampedHarmonicOscillator, { equ, k, b });

			vec3 disp = vec3(0.0l, 0.0l, 0.0l);
			vec3 vel = vec3(0.0l, -4.0l, 0.0l);
			long double time = 0.0l;
			long double mass = 1.0l;

			long double L = V.lagrangian(disp, vel, time, mass);

			long double expected = 8.0l;
			assertTrueWithErrorLD(expected, L);
		}

		/*
		* Tests the velocity case with damping (no displacement, no time) of Potential::lagrangian(...) for PotentialType::DampedHarmonicOscillator.
		*/
		TEST_METHOD(LagrangianTestDampingZeroDisplacementZeroVelocityNonZeroTimeHarmonicOscillator)
		{
			long double equ = 0.0l;
			long double k = 1.0;
			long double b = 1.0l;

			Potential V(PotentialType::DampedHarmonicOscillator, { equ, k, b });

			vec3 disp = vec3(0.0l, 0.0l, 0.0l);
			vec3 vel = vec3(0.0l, 0.0l, 0.0l);
			long double time = 15.0l;
			long double mass = 1.0l;

			long double L = V.lagrangian(disp, vel, time, mass);

			long double expected = 0.0l;
			assertTrueWithErrorLD(expected, L);
		}

		/*
		* Tests the combined case with damping of Potential::lagrangian(...) for PotentialType::DampedHarmonicOscillator.
		*/
		TEST_METHOD(LagrangianTestDampingNonZeroDisplacementNonZeroVelocityNonZeroTimeHarmonicOscillator)
		{
			long double equ = 0.0l;
			long double k = 1.0;
			long double b = 1.0l;

			Potential V(PotentialType::DampedHarmonicOscillator, { equ, k, b });

			vec3 disp = vec3(-sqrtl(2.0l), 0.0l, sqrtl(2.0l));
			vec3 vel = vec3(-sqrtl(3));
			long double time = logl(2);
			long double mass = 1.0l;

			long double L = V.lagrangian(disp, vel, time, mass);

			long double expected = 5.0l;
			assertTrueWithErrorLD(expected, L);
		}

		/*
		* Tests the base case (origin, g = 0) of Potential::lagrangian for PotentialType::PlanetaryGravitationalPotential.
		*/
		TEST_METHOD(LagrangianTestZeroOriginZeroGZeroDisplacementZeroVelocityPlanetaryGravitationalPotential) {
			vec3 g = vec3(0.0l, 0.0l, 0.0l);
			long double equ = 0.0l;

			Potential V(PotentialType::PlanetaryGravitationalPotential, { g.x, g.y, g.z, equ });

			vec3 disp = vec3(0.0l);
			vec3 vel = vec3(0.0l);
			long double time = logl(2); // shouldn't matter
			long double mass = 1.0l;

			long double L = V.lagrangian(disp, vel, time, mass);

			long double expected = 0.0l;
			assertTrueWithErrorLD(expected, L);
		}

		/*
		* Tests the non-zero origin case (g, displacement, velocity = 0) of Potential::lagrangian for PotentialType::PlanetaryGravitationalPotential.
		*/
		TEST_METHOD(LagrangianTestNonZeroOriginZeroGZeroDisplacementZeroVelocityPlanetaryGravitationalPotential) {
			vec3 g = vec3(0.0l, 0.0l, 0.0l);
			long double equ = 2.0l;

			Potential V(PotentialType::PlanetaryGravitationalPotential, { g.x, g.y, g.z, equ });

			vec3 disp = vec3(0.0l);
			vec3 vel = vec3(0.0l);
			long double time = logl(2); // shouldn't matter
			long double mass = 1.0l;

			long double L = V.lagrangian(disp, vel, time, mass);

			long double expected = 0.0l;
			assertTrueWithErrorLD(expected, L);
		}

		/*
		* Tests the non-zero g case (origin, displacement, velocity = 0) of Potential::lagrangian for PotentialType::PlanetaryGravitationalPotential.
		*/
		TEST_METHOD(LagrangianTestZeroOriginNonZeroGZeroDisplacementZeroVelocityPlanetaryGravitationalPotential) {
			vec3 g = vec3(sqrtl(2.0l), -sqrtl(2.0l), 0.0l);
			long double equ = 0.0l;

			Potential V(PotentialType::PlanetaryGravitationalPotential, { g.x, g.y, g.z, equ });

			vec3 disp = vec3(0.0l);
			vec3 vel = vec3(0.0l);
			long double time = logl(2); // shouldn't matter
			long double mass = 1.0l;

			long double L = V.lagrangian(disp, vel, time, mass);

			long double expected = 0.0l;
			assertTrueWithErrorLD(expected, L);
		}

		/*
		* Tests the non-zero displacement case (origin, g, velocity = 0) of Potential::lagrangian for PotentialType::PlanetaryGravitationalPotential.
		*/
		TEST_METHOD(LagrangianTestZeroOriginZeroGNonZeroDisplacementZeroVelocityPlanetaryGravitationalPotential) {
			vec3 g = vec3(0.0l, 0.0l, 0.0l);
			long double equ = 0.0l;

			Potential V(PotentialType::PlanetaryGravitationalPotential, { g.x, g.y, g.z, equ });

			vec3 disp = vec3(0.0l, sqrtl(2.0l), -sqrtl(2.0l));
			vec3 vel = vec3(0.0l);
			long double time = logl(2); // shouldn't matter
			long double mass = 1.0l;

			long double L = V.lagrangian(disp, vel, time, mass);

			long double expected = 0.0l;
			assertTrueWithErrorLD(expected, L);
		}

		/*
		* Tests the non-zero velocity case (origin, g, displacement = 0) of Potential::lagrangian for PotentialType::PlanetaryGravitationalPotential.
		*/
		TEST_METHOD(LagrangianTestZeroOriginZeroGZeroDisplacementNonZeroVelocityPlanetaryGravitationalPotential) {
			vec3 g = vec3(0.0l, 0.0l, 0.0l);
			long double equ = 0.0l;

			Potential V(PotentialType::PlanetaryGravitationalPotential, { g.x, g.y, g.z, equ });

			vec3 disp = vec3(0.0l);
			vec3 vel = vec3(-sqrtl(2.0l), 0.0, sqrtl(2.0l));
			long double time = logl(2); // shouldn't matter
			long double mass = 1.0l;

			long double L = V.lagrangian(disp, vel, time, mass);

			long double expected = 2.0l;
			assertTrueWithErrorLD(expected, L);
		}

	private:
		LongDoubleComparator ldComp = LongDoubleComparator(EPSILON);

		void assertTrueWithErrorLD(long double lExpected, long double lActual) {
			std::wstring msg = L"Expected: " + std::to_wstring(lExpected) + L"; Actual: " + std::to_wstring(lActual);
			Assert::IsTrue(ldComp.areEqual(lExpected, lActual), msg.c_str());
		}
	};
}
