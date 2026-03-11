#pragma once
#include "Particle.h"
#include "SystemGLMath.h"
#include <type_traits>
#include <utility>
#include <vector>

enum class PotentialType {
	Default, // non-damped harmonic oscillater w/ equilibrium = vec3(0)
	DampedHarmonicOscillator,
	PlanetaryGravitationalPotential, // constant acceleration field
	LogarithmForce
};

class Potential {
public:
	Potential(enum PotentialType t = PotentialType::Default, std::vector<long double> param = { 1.0l });

	vec3 positionGradientOverM(vec3 displacement, long double mass);
	vec3 velocityGradientOverM(vec3 velocity, long double mass);

	long double lagrangian(vec3 displacement, vec3 velocity, long double time, long double mass);

	bool operator==(const Potential&) const;

private:
	std::vector<long double> parameters;
	enum PotentialType type;
};

class Interaction {
public:
	Interaction(Particle& p1, Particle& p2, Potential V);
	//Interaction(Particle& p1, Particle& p2, Potential Vptr) : parts({ p1, p2 }), V(new Potential(Vptr)) {}

	virtual void recordDisplacement();
	virtual void recordRelativeVelocity();

	virtual void apply();

	virtual bool inPair(Particle& p1);

	virtual Potential getPotential();
	virtual bool hasPotential(Potential otherV);

	virtual bool isInteraction(Interaction i);
	virtual bool isInteraction(Potential V, Particle& p1, Particle& p2);

	virtual long double lagrangian() {
		long double L = 0.0l;

		L += V.lagrangian(displacement, relVelocity.first, first().getTime(), first().getMass());
		L += V.lagrangian(displacement, relVelocity.second, second().getTime(), second().getMass());

		return L;
	}
private:
	std::pair<std::reference_wrapper<Particle>, std::reference_wrapper<Particle>> parts;
	Potential V;

	vec3 displacement = vec3(0);
	std::pair<vec3, vec3> relVelocity = std::make_pair<vec3, vec3>(vec3(0.0l), vec3(0.0l)) ;

	Particle& first();
	Particle& second();
};

class UniversalInteraction {
public:
	UniversalInteraction(std::vector<std::reference_wrapper<Particle>> ps, Potential V);
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

	virtual bool isInteractionOf(Particle& pPrime);

	virtual long double lagrangianSum() {
		long double L = 0.0l;

		for (Particle p : parts) {
			L += V.lagrangian(p.getPosition(), p.getVelocity(), p.getTime(), p.getMass());
		}

		return L;
	}
private:
	std::vector<std::reference_wrapper<Particle>> parts;
	Potential V;
};