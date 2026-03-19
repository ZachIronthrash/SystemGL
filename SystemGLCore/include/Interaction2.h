#pragma once
#include "Particle.h"
#include "SystemGLMath.h"
#include <iostream>
#include <type_traits>
#include <utility>
#include <vector>

#include <cassert>

// ToDo:
//	- Use enum classes to define the allowed parameters for each case rather than hard-coding it

enum class PotentialType {
	Default, // non-damped harmonic oscillater w/ equilibrium = vec3(0)
	//DampedHarmonicOscillator,
	SimpleHarmonicOscillator,
	PlanetaryGravitationalPotential, // constant acceleration field
	LogarithmForce
};

enum class RayleighType {
	Default,
	VelocityDissipation // NOTE: with an interaction this acts as damping; with a universal interaction this acts as air resistance
};

class Potential {
public:
	Potential(enum PotentialType t = PotentialType::Default, std::vector<long double> param = {});

	vec3 positionGradientOverM(vec3 displacement, long double mass);
	vec3 velocityGradientOverM(vec3 velocity, long double mass);

	//vec3 gradientOverM(vec3 displacement, vec3 velocity, long double mass) {
	//	//switch (type) {
	//	//case PotentialType::DampedHarmonicOscillator:
	//	//	return parameters.at(1) * displacement.normalized() * (displacement.magnitude() - parameters.at(0)) / mass; // k * (vec3) d * (|d| - equilibriumDisplacement)
	//	//	break;
	//	//case PotentialType::PlanetaryGravitationalPotential:
	//	//	return -vec3(parameters.at(0), parameters.at(1), parameters.at(2)); // -g
	//	//	break;
	//	//	//case PotentialType::LogarithmForce: // I don't think this is conservative, but it might be fun to mess with
	//	//		//return parameters.at(1) * parameters.at(0) * displacement.normalized() * std::log(displacement.magnitude() / parameters.at(0)) / mass;
	//	//default: // Type::Default
	//	//	return parameters.at(0) * displacement / mass;
	//	//	break;
	//	//}
	//	//switch (type) {
	//	//case PotentialType::DampedHarmonicOscillator:
	//	//	return -parameters.at(2) * velocity / (2.0l * mass); // - ( b / 2m ) * (vec3) rel_vel_projected onto axis
	//	//	break;
	//	//case PotentialType::PlanetaryGravitationalPotential:
	//	//	return vec3(0);
	//	//	break;
	//	//	//case PotentialType::LogarithmForce:
	//	//	//	return vec3(0);
	//	//	//	break;
	//	//default: // Type::Default
	//	//	return vec3(0);
	//	//	break;
	//	//}

	//	vec3 positionGradient = vec3(0);
	//	vec3 velocityGradient = vec3(0);

	//	switch (type) {
	//	case PotentialType::DampedHarmonicOscillator:
	//		positionGradient = parameters.at(1) * displacement.normalized() * (displacement.magnitude() - parameters.at(0));
	//		velocity = velocity.dot(displacement.normalized()); // get axis-relative velocity
	//		velocityGradient = -parameters.at(2) * velocity / 2.0l;
	//		break;
	//	}
	//}

	//long double lagrangian(vec3 displacement, vec3 velocity, long double time, long double mass);
	long double potentialEnergy(vec3 displacement, vec3 velocity, long double time, long double mass);

	bool operator==(const Potential&) const;

private:
	std::vector<long double> parameters;
	enum PotentialType type;
};

class Rayleigh {
public:
	Rayleigh(enum RayleighType t = RayleighType::Default, std::vector<long double> param = {}) : type(t), parameters(param) {
		switch (type) {
		case RayleighType::VelocityDissipation:
			assert(parameters.size() == 1);
			break;
		default:
			assert(parameters.size() == 0);
			break;
		}
	}

	vec3 velocityGradientOverM(vec3 velocity, long double mass) {
		switch (type) {
		case RayleighType::VelocityDissipation:
			return parameters.at(0) * velocity;
			break;
		default:
			return vec3(0);
			break;
		}
	}

	long double rayleigh(vec3 displacement, vec3 velocity) {
		switch (type) {
		case RayleighType::VelocityDissipation:
			//  R = 0.5 * b * v^2
			return parameters.at(0) * velocity.dot(velocity);
			break;
		default:
			return 0.0l;
			break;
		}
	}

	bool operator==(const Rayleigh&) const = default;

private:
	std::vector<long double> parameters;
	enum RayleighType type;
};

class Interaction {
public:
	Interaction(Particle& p1, Particle& p2, Potential V, Rayleigh R);
	//Interaction(Particle& p1, Particle& p2, Potential Vptr) : parts({ p1, p2 }), V(new Potential(Vptr)) {}

	virtual void recordDisplacement();
	virtual void recordRelativeVelocity();

	virtual void apply();

	virtual bool inPair(Particle& p1);

	virtual Potential getPotential();
	virtual bool hasPotential(Potential otherV);

	virtual Rayleigh getRayleigh() {
		return R;
	}
	virtual bool hasRayleigh(const Rayleigh& otherR) {
		return R == otherR;
	}

	virtual bool isInteraction(Interaction i);
	virtual bool isInteraction(Potential V, Rayleigh R, Particle& p1, Particle& p2);

	virtual long double potentialEnergy() {
		long double Vtot = 0.0l;

		// IT IS A PROBLEM THAT THIS PASSES IN VALUES FOR ONLY ONE PARTICLE
		// SOMETHING IS WRONG WITH MY ASSUMPTIONS
		// A GOOGLE SEARCH REVEALS THAT V MUST ONLY DEPEND ON "CONFIGURATION" IE POSITION (and mass)
		// FOR NOW NO POTENTIALS DEPEND ON TIME OR VELOCITY AND NONE WILL BE ALLOWED GOING FORWARD
		// POTENTIAL CLASS WILL BE REFACTORED AT A LATER TIME
		Vtot += V.potentialEnergy(displacement, relVelocity.first, first().getTime(), first().getMass());
		//Vtot += 0.5l * V.potentialEnergy(displacement, relVelocity.second, second().getTime(), second().getMass());

		return Vtot;
	}

	virtual long double deltaWorkNC() {
		// dW = 2 * R * dt
		long double dW = R.rayleigh(displacement, relVelocity.first) * first().getDt();
		dW += R.rayleigh(displacement, relVelocity.second) * second().getDt();

		return dW;
	}

	virtual long double rayleigh() {
		long double Rtot = R.rayleigh(displacement, relVelocity.first);
		Rtot += R.rayleigh(displacement, relVelocity.second);

		return Rtot;
	}
private:
	std::pair<std::reference_wrapper<Particle>, std::reference_wrapper<Particle>> parts;
	Potential V;
	Rayleigh R;

	// to be extra clear: we don't store the displacement and rel_velocity because we have no where else to put it
	// (the particle references continue to hold these values [or equivalents] in real-time)
	// instead the point is to capture these values *before* we start changing them, so that update order does not matter
	// DO NOT: update the value of a particle then use that new value to update another particle  <- THIS IS BAD
	// DO: store copies of relavant values and utilize those copies to update the original. <- THIS IS BETTER
	vec3 displacement = vec3(0);
	std::pair<vec3, vec3> relVelocity = std::make_pair<vec3, vec3>(vec3(0.0l), vec3(0.0l)) ;

	Particle& first();
	Particle& second();
};

class UniversalInteraction {
public:
	UniversalInteraction(std::vector<std::reference_wrapper<Particle>> ps, Potential V, Rayleigh R);
	//UniversalInteraction(std::vector<std::reference_wrapper<Particle>> ps, Potential Vptr) : parts(ps), V(new Potential(Vptr)) {}
	//UniversalInteraction(std::vector<std::unique_ptr<Particle>> ps, Potential V) : parts(std::ref(*ps.back())), V(V) {}

	virtual void apply();
	//virtual void apply(Particle& part, long double dt) {
	//	vec3 displacement = part.getPosition();

	//	vec3 grad = V->gradientOverM(displacement, part.getVelocity(), part.getTime(), part.getMass());
	//	vec3 dv = -part.getDt() * grad /*/ part.getMass()*/;

	//	part.accelerateBy(dv);
	//}

	virtual bool isPart(Particle& p);

	virtual void addPart(Particle& p);
	virtual void replaceParts(std::vector<std::reference_wrapper<Particle>> parts);
	virtual void addParts(std::vector<std::reference_wrapper<Particle>> parts);
	virtual void clearParts();

	virtual Potential getPotential();
	virtual bool hasPotential(Potential otherV);

	virtual Rayleigh getRayleigh() {
		return R;
	}
	virtual bool hasRayleigh(const Rayleigh& otherR) {
		return R == otherR;
	}

	virtual bool isInteractionOf(Particle& pPrime);

	virtual long double potentialEnergySum() {
		long double Vtot = 0.0l;

		for (Particle p : parts) {
			Vtot += V.potentialEnergy(p.getPosition(), p.getVelocity(), p.getTime(), p.getMass());
		}

		return Vtot;
	}

	virtual long double deltaWorkNCSum() {
		long double dWtot = 0.0l;

		for (Particle p : parts) {
			dWtot = R.rayleigh(p.getPosition(), p.getVelocity()) * p.getDt();
		}

		return dWtot;
	}

	virtual long double rayleighSum() {
		long double Rtot = 0.0l;

		for (Particle p : parts) {
			Rtot += R.rayleigh(p.getPosition(), p.getVelocity());
		}

		return Rtot;
	}
private:
	std::vector<std::reference_wrapper<Particle>> parts;
	Potential V;
	Rayleigh R;
};