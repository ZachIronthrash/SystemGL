#pragma once
#include "Particle.h"
#include "SystemGLMath.h"
#include <functional>
#include <utility>
#include <vector>

class Potential {
public:
	Potential() {}

	virtual vec3 gradient(vec3 displacement, vec3 velocity, long double time) {
		return -strength * displacement;
	}

	virtual bool operator==(const Potential&) const = default;

private:
	long double strength = 1.0l;
};

class ElasticPotential : public Potential {
public:
	ElasticPotential(long double k, vec3 equilibriumDisplacement) : k(k), equilibriumDisplacement(equilibriumDisplacement) {}

	// THIS IS WRONG EQUILIBRIUM SHOULD BE A SCALAR
	vec3 gradient(vec3 displacement, vec3 velocity, long double time) override {
		return -k * (displacement - equilibriumDisplacement);
	}

private:
	vec3 equilibriumDisplacement = vec3(0.0l);
	long double k = 1.0l;
};

class PlanetaryGravitationalPotential : public Potential {
public:
	PlanetaryGravitationalPotential(vec3 g, vec3 p0) : g(g), p0(p0) {}

	vec3 gradient(vec3 displacement, vec3 velocity, long double time) override {
		vec3 r = displacement - p0;
		vec3 rHat = r.normalized();
		long double rDotg = r.dot(g);
		return -rDotg * rHat;
	}
private:
	vec3 g = vec3(0.0l);
	vec3 p0 = vec3(0.0l);
};

class Interaction {
public:
	Interaction(Particle& p1, Particle& p2, Potential V) : parts({ p1, p2 }), V(V) {}

	virtual void apply() {
		vec3 displacement = second().getPosition() - first().getPosition();

		// replace 0's with particle.getTime()
		vec3 grad1 = V.gradient(-displacement, first().getVelocity(), first().getTime());
		vec3 grad2 = V.gradient(displacement, second().getVelocity(), second().getTime());

		vec3 dv1 = -first().getDt() * grad1 / first().getMass();
		vec3 dv2 = -second().getDt() * grad2 / second().getMass();

		first().accelerateBy(dv1);
		second().accelerateBy(dv2);
	}

	virtual bool inPair(Particle& p1) {
		return (&p1 == &first()) || (&p1 == &second());
	}

	virtual Potential getPotential() {
		return V;
	}
	virtual bool hasPotential(Potential V) {
		return this->V == V;
	}

	virtual bool isInteraction(Interaction i) {
		return i.inPair(parts.first) && i.inPair(parts.second) && hasPotential(i.getPotential());
	}
	virtual bool isInteraction(Potential V, Particle& p1, Particle& p2) {
		return inPair(p1) && inPair(p2) && hasPotential(V);
	}
private:
	std::pair<std::reference_wrapper<Particle>, std::reference_wrapper<Particle>> parts;
	Potential V;

	Particle& first() {
		return parts.first.get();
	}
	Particle& second() {
		return parts.second.get();
	}
};

class UniversalInteraction {
public:
	UniversalInteraction(std::vector<std::reference_wrapper<Particle>> ps, Potential V) : parts(ps), V(V) {}
	//UniversalInteraction(std::vector<std::unique_ptr<Particle>> ps, Potential V) : parts(std::ref(*ps.back())), V(V) {}

	virtual void apply() {
		for (Particle& part : parts) {
			vec3 displacement = part.getPosition();

			vec3 grad = V.gradient(displacement, part.getVelocity(), part.getTime());
			vec3 dv = -part.getDt() * grad / part.getMass();

			part.accelerateBy(dv);
		}
	}

	virtual bool isPart(Particle& p) {
		for (Particle& part : parts) {
			if (&p == &part) {
				return true;
			}
		}
		return false;
	}

	virtual void addPart(Particle& p) {
		parts.push_back(p);
	}

	virtual Potential getPotential() {
		return V;
	}
	virtual bool hasPotential(Potential V) {
		return this->V == V;
	}

	virtual bool isInteractionOf(Particle& pPrime) {
		for (Particle& part : parts) {
			if (pPrime == part) {
				return true;
			}
		}
		return false;
	}
private:
	std::vector<std::reference_wrapper<Particle>> parts;
	Potential V;
};