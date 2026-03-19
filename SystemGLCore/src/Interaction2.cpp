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
	case PotentialType::SimpleHarmonicOscillator:
		assert(parameters.size() == 2 && parameters.at(0) >= 0.0l);
		// parameters = param; // equilibriumDisplacement, k
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
		assert(parameters.size() == 0);
		break;
	}
}

vec3 Potential::positionGradientOverM(vec3 displacement, long double mass) {
	switch (type) {
	case PotentialType::SimpleHarmonicOscillator:
		return parameters.at(1) * displacement.normalized() * (displacement.magnitude() - parameters.at(0)) / mass; // k * (vec3) d * (|d| - equilibriumDisplacement)
		break;
	case PotentialType::PlanetaryGravitationalPotential:
		return -vec3(parameters.at(0), parameters.at(1), parameters.at(2)); // -g
		break;
	//case PotentialType::LogarithmForce: // I don't think this is conservative, but it might be fun to mess with
		//return parameters.at(1) * parameters.at(0) * displacement.normalized() * std::log(displacement.magnitude() / parameters.at(0)) / mass;
	default: // Type::Default
		return vec3(0);
		break;
	}
}
vec3 Potential::velocityGradientOverM(vec3 velocity, long double mass) {
	switch (type) {
	case PotentialType::SimpleHarmonicOscillator:
		//return -parameters.at(2) * velocity / (2.0l * mass); // - ( b / 2m ) * (vec3) rel_velocity
		return vec3(0);
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

long double Potential::potentialEnergy(vec3 displacement, vec3 velocity, long double time, long double mass) {
	long double L = 0.0l;
	vec3 temp1 = vec3(0);
	vec3 temp2 = vec3(0);
	vec3 temp3 = vec3(0);
	switch (type) {
	case PotentialType::SimpleHarmonicOscillator:
		// L(x, x', t) = (1/2) * e^(bt/m) * (mx'^2 - kx^2) 
		//if (displacement != vec3(0)) {
		//	temp1 = displacement.normalized();
		//	temp2 = velocity.dot(temp1) * temp1; // need v along displacement axis
		//}
		//else {
		//	temp2 = velocity;
		//}
		L = 0.5l * parameters.at(1) * powl(displacement.magnitude() - parameters.at(0), 2);
		
		return L;
		break;
	case PotentialType::PlanetaryGravitationalPotential:
		// L (y', y) = (1/2) * m * y'^2 - m * g * y
		temp1 = { parameters.at(0), parameters.at(1), parameters.at(2) }; // acts as g
		//temp2 = (temp1 != vec3(0.0l)) ? temp1.normalized() : displacement.normalized();
		if (temp1 != vec3(0.0l)) {
			temp2 = temp1.normalized();
		} else if (displacement != vec3(0)) {
			temp2 = displacement.normalized();
		}
		else {
			temp2 = velocity.normalized();
		}
		//temp3 = velocity.dot(temp2) * temp2; // isolate velocity along relevant axis
		L = mass * temp1.dot(displacement - parameters.at(3) * temp2); // this last part is displacement - equilibrium_displacement * displacement_hat
		return L;
		break;
	}
}

bool Potential::operator==(const Potential&) const = default;

Interaction::Interaction(Particle& p1, Particle& p2, Potential V, Rayleigh R) : parts({ p1, p2 }), V(V), R(R) {
	recordDisplacement();
	recordRelativeVelocity();
}
//Interaction(Particle& p1, Particle& p2, Potential Vptr) : parts({ p1, p2 }), V(new Potential(Vptr)) {}

void Interaction::recordDisplacement() {
	displacement = second().getPosition() - first().getPosition();
}
void Interaction::recordRelativeVelocity() { // (in direction of displacement)
	//vec3 displacementHat = (second().getPosition() - first().getPosition()).normalized();
	//if (displacementHat == vec3(0.0l)) displacementHat = first().getVelocity().normalized(); // assume first particle and we flip when passing to second
	//deltaV = displacementHat * (second().getVelocity() - first().getVelocity()).dot(displacementHat);
	//std::cout << deltaV << std::endl;

	// first we find the cm velocity
	vec3 cm = 0.5l * (second().getMass() * second().getVelocity() + first().getMass() * first().getVelocity()) / (first().getMass() + second().getMass());
	// then we find the velocity relative to cm
	relVelocity.first = first().getVelocity() - cm;
	relVelocity.second = second().getVelocity() - cm;
	// finally we isolate the components to only those along the displacement axis
	// (this might not be a general sol'n. For instance, the coriolis force can depend on motion perpendicular to the displacement axis)
	//relVelocity.first = relVelocity.first.dot(displacement.normalized()) * displacement.normalized();
	//relVelocity.second = relVelocity.second.dot(displacement.normalized()) * displacement.normalized();

}

void Interaction::apply() {
	//vec3 displacement = second().getPosition() - first().getPosition();

	// worth noting that only conservative potentials should be dependent on v; otherwise use a rayleigh
	vec3 velGrad1 = V.velocityGradientOverM(relVelocity.first, first().getMass());
	vec3 velGrad2 = V.velocityGradientOverM(relVelocity.second, second().getMass());

	/*if (velGrad1 != vec3(0)) {
		std::cout << velGrad1 << std::endl;
	}*/

	// replace 0's with particle.getTime()
	vec3 posGrad1 = V.positionGradientOverM(-displacement, first().getMass());
	vec3 posGrad2 = V.positionGradientOverM(displacement, second().getMass());

	vec3 rayleighGrad1 = R.velocityGradientOverM(relVelocity.first, first().getMass());
	vec3 rayleighGrad2 = R.velocityGradientOverM(relVelocity.second, second().getMass());

	//std::cout << rayleighGrad1 << std::endl;

	vec3 dv1 = velGrad1 - first().getDt() * ( posGrad1 + rayleighGrad1 )/*/ first().getMass()*/;
	vec3 dv2 = velGrad2 - second().getDt() * ( posGrad2 + rayleighGrad2 ) /*/ second().getMass()*/;

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
bool Interaction::isInteraction(Potential V, Rayleigh R, Particle& p1, Particle& p2) {
	return inPair(p1) && inPair(p2) && hasPotential(V) && hasRayleigh(R);
}

Particle& Interaction::first() {
	return parts.first.get();
}
Particle& Interaction::second() {
	return parts.second.get();
}

UniversalInteraction::UniversalInteraction(std::vector<std::reference_wrapper<Particle>> ps, Potential V, Rayleigh R) : parts(ps), V(V), R(R) {}
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
		vec3 rayleighGrad = R.velocityGradientOverM(deltaV, part.getMass());
		vec3 dv = velGrad - part.getDt() * ( posGrad + rayleighGrad) /*/ part.getMass()*/;

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
