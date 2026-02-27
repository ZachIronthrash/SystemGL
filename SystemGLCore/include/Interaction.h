#pragma once
#include "Particle.h"
#include "SystemGLMath.h"
#include <iostream>
#include <memory>
#include <ostream>
#include <type_traits>
#include <utility>
#include <vector>

class Potential {
public:
	Potential() {}
	Potential(long double s) : strength(s) {}
	virtual ~Potential() = default;

	virtual vec3 positionGradientOverM(vec3 displacement, long double mass) {
		std::cout << "NOT WORkING" << std::endl;
		return strength * displacement / mass; // from PE = 0.5 * k * dr^2
	}
	virtual vec3 velocityGradientOverM(vec3 deltaV, long double mass) {
		std::cout << "YOUR FEARS WILL BE REALISED" << std::endl;
		return strength * deltaV / mass;
	}

	virtual bool operator==(const Potential&) const = default;

private:
	long double strength = 1.0l;
};

class DampedHarmonicOscillator : public Potential {
public:
	DampedHarmonicOscillator(long double equilibriumDisplacement, long double k, long double b) : b(b), k(k), equilibriumDisplacement(equilibriumDisplacement) {}

	vec3 positionGradientOverM(vec3 displacement, long double mass) override {
		vec3 gradient = k * displacement.normalized() * (displacement.magnitude() - equilibriumDisplacement) / mass;
		//std::cout << "gradient = " << gradient << std::endl;
		return gradient;
		//std::cout << "gradient = " << k * displacement << std::endl;
		//return k * displacement;
	}
	vec3 velocityGradientOverM(vec3 deltaV, long double mass) override {
		vec3 gradient = -b * deltaV / (2.0l * mass); // using force = -( b / 2m ) * v
		/*if (gradient != vec3(0)) {
			std::cout << gradient << std::endl;
		}*/
		return gradient;
	}

private:
	long double equilibriumDisplacement = 0.0l;
	long double k = 1.0l; // spring constant
	long double b = 0.0l; // damping force?
	// DEFINITIONS:
	// [angular_frequency] = sqrt(k / m)
	// [gamma/damping_constant] = b / 2m
};

class PlanetaryGravitationalPotential : public Potential {
public:
	PlanetaryGravitationalPotential(vec3 g/*, vec3 p0*/) : g(g)/*, p0(p0)*/ {}

	vec3 positionGradientOverM(vec3 displacement, long double mass) override {
		//vec3 r = displacement - p0;
		/*vec3 gHat = g.normalized();
		long double rDotg = r.dot(g);
		return -rDotg * gHat;*/
		//std::cout << -g << std::endl;
		return /*g.dot(r);*/-g /** mass*/;

	}
	vec3 velocityGradientOverM(vec3 deltaV, long double mass) override {
		return vec3(0);
	}
private:
	vec3 g = vec3(0.0l);
	//vec3 p0 = vec3(0.0l);
};

class Interaction {
public:
	Interaction(Particle& p1, Particle& p2, std::shared_ptr<Potential> Vptr) : parts({ p1, p2 }), V(Vptr) {}
	//Interaction(Particle& p1, Particle& p2, Potential Vptr) : parts({ p1, p2 }), V(new Potential(Vptr)) {}

	virtual void recordDisplacement() {
		displacement = second().getPosition() - first().getPosition();
	}
	virtual void recordDeltaV() { // (in direction of displacement)
		vec3 displacementHat = (second().getPosition() - first().getPosition()).normalized();
		deltaV = displacementHat * (second().getVelocity() - first().getVelocity()).dot(displacementHat);
		//std::cout << deltaV << std::endl;
	}

	virtual void apply() {
		//vec3 displacement = second().getPosition() - first().getPosition();

		vec3 velGrad1 = V->velocityGradientOverM(-deltaV, first().getMass());
		vec3 velGrad2 = V->velocityGradientOverM(deltaV, second().getMass());

		/*if (velGrad1 != vec3(0)) {
			std::cout << velGrad1 << std::endl;
		}*/

		// replace 0's with particle.getTime()
		vec3 posGrad1 = V->positionGradientOverM(-displacement, first().getMass());
		vec3 posGrad2 = V->positionGradientOverM(displacement, second().getMass());

		vec3 dv1 = velGrad1 - first().getDt() * posGrad1 /*/ first().getMass()*/;
		vec3 dv2 = velGrad2 - second().getDt() * posGrad2 /*/ second().getMass()*/;

		first().accelerateBy(dv1);
		second().accelerateBy(dv2);
	}

	virtual bool inPair(Particle& p1) {
		return (&p1 == &first()) || (&p1 == &second());
	}

	virtual std::shared_ptr<Potential> getPotential() {
		return V;
	}
	virtual bool hasPotential(Potential otherV) {
		//if (!V || !otherV) return false;
		return *V == otherV;
	}

	virtual bool isInteraction(Interaction i) {
		return i.inPair(parts.first) && i.inPair(parts.second) && hasPotential(*i.getPotential());
	}
	virtual bool isInteraction(Potential V, Particle& p1, Particle& p2) {
		return inPair(p1) && inPair(p2) && hasPotential(V);
	}
private:
	std::pair<std::reference_wrapper<Particle>, std::reference_wrapper<Particle>> parts;
	std::shared_ptr<Potential> V;

	vec3 displacement = vec3(0);
	vec3 deltaV = vec3(0);

	Particle& first() {
		return parts.first.get();
	}
	Particle& second() {
		return parts.second.get();
	}
};

class UniversalInteraction {
public:
	UniversalInteraction(std::vector<std::reference_wrapper<Particle>> ps, std::shared_ptr<Potential> Vptr) : parts(ps), V(Vptr) {}
	//UniversalInteraction(std::vector<std::reference_wrapper<Particle>> ps, Potential Vptr) : parts(ps), V(new Potential(Vptr)) {}
	//UniversalInteraction(std::vector<std::unique_ptr<Particle>> ps, Potential V) : parts(std::ref(*ps.back())), V(V) {}

	virtual void apply() {
		//std::cout << "WORKING1" << std::endl;

		/*if (!V) { std::cerr << "Interaction::apply: V is null\n"; }
		else { std::cerr << "Interaction::apply: V type = " << typeid(*V).name() << std::endl; }*/

		for (Particle& part : parts) {
			vec3 displacement = part.getPosition();
			vec3 deltaV = part.getVelocity();

			vec3 velGrad = V->velocityGradientOverM(deltaV, part.getMass());
			vec3 posGrad = V->positionGradientOverM(displacement, part.getMass());
			vec3 dv = velGrad - part.getDt() * posGrad /*/ part.getMass()*/;

			part.accelerateBy(dv);
		}
	}
	//virtual void apply(Particle& part, long double dt) {
	//	vec3 displacement = part.getPosition();

	//	vec3 grad = V->gradientOverM(displacement, part.getVelocity(), part.getTime(), part.getMass());
	//	vec3 dv = -part.getDt() * grad /*/ part.getMass()*/;

	//	part.accelerateBy(dv);
	//}

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
	virtual void replaceParts(std::vector<std::reference_wrapper<Particle>> parts) {
		this->parts = parts;
	}
	virtual void addParts(std::vector<std::reference_wrapper<Particle>> parts) {
		//std::cout << "WORKING" << std::endl;
		for (Particle& p : parts) {
			this->parts.push_back(p);
			//std::cout << p.getPosition() << std::endl;
		}
	}
	virtual void clearParts() {
		parts.clear();
	}

	virtual std::shared_ptr<Potential> getPotential() {
		return V;
	}
	virtual bool hasPotential(Potential otherV) {
		//if (!V || !otherV) return false;
		return *V == otherV;
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
	std::shared_ptr<Potential> V;
};