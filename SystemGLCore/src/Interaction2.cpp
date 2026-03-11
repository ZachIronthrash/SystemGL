#include "pch.h"
#include "Interaction2.h"
#include <cassert>
#include <cmath>
#include <Particle.h>
#include <SystemGLMath.h>
#include <type_traits>
#include <vector>

Potential::Potential(enum PotentialType t, std::vector<long double> param) : type(t), parameters(param) {
	switch (t) {
	case PotentialType::DampedHarmonicOscillator:
		assert(parameters.size() == 3);
		// parameters = param; // equilibriumDisplacement, k, b
		break;
	case PotentialType::PlanetaryGravitationalPotential:
		assert(parameters.size() == 4);
		// parameters = param; // acceleration due to gravity (g), equilibrium displacement along g
		break;
		//case Type::LennardJonesPotential:
		//	assert(parameters.size() == 2); // epsilon (depth of potential well) & sigma (distance at which potential is zero)
		//	break;
	//case PotentialType::LogarithmForce:
	//	assert(parameters.size() == 2); // equilibriumDisplacement, strength
	//	assert(parameters.at(0) > 0);
	//	assert(parameters.at(1) > 0);
	//	break;
	default: // Type::Default
		assert(parameters.size() == 1);
		break;
	}
}

vec3 Potential::positionGradientOverM(vec3 displacement, long double mass) {
	switch (type) {
	case PotentialType::DampedHarmonicOscillator:
		return parameters.at(1) * displacement.normalized() * (displacement.magnitude() - parameters.at(0)) / mass; // k * (vec3) d * (|d| - equilibriumDisplacement)
		break;
	case PotentialType::PlanetaryGravitationalPotential:
		return -vec3(parameters.at(0), parameters.at(1), parameters.at(2)); // -g
		break;
	//case PotentialType::LogarithmForce: // I don't think this is conservative, but it might be fun to mess with
		//return parameters.at(1) * parameters.at(0) * displacement.normalized() * std::log(displacement.magnitude() / parameters.at(0)) / mass;
	default: // Type::Default
		return parameters.at(0) * displacement / mass;
		break;
	}
}
vec3 Potential::velocityGradientOverM(vec3 velocity, long double mass) {
	switch (type) {
	case PotentialType::DampedHarmonicOscillator:
		return -parameters.at(2) * velocity / (2.0l * mass); // - ( b / 2m ) * (vec3) deltaV
		break;
	case PotentialType::PlanetaryGravitationalPotential:
		return vec3(0);
		break;
	//case PotentialType::LogarithmForce:
	//	return vec3(0);
	//	break;
	default: // Type::Default
		return vec3(0);
		break;
	}
}

long double Potential::lagrangian(vec3 displacement, vec3 velocity, long double time, long double mass) {
	long double L = 0.0l;
	vec3 temp1 = vec3(0);
	vec3 temp2 = vec3(0);
	switch (type) {
	case PotentialType::DampedHarmonicOscillator:
		// L(x, x', t) = (1/2) * e^(bt/m) * (mx'^2 - kx^2) 
		// where x is displacement from equilibrium
		// and x' is velocity along displacement
		//temp1 = displacement.normalized();
		//if (temp1 == vec3(0.0l)) temp1 = velocity.normalized(); // if displacement is zero, then the direction the particle is heading in is the separation axis
		L = 0.5l * expl(parameters.at(2) * time / mass) 
			* (mass * velocity.dot(velocity) // interactions pass v_proj = v.dot(displacementHat)
			- parameters.at(1) * powl(displacement.magnitude() - parameters.at(0), 2));
		
		return L;
		break;
	case PotentialType::PlanetaryGravitationalPotential:
		// L (y', y) = (1/2) * m * y'^2 - m * g * y
		temp1 = { parameters.at(0), parameters.at(1), parameters.at(2) }; // acts as g
		temp2 = (temp1 != vec3(0.0l)) ? temp1.normalized() : displacement.normalized();
		L = 0.5l * mass * velocity.dot(velocity) - mass * temp1.dot(displacement - parameters.at(3) * temp2); // this last part is displacement - equilibrium_displacement * displacement_hat
		return L;
		break;
	}
}

bool Potential::operator==(const Potential&) const = default;

Interaction::Interaction(Particle& p1, Particle& p2, Potential V) : parts({ p1, p2 }), V(V) {}
//Interaction(Particle& p1, Particle& p2, Potential Vptr) : parts({ p1, p2 }), V(new Potential(Vptr)) {}

void Interaction::recordDisplacement() {
	displacement = second().getPosition() - first().getPosition();
}
void Interaction::recordRelativeVelocity() { // (in direction of displacement)
	//vec3 displacementHat = (second().getPosition() - first().getPosition()).normalized();
	//if (displacementHat == vec3(0.0l)) displacementHat = first().getVelocity().normalized(); // assume first particle and we flip when passing to second
	//deltaV = displacementHat * (second().getVelocity() - first().getVelocity()).dot(displacementHat);
	//std::cout << deltaV << std::endl;

	vec3 avg = 0.5l * (second().getVelocity() + first().getVelocity());
	relVelocity.first = first().getVelocity() - avg;
	relVelocity.second = second().getVelocity() - avg;
}

void Interaction::apply() {
	//vec3 displacement = second().getPosition() - first().getPosition();

	vec3 velGrad1 = V.velocityGradientOverM(relVelocity.first, first().getMass());
	vec3 velGrad2 = V.velocityGradientOverM(relVelocity.second, second().getMass());

	/*if (velGrad1 != vec3(0)) {
		std::cout << velGrad1 << std::endl;
	}*/

	// replace 0's with particle.getTime()
	vec3 posGrad1 = V.positionGradientOverM(-displacement, first().getMass());
	vec3 posGrad2 = V.positionGradientOverM(displacement, second().getMass());

	vec3 dv1 = velGrad1 - first().getDt() * posGrad1 /*/ first().getMass()*/;
	vec3 dv2 = velGrad2 - second().getDt() * posGrad2 /*/ second().getMass()*/;

	first().accelerateBy(dv1);
	second().accelerateBy(dv2);
}

bool Interaction::inPair(Particle& p1) {
	return (&p1 == &first()) || (&p1 == &second());
}

Potential Interaction::getPotential() {
	return V;
}
bool Interaction::hasPotential(Potential otherV) {
	//if (!V || !otherV) return false;
	return V == otherV;
}

bool Interaction::isInteraction(Interaction i) {
	return i.inPair(parts.first) && i.inPair(parts.second) && hasPotential(i.getPotential());
}
bool Interaction::isInteraction(Potential V, Particle& p1, Particle& p2) {
	return inPair(p1) && inPair(p2) && hasPotential(V);
}

Particle& Interaction::first() {
	return parts.first.get();
}
Particle& Interaction::second() {
	return parts.second.get();
}

UniversalInteraction::UniversalInteraction(std::vector<std::reference_wrapper<Particle>> ps, Potential V) : parts(ps), V(V) {}
//UniversalInteraction(std::vector<std::reference_wrapper<Particle>> ps, Potential Vptr) : parts(ps), V(new Potential(Vptr)) {}
//UniversalInteraction(std::vector<std::unique_ptr<Particle>> ps, Potential V) : parts(std::ref(*ps.back())), V(V) {}

void UniversalInteraction::apply() {
	//std::cout << "WORKING1" << std::endl;

	/*if (!V) { std::cerr << "Interaction::apply: V is null\n"; }
	else { std::cerr << "Interaction::apply: V type = " << typeid(*V).name() << std::endl; }*/

	for (Particle& part : parts) {
		vec3 displacement = part.getPosition();
		vec3 deltaV = part.getVelocity();

		vec3 velGrad = V.velocityGradientOverM(deltaV, part.getMass());
		vec3 posGrad = V.positionGradientOverM(displacement, part.getMass());
		vec3 dv = velGrad - part.getDt() * posGrad /*/ part.getMass()*/;

		part.accelerateBy(dv);
	}
}

bool UniversalInteraction::isPart(Particle& p) {
	for (Particle& part : parts) {
		if (&p == &part) {
			return true;
		}
	}
	return false;
}

void UniversalInteraction::addPart(Particle& p) {
	parts.push_back(p);
}
void UniversalInteraction::replaceParts(std::vector<std::reference_wrapper<Particle>> parts) {
	this->parts = parts;
}
void UniversalInteraction::addParts(std::vector<std::reference_wrapper<Particle>> parts) {
	//std::cout << "WORKING" << std::endl;
	for (Particle& p : parts) {
		this->parts.push_back(p);
		//std::cout << p.getPosition() << std::endl;
	}
}
void UniversalInteraction::clearParts() {
	parts.clear();
}

Potential UniversalInteraction::getPotential() {
	return V;
}
bool UniversalInteraction::hasPotential(Potential otherV) {
	//if (!V || !otherV) return false;
	return V == otherV;
}

bool UniversalInteraction::isInteractionOf(Particle& pPrime) {
	for (Particle& part : parts) {
		if (pPrime == part) {
			return true;
		}
	}
	return false;
}
