#include "pch.h"
#include "CppUnitTest.h"
#include "Interaction2.h"
#include "SystemGLMath.h"
#include <functional>
#include <Particle.h>
#include <string>
#include <cmath>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ActionTrackerUnitTests
{
	const long double EPSILON = 1e-12l;

	/*
	* Simple comparator for long double. Comparisons check for equality based on user suppiled threshold (epsilon).
	*/
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

	/*
	* Helper function for Assert:IsTrue(...).
	*/
	static void assertTrueWithMessageLD(long double lExpected, long double lActual, LongDoubleComparator c) {
		std::wstring msg = L"Expected: " + std::to_wstring(lExpected) + L"; Actual: " + std::to_wstring(lActual);
		Assert::IsTrue(c.areEqual(lExpected, lActual), msg.c_str());
	}

	TEST_CLASS(PotentialUnitTests)
	{
	public:
		/*
		* SANITY CHECK... MIGHT REMOVE
		* ...
		* I DID REMOVE IT BUT THE vec3 STRUCT NEEDS TESTING
		*/
		//TEST_METHOD(Vec3TestMagnitude) {
		//	vec3 zero(0.0l);
		//	vec3 one(1.0l, 0.0l, 0.0l);
		//	vec3 two(sqrtl(4.0l / 3.0l));
		//	vec3 three(sqrtl(3.0l), -sqrtl(3.0l), sqrtl(3.0l));
		//	vec3 four(-sqrtl(16.0l / 3.0l), -sqrtl(16.0l / 3.0l), -sqrtl(16.0l / 3.0l));

		//	long double zeroExp = 0.0;
		//	long double oneExp = 1.0;
		//	long double twoExp = 2.0;
		//	long double threeExp = 3.0;
		//	long double fourExp = 4.0;

		//	long double mag = zero.magnitude();
		//	//std::wstring msg = L"Expected: " + std::to_wstring(zeroExp) + L"; Actual: " + std::to_wstring(mag);
		//	//Assert::IsTrue(ldComp.areEqual(mag, zeroExp), msg.c_str());
		//	assertTrueWithMessageLD(zeroExp, mag);

		//	mag = one.magnitude();
		//	//msg = L"Expected: " + std::to_wstring(oneExp) + L"; Actual: " + std::to_wstring(mag);
		//	//Assert::IsTrue(ldComp.areEqual(mag, oneExp), msg.c_str());
		//	assertTrueWithMessageLD(oneExp, mag);

		//	mag = two.magnitude();
		//	//msg = L"Expected: " + std::to_wstring(twoExp) + L"; Actual: " + std::to_wstring(mag);
		//	//Assert::IsTrue(ldComp.areEqual(mag, twoExp), msg.c_str());
		//	assertTrueWithMessageLD(twoExp, mag);

		//	mag = three.magnitude();
		//	//msg = L"Expected: " + std::to_wstring(threeExp) + L"; Actual: " + std::to_wstring(mag);
		//	//Assert::IsTrue(ldComp.areEqual(mag, threeExp), msg.c_str());
		//	assertTrueWithMessageLD(threeExp, mag);

		//	mag = four.magnitude();
		//	//msg = L"Expected: " + std::to_wstring(fourExp) + L"; Actual: " + std::to_wstring(mag);
		//	//Assert::IsTrue(ldComp.areEqual(mag, fourExp), msg.c_str());
		//	assertTrueWithMessageLD(fourExp, mag);
		//}

		/*
		* Tests the base case (no damping, no displacement, no velocity) of Potential::potentialEnergy(...) for PotentialType::SimpleHarmonicOscillator.
		*/
		TEST_METHOD(PotentialEnergyTestZeroDisplacementZeroVelocityHarmonicOscillator)
		{
			long double equ = 0.0l;
			long double k = 1.0;

			Potential V(PotentialType::SimpleHarmonicOscillator, { equ, k });

			vec3 disp = vec3(0.0l);
			vec3 vel = vec3(0.0l);
			long double time = 0.0l;
			long double mass = 1.0l;

			long double PE = V.potentialEnergy(disp, vel, time, mass);

			long double expected = 0.0l;

			//std::wstring msg = L"Expected: " + std::to_wstring(expected) + L"; Actual: " + std::to_wstring(L);

			//Assert::IsTrue(ldComp.areEqual(L, expected), msg.c_str());
			assertTrueWithMessageLD(expected, PE, ldComp);
		}

		/*
		* Tests the displacement case (no damping, no velocity) of Potential::potentialEnergy(...) for PotentialType::SimpleHarmonicOscillator.
		*/
		TEST_METHOD(PotentialEnergyTestNonZeroDisplacementZeroVelocityHarmonicOscillator)
		{
			long double equ = 0.0l;
			long double k = 1.0;

			Potential V(PotentialType::SimpleHarmonicOscillator, { equ, k });

			vec3 disp = vec3(1.0l, 0.0l, 0.0l);
			vec3 vel = vec3(0.0l);
			long double time = 0.0l;
			long double mass = 1.0l;

			long double PE = V.potentialEnergy(disp, vel, time, mass);

			long double expected = 0.5l;

			//std::wstring msg = L"Expected: " + std::to_wstring(expected) + L"; Actual: " + std::to_wstring(L);

			//Assert::IsTrue(ldComp.areEqual(L, expected), msg.c_str());
			assertTrueWithMessageLD(expected, PE, ldComp);
		}

		/*
		* Tests the velocity case (no damping, no displacement) of Potential::potentialEnergy(...) for PotentialType::SimpleHarmonicOscillator.
		*/
		TEST_METHOD(PotentialEnergyTestZeroDisplacementNonZeroVelocityHarmonicOscillator)
		{
			long double equ = 0.0l;
			long double k = 1.0;

			Potential V(PotentialType::SimpleHarmonicOscillator, { equ, k });

			vec3 disp = vec3(0.0l);
			vec3 vel = vec3(1.0l, 0.0l, 0.0l);
			long double time = 0.0l;
			long double mass = 1.0l;

			long double PE = V.potentialEnergy(disp, vel, time, mass);

			long double expected = 0.0l;

			//std::wstring msg = L"Expected: " + std::to_wstring(expected) + L"; Actual: " + std::to_wstring(L);

			//Assert::IsTrue(ldComp.areEqual(L, expected), msg.c_str());
			assertTrueWithMessageLD(expected, PE, ldComp);
		}

		/*
		* Tests the combined non-damping case of Potential::potentialEnergy(...) for PotentialType::SimpleHarmonicOscillator.
		*/
		TEST_METHOD(PotentialEnergyTestNonZeroDisplacementNonZeroVelocityHarmonicOscillator)
		{
			long double equ = 0.0l;
			long double k = 1.0;

			Potential V(PotentialType::SimpleHarmonicOscillator, { equ, k });

			vec3 disp = vec3(0.0l, 3.0l, 0.0l);
			vec3 vel = vec3(0.0l, 3.0l, 3.0l); // z has no effect b/c vel is off axis
			long double time = 0.0l;
			long double mass = 1.0l;

			long double PE = V.potentialEnergy(disp, vel, time, mass);

			long double expected = 4.5l;
			assertTrueWithMessageLD(expected, PE, ldComp);
		}

		///*
		//* Tests the base case with damping (no displacement, no velocity, no time) of Potential::potentialEnergy(...) for PotentialType::DampedHarmonicOscillator.
		//*/
		//TEST_METHOD(PotentialEnergyTestDampingZeroDisplacementZeroVelocityZeroTimeHarmonicOscillator)
		//{
		//	long double equ = 0.0l;
		//	long double k = 1.0;
		//	long double b = 1.0l;

		//	Potential V(PotentialType::DampedHarmonicOscillator, { equ, k, b });

		//	vec3 disp = vec3(0.0l, 0.0l, 0.0l);
		//	vec3 vel = vec3(0.0l, 0.0l, 0.0l);
		//	long double time = 0.0l;
		//	long double mass = 1.0l;

		//	long double L = V.potentialEnergy(disp, vel, time, mass);

		//	long double expected = 0.0l;
		//	assertTrueWithMessageLD(expected, L, ldComp);
		//}

		///*
		//* Tests the displacement case with damping (no velocity, no time) of Potential::potentialEnergy(...) for PotentialType::DampedHarmonicOscillator.
		//*/
		//TEST_METHOD(PotentialEnergyTestDampingNonZeroDisplacementZeroVelocityZeroTimeHarmonicOscillator)
		//{
		//	long double equ = 0.0l;
		//	long double k = 1.0;
		//	long double b = 1.0l;

		//	Potential V(PotentialType::DampedHarmonicOscillator, { equ, k, b });

		//	vec3 disp = vec3(4.0l, 0.0l, 0.0l);
		//	vec3 vel = vec3(0.0l, 0.0l, 0.0l);
		//	long double time = 0.0l;
		//	long double mass = 1.0l;

		//	long double L = V.potentialEnergy(disp, vel, time, mass);

		//	long double expected = -8.0l;
		//	assertTrueWithMessageLD(expected, L, ldComp);
		//}

		///*
		//* Tests the velocity case with damping (no displacement, no time) of Potential::potentialEnergy(...) for PotentialType::DampedHarmonicOscillator.
		//*/
		//TEST_METHOD(PotentialEnergyTestDampingZeroDisplacementNonZeroVelocityZeroTimeHarmonicOscillator)
		//{
		//	long double equ = 0.0l;
		//	long double k = 1.0;
		//	long double b = 1.0l;

		//	Potential V(PotentialType::DampedHarmonicOscillator, { equ, k, b });

		//	vec3 disp = vec3(0.0l, 0.0l, 0.0l);
		//	vec3 vel = vec3(0.0l, -4.0l, 0.0l);
		//	long double time = 0.0l;
		//	long double mass = 1.0l;

		//	long double L = V.potentialEnergy(disp, vel, time, mass);

		//	long double expected = 8.0l;
		//	assertTrueWithMessageLD(expected, L, ldComp);
		//}

		///*
		//* Tests the velocity case with damping (no displacement, no time) of Potential::potentialEnergy(...) for PotentialType::DampedHarmonicOscillator.
		//*/
		//TEST_METHOD(PotentialEnergyTestDampingZeroDisplacementZeroVelocityNonZeroTimeHarmonicOscillator)
		//{
		//	long double equ = 0.0l;
		//	long double k = 1.0;
		//	long double b = 1.0l;

		//	Potential V(PotentialType::DampedHarmonicOscillator, { equ, k, b });

		//	vec3 disp = vec3(0.0l, 0.0l, 0.0l);
		//	vec3 vel = vec3(0.0l, 0.0l, 0.0l);
		//	long double time = 15.0l;
		//	long double mass = 1.0l;

		//	long double L = V.potentialEnergy(disp, vel, time, mass);

		//	long double expected = 0.0l;
		//	assertTrueWithMessageLD(expected, L, ldComp);
		//}

		///*
		//* Tests the general case with damping of Potential::potentialEnergy(...) for PotentialType::DampedHarmonicOscillator.
		//*/
		//TEST_METHOD(PotentialEnergyTestGeneralDampedHarmonicOscillator)
		//{
		//	long double equ = 0.0l;
		//	long double k = 1.0;
		//	long double b = 1.0l;

		//	Potential V(PotentialType::DampedHarmonicOscillator, { equ, k, b });

		//	vec3 disp = vec3(-sqrtl(2.0l), 0.0l, sqrtl(2.0l));
		//	vec3 vel = vec3(sqrtl(8.0l), 100000.0l, -sqrtl(8.0l)); // velocity perp. to displacement_hat should have no effect
		//	long double time = logl(2);
		//	long double mass = 1.0l;

		//	long double L = V.potentialEnergy(disp, vel, time, mass);

		//	long double expected = 12.0l;
		//	assertTrueWithMessageLD(expected, L, ldComp);
		//}

		/*
		* Tests the base case (origin, g = 0) of Potential::potentialEnergy for PotentialType::PlanetaryGravitationalPotential.
		*/
		TEST_METHOD(PotentialEnergyTestZeroOriginZeroGZeroDisplacementZeroVelocityPlanetaryGravitationalPotential) {
			vec3 g = vec3(0.0l, 0.0l, 0.0l);
			long double equ = 0.0l;

			Potential V(PotentialType::PlanetaryGravitationalPotential, { g.x, g.y, g.z, equ });

			vec3 disp = vec3(0.0l);
			vec3 vel = vec3(0.0l);
			long double time = logl(2); // shouldn't matter
			long double mass = 1.0l;

			long double PE = V.potentialEnergy(disp, vel, time, mass);

			long double expected = 0.0l;
			assertTrueWithMessageLD(expected, PE, ldComp);
		}

		/*
		* Tests the non-zero origin case (g, displacement, velocity = 0) of Potential::potentialEnergy for PotentialType::PlanetaryGravitationalPotential.
		*/
		TEST_METHOD(PotentialEnergyTestNonZeroOriginZeroGZeroDisplacementZeroVelocityPlanetaryGravitationalPotential) {
			vec3 g = vec3(0.0l, 0.0l, 0.0l);
			long double equ = 2.0l;

			Potential V(PotentialType::PlanetaryGravitationalPotential, { g.x, g.y, g.z, equ });

			vec3 disp = vec3(0.0l);
			vec3 vel = vec3(0.0l);
			long double time = logl(2); // shouldn't matter
			long double mass = 1.0l;

			long double PE = V.potentialEnergy(disp, vel, time, mass);

			long double expected = 0.0l;
			assertTrueWithMessageLD(expected, PE, ldComp);
		}

		/*
		* Tests the non-zero g case (origin, displacement, velocity = 0) of Potential::potentialEnergy for PotentialType::PlanetaryGravitationalPotential.
		*/
		TEST_METHOD(PotentialEnergyTestZeroOriginNonZeroGZeroDisplacementZeroVelocityPlanetaryGravitationalPotential) {
			vec3 g = vec3(sqrtl(2.0l), -sqrtl(2.0l), 0.0l);
			long double equ = 0.0l;

			Potential V(PotentialType::PlanetaryGravitationalPotential, { g.x, g.y, g.z, equ });

			vec3 disp = vec3(0.0l);
			vec3 vel = vec3(0.0l);
			long double time = logl(2); // shouldn't matter
			long double mass = 1.0l;

			long double PE = V.potentialEnergy(disp, vel, time, mass);

			long double expected = 0.0l;
			assertTrueWithMessageLD(expected, PE, ldComp);
		}

		/*
		* Tests the non-zero displacement case (origin, g, velocity = 0) of Potential::potentialEnergy for PotentialType::PlanetaryGravitationalPotential.
		*/
		TEST_METHOD(PotentialEnergyTestZeroOriginZeroGNonZeroDisplacementZeroVelocityPlanetaryGravitationalPotential) {
			vec3 g = vec3(0.0l, 0.0l, 0.0l);
			long double equ = 0.0l;

			Potential V(PotentialType::PlanetaryGravitationalPotential, { g.x, g.y, g.z, equ });

			vec3 disp = vec3(0.0l, sqrtl(2.0l), -sqrtl(2.0l));
			vec3 vel = vec3(0.0l);
			long double time = logl(2); // shouldn't matter
			long double mass = 1.0l;

			long double L = V.potentialEnergy(disp, vel, time, mass);

			long double expected = 0.0l;
			assertTrueWithMessageLD(expected, L, ldComp);
		}

		/*
		* Tests the non-zero velocity case (origin, g, displacement = 0) of Potential::potentialEnergy for PotentialType::PlanetaryGravitationalPotential.
		*/
		TEST_METHOD(PotentialEnergyTestZeroOriginZeroGZeroDisplacementNonZeroVelocityPlanetaryGravitationalPotential) {
			vec3 g = vec3(0.0l, 0.0l, 0.0l);
			long double equ = 0.0l;

			Potential V(PotentialType::PlanetaryGravitationalPotential, { g.x, g.y, g.z, equ });

			vec3 disp = vec3(0.0l);
			vec3 vel = vec3(-sqrtl(2.0l), 0.0, sqrtl(2.0l));
			long double time = logl(2); // shouldn't matter
			long double mass = 1.0l;

			long double PE = V.potentialEnergy(disp, vel, time, mass);

			long double expected = 0.0l;
			assertTrueWithMessageLD(expected, PE, ldComp);
		}

		/*
		* Tests the general case of Potential::potentialEnergy for PotentialType::PlanetaryGravitationalPotential.
		*/
		TEST_METHOD(PotentialEnergyTestGeneralPlanetaryGravitationalPotential) {
			vec3 g = vec3(0.0l, -sqrtl(12.5), sqrtl(12.5));
			long double equ = 3.0l;

			Potential V(PotentialType::PlanetaryGravitationalPotential, { g.x, g.y, g.z, equ });

			vec3 disp = vec3(0.0l, sqrtl(8.0l), -sqrtl(8.0l));
			vec3 vel = vec3(100000.0l, 1.0, -1.0);
			long double time = logl(2); // shouldn't matter
			long double mass = 1.0l;

			long double PE = V.potentialEnergy(disp, vel, time, mass);

			long double expected = -35.0l;
			assertTrueWithMessageLD(expected, PE, ldComp);
		}

	private:
		LongDoubleComparator ldComp = LongDoubleComparator(EPSILON);

		//void assertTrueWithMessageLD(long double lExpected, long double lActual) {
		//	std::wstring msg = L"Expected: " + std::to_wstring(lExpected) + L"; Actual: " + std::to_wstring(lActual);
		//	Assert::IsTrue(ldComp.areEqual(lExpected, lActual), msg.c_str());
		//}
	};

	TEST_CLASS(InteractionUnitTests)
	{
	public:
		/*
		* Tests the base case (spring exists: k != 0; & everything else is zero/defaults) 
		*	of Interaction::potentialEnergy(...) for PotentialType::SimpleHarmonicOscillator with no damping.
		*/
		TEST_METHOD(PotentialEnergyTestBaseHarmonicOscillator) {
			long double equ = 0.0l;
			long double k = 1.0;

			Potential V(PotentialType::SimpleHarmonicOscillator, { equ, k });

			Particle p1 = Particle();
			Particle p2 = Particle();

			Interaction I = Interaction(p1, p2, V, Rayleigh());
			I.recordDisplacement();
			I.recordRelativeVelocity();

			long double PE = I.potentialEnergy();
			long double expected = 0.0l;

			assertTrueWithMessageLD(expected, PE, ldComp);
		}

		/*
		* Tests the stationary displaced case of Interaction::potentialEnergy for PotentialType::SimpleHarmonicOscillator with no damping.
		*/
		TEST_METHOD(PotentialEnergyTestStationaryHarmonicOscillator) {
			long double equ = 0.0l;
			long double k = 1.0;

			Potential V(PotentialType::SimpleHarmonicOscillator, { equ, k });

			Particle p1 = Particle(vec3(1.0l, 0.0l, 0.0l), vec3(0.0l), 1.0l);
			Particle p2 = Particle(vec3(-1.0l, 0.0l, 0.0l), vec3(0.0l), 1.0l);

			Interaction I = Interaction(p1, p2, V, Rayleigh());
			I.recordDisplacement();
			I.recordRelativeVelocity();

			long double PE = I.potentialEnergy();
			long double expected = 2.0l;

			assertTrueWithMessageLD(expected, PE, ldComp);
		}

		/*
		* Tests the perpendicularly moving displaced case of Interaction::potentialEnergy for PotentialType::SimpleHarmonicOscillator with no damping.
		*/
		TEST_METHOD(PotentialEnergyTestPerpendicularVelocityHarmonicOscillator) {
			long double equ = 0.0l;
			long double k = 1.0;

			Potential V(PotentialType::SimpleHarmonicOscillator, { equ, k });

			Particle p1 = Particle(vec3(1.0l, 0.0l, 0.0l), vec3(0.0l, 0.0l, 111111.0l), 1.0l);
			Particle p2 = Particle(vec3(-1.0l, 0.0l, 0.0l), vec3(0.0l, 1.0l, 0.0l), 2.0l);

			Interaction I = Interaction(p1, p2, V, Rayleigh());
			I.recordDisplacement();
			I.recordRelativeVelocity();

			long double PE = I.potentialEnergy();
			long double expected = 2.0l;

			assertTrueWithMessageLD(expected, PE, ldComp);
		}

		/*
		* Tests the general case of Interaction::potentialEnergy for PotentialType::SimpleHarmonicOscillator with no damping.
		*/
		TEST_METHOD(PotentialEnergyTestGeneralHarmonicOscillator) {
			long double equ = 1.0l;
			long double k = 1.0;

			Potential V(PotentialType::SimpleHarmonicOscillator, { equ, k });

			Particle p1 = Particle(vec3(2.0l, 0.0l, 0.0l), vec3(3.0l, 0.0l, 111111.0l), 1.0l);
			Particle p2 = Particle(vec3(-2.0l, 0.0l, 0.0l), vec3(5.0l, 1.0l, 0.0l), 3.0l);

			Interaction I = Interaction(p1, p2, V, Rayleigh());
			I.recordDisplacement();
			I.recordRelativeVelocity();

			long double PE = I.potentialEnergy();
			long double expected = 4.5l;

			assertTrueWithMessageLD(expected, PE, ldComp);
		}

		///*
		//* Tests the base case (spring exists: k != 0, b != 0; & everything else is zero/defaults)
		//*	of Interaction::potentialEnergy(...) for PotentialType::DampedHarmonicOscillator with damping.
		//*/
		//TEST_METHOD(PotentialEnergyTestBaseDampingHarmonicOscillator) {
		//	long double equ = 0.0l;
		//	long double k = 1.0;
		//	long double b = 1.0l;

		//	Potential V(PotentialType::DampedHarmonicOscillator, { equ, k, b });

		//	Particle p1 = Particle();
		//	Particle p2 = Particle();

		//	Interaction I = Interaction(p1, p2, V);
		//	I.recordDisplacement();
		//	I.recordRelativeVelocity();

		//	long double L = I.potentialEnergy();
		//	long double expected = 0.0l;

		//	assertTrueWithMessageLD(expected, L, ldComp);
		//}

		///*
		//* Tests the stationary displaced case of Interaction::potentialEnergy for PotentialType::DampedHarmonicOscillator with damping.
		//*/
		//TEST_METHOD(PotentialEnergyTestStationaryDampingHarmonicOscillator) {
		//	long double equ = 0.0l;
		//	long double k = 1.0;
		//	long double b = 2.0l;

		//	Potential V(PotentialType::DampedHarmonicOscillator, { equ, k, b });

		//	// for damping make sure t != 0
		//	// using t = ln (WHOLE_NUMBER) leads to more WHOLE_NUMBERS
		//	long double m1 = 1.0l;
		//	long double m2 = 2.0l;
		//	long double time = logl(5);
		//	Particle p1 = Particle(vec3(1.0l, 0.0l, 0.0l), vec3(0.0l), m1, time);
		//	Particle p2 = Particle(vec3(-1.0l, 0.0l, 0.0l), vec3(0.0l), m2, time);

		//	Interaction I = Interaction(p1, p2, V);
		//	I.recordDisplacement();
		//	I.recordRelativeVelocity();

		//	long double L = I.potentialEnergy();
		//	long double expected = -60.0l;

		//	assertTrueWithMessageLD(expected, L, ldComp);
		//}

		///*
		//* Tests the perpendicularly moving displaced case of Interaction::potentialEnergy for PotentialType::DampedHarmonicOscillator.
		//*/
		//TEST_METHOD(PotentialEnergyTestPerpendicularVelocityDampingHarmonicOscillator) {
		//	long double equ = 0.0l;
		//	long double k = 1.0;
		//	long double b = 2.0l;

		//	Potential V(PotentialType::DampedHarmonicOscillator, { equ, k, b });

		//	long double m1 = 1.0l;
		//	long double m2 = 2.0l;
		//	long double time = logl(5);
		//	Particle p1 = Particle(vec3(1.0l, 0.0l, 0.0l), vec3(0.0l, 0.0l, 111111.0l), m1, time);
		//	Particle p2 = Particle(vec3(-1.0l, 0.0l, 0.0l), vec3(0.0l, 1.0l, 0.0l), m2, time);

		//	Interaction I = Interaction(p1, p2, V);
		//	I.recordDisplacement();
		//	I.recordRelativeVelocity();

		//	long double L = I.potentialEnergy();
		//	long double expected = -60.0l;

		//	assertTrueWithMessageLD(expected, L, ldComp);
		//}

		///*
		//* Tests the general case of Interaction::potentialEnergy for PotentialType::DampedHarmonicOscillator with damping.
		//*/
		//TEST_METHOD(PotentialEnergyTestGeneralDampingHarmonicOscillator) {
		//	long double equ = 1.0l;
		//	long double k = 1.0;
		//	long double b = 3.0l;

		//	Potential V(PotentialType::DampedHarmonicOscillator, { equ, k, b });

		//	long double m1 = 1.0l;
		//	long double m2 = 3.0l;
		//	long double time = logl(5);
		//	Particle p1 = Particle(vec3(2.0l, 0.0l, 0.0l), vec3(3.0l, 0.0l, 111111.0l), m1, time);
		//	Particle p2 = Particle(vec3(-2.0l, 0.0l, 0.0l), vec3(5.0l, 1.0l, 0.0l), m2, time);

		//	Interaction I = Interaction(p1, p2, V);
		//	I.recordDisplacement();
		//	I.recordRelativeVelocity();

		//	long double L = I.potentialEnergy();
		//	long double expected = -515.0l;

		//	assertTrueWithMessageLD(expected, L, ldComp);
		//}

	private:
		LongDoubleComparator ldComp = LongDoubleComparator(EPSILON);
	};

	TEST_CLASS(UniversalInteractionUnitTests) {
	public:
		/*
		* Tests the base case (gravity exists: g != 0; no particles) of UniversalInteraction::potentialEnergySum for PotentialType::PlanetaryGravitationalPotential.
		*/
		TEST_METHOD(PotentialEnergySumTestBasePlanetaryGravitationalPotential) {
			vec3 g = vec3(10.0l);
			long double equ = 0.0l;

			Potential V(PotentialType::PlanetaryGravitationalPotential, { g.x, g.y, g.z, equ });

			UniversalInteraction uI = UniversalInteraction({}, V, Rayleigh());

			long double PE = uI.potentialEnergySum();

			long double expected = 0.0l;

			assertTrueWithMessageLD(expected, PE, ldComp);
		}

		/*
		* Tests the single particle base case of UniversalInteraction::potentialEnergySum for PotentialType::PlanetaryGravitationalPotential.
		*/
		TEST_METHOD(PotentialEnergySumTestSingleParticleBasePlanetaryGravitationalPotential) {
			vec3 g = vec3(10.0l);
			long double equ = 0.0l;

			Potential V(PotentialType::PlanetaryGravitationalPotential, { g.x, g.y, g.z, equ });

			Particle p1 = Particle();

			UniversalInteraction uI = UniversalInteraction({ p1 }, V, Rayleigh());

			long double PE = uI.potentialEnergySum();

			long double expected = 0.0l;

			assertTrueWithMessageLD(expected, PE, ldComp);
		}

		/*
		* Tests the single stationary particle case of UniversalInteraction::potentialEnergySum for PotentialType::PlanetaryGravitationalPotential.
		*/
		TEST_METHOD(PotentialEnergySumTestSingleParticleNonZeroPositionZeroVelocityPlanetaryGravitationalPotential) {
			vec3 g = vec3(10.0l);
			long double equ = -0.5l * sqrtl(12.0l);

			Potential V(PotentialType::PlanetaryGravitationalPotential, { g.x, g.y, g.z, equ });

			Particle p1 = Particle(vec3(-2.0l), vec3(0.0l), 1.0l);

			UniversalInteraction uI = UniversalInteraction({ p1 }, V, Rayleigh());

			long double PE = uI.potentialEnergySum();

			long double expected = -30.0l;

			assertTrueWithMessageLD(expected, PE, ldComp);
		}

		/*
		* Tests the single stationary particle case of UniversalInteraction::potentialEnergySum for PotentialType::PlanetaryGravitationalPotential.
		*/
		TEST_METHOD(PotentialEnergySumTestSingleParticleZeroPositionNonZeroVelocityPlanetaryGravitationalPotential) {
			vec3 g = vec3(10.0l);
			long double equ = sqrtl(12.0l);

			Potential V(PotentialType::PlanetaryGravitationalPotential, { g.x, g.y, g.z, equ });

			Particle p1 = Particle(vec3(2.0l), vec3(2.0l), 1.0l);

			UniversalInteraction uI = UniversalInteraction({ p1 }, V, Rayleigh());

			long double PE = uI.potentialEnergySum();

			long double expected = 0;

			assertTrueWithMessageLD(expected, PE, ldComp);
		}

		/*
		* Tests the single general particle case of UniversalInteraction::potentialEnergySum for PotentialType::PlanetaryGravitationalPotential.
		*/
		TEST_METHOD(PotentialEnergySumTestSingleParticleGeneralPlanetaryGravitationalPotential) {
			vec3 g = vec3(10.0l, 0.0l, 0.0l);
			long double equ = 3.0l;

			Potential V(PotentialType::PlanetaryGravitationalPotential, { g.x, g.y, g.z, equ });

			Particle p1 = Particle(-vec3(5.0l), -vec3(2.0l), 1.0l);

			UniversalInteraction uI = UniversalInteraction({ p1 }, V, Rayleigh());

			long double PE = uI.potentialEnergySum();

			long double expected = -80.0l;

			assertTrueWithMessageLD(expected, PE, ldComp);
		}

		/*
		* Tests the ten particle base case of UniversalInteraction::potentialEnergySum for PotentialType::PlanetaryGravitationalPotential.
		*/
		TEST_METHOD(PotentialEnergySumTestTenParticleBasePlanetaryGravitationalPotential) {
			vec3 g = vec3(10.0l);
			long double equ = 0.0l;

			Potential V(PotentialType::PlanetaryGravitationalPotential, { g.x, g.y, g.z, equ });

			std::vector<Particle> parts;

			long double expected = 0.0l;

			for (int i = 0; i < 10; i++) {
				Particle p = Particle();
				parts.push_back(p);
				expected += V.potentialEnergy(p.getPosition(), p.getVelocity(), p.getTime(), p.getMass());
			}
			
			UniversalInteraction uI = UniversalInteraction({}, V, Rayleigh());

			long double PE = uI.potentialEnergySum();

			assertTrueWithMessageLD(expected, PE, ldComp);
		}
	private:
		LongDoubleComparator ldComp = LongDoubleComparator(EPSILON);
	};
}
