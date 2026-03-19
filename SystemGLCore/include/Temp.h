#pragma once
#include "Particle.h"
//#include "Interaction2.h"
#include "SystemGLMath.h"
#include <vector>
#include <type_traits>
#include <map>
#include <functional>
#include <cassert>

// comparator for std::reference_wrapper<Particle> so ordered maps can use
// the referenced object address as the ordering key (stable and fast).
struct RefWrapperCmp {
    bool operator()(const std::reference_wrapper<Particle>& a, const std::reference_wrapper<Particle>& b) const noexcept {
        return std::addressof(a.get()) < std::addressof(b.get());
    }
};

enum class PotentialType {
	Default, // non-damped harmonic oscillater w/ equilibrium = vec3(0)
	//DampedHarmonicOscillator,
	SimpleHarmonicOscillator,
	PlanetaryGravitationalPotential, // constant acceleration field
	LogarithmForce
};

class Potential {
public:
	Potential(enum PotentialType t = PotentialType::Default, std::vector<long double> param = {}) : type(t), parameters(param) {
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

	vec3 positionGradientOverM(vec3 displacement, long double mass) {
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
	vec3 velocityGradientOverM(vec3 velocity, long double mass) {
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

	long double potentialEnergy(vec3 displacement, vec3 velocity, long double time, long double mass) {
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
			}
			else if (displacement != vec3(0)) {
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

	bool operator==(const Potential&) const = default;

private:
	std::vector<long double> parameters;
	enum PotentialType type;
};

struct interaction {
	Potential pot;
	vec3 displacement = vec3(0.0l);
	vec3 relVel = vec3(0.0l);
	//vec3 relVel2 = vec3(0.0l);

	interaction(Potential pot) : pot(pot) {}
};

enum class BoundaryConditions {
	reflection,
	absorption
};

class System {
public:
	System(BoundaryConditions bc) : bc(bc) {}

	void addNode(Particle& p) {
		graph.insert({ std::ref(p), {} });
	}

	size_t nodeCount() {
		return graph.size();
	}

	void createInteraction(Particle& p1, Particle& p2, Potential pot) {
		auto pair1 = graph.find(std::ref(p1));
		auto pair2 = graph.find(std::ref(p2));

		pair1->second.insert({ std::ref(p2), interaction(pot) });
		pair2->second.insert({ std::ref(p1), interaction(pot) });
	}

	void pushVelocity() {
		recordState();

		for (auto& outerPair : graph) {
			Particle& current = outerPair.first.get();

			vec3 dv = vec3(0.0l);

			// "double counting" the interactions makes this portion a lot simpler
			for (auto& innerPair : outerPair.second) {
				Potential V = innerPair.second.pot;
				vec3 disp = innerPair.second.displacement;
				vec3 relVel = innerPair.second.relVel;

				dv += V.velocityGradientOverM(relVel, current.getMass()) - current.getDt() * V.positionGradientOverM(disp, current.getMass());
			}

			current.accelerateBy(dv);
		}
	}

	int pushPosition() {
		for (auto& outerPair : graph) {
			Particle& current = outerPair.first.get();

			current.translateBy(current.getVelocity() * current.getDt());

			current.pushTime();
		}

		return ++iterationCount;
	}

	long double kineticEnergy() {
		long double KE = 0.0l;

		for (auto& outerPair : graph) {
			Particle& current = outerPair.first.get();

			KE += current.calcKE();
		}

		return KE;
	}

	long double potentialEnergy() {
		recordState();

		long double PE = 0.0l;

		// THIS DOUBLE COUNTS AS IT IS RN
		// FIX IT!!!!
		//	(since { p1, { ..., { p2, pot } } & { p2 , { ..., { p1, pot } } are both in the graph)
		// -> this isn't a problem for the other functions b/c of symmetry or because we only retrieve the keys
		for (auto& outerPair : graph) {
			Particle& current = outerPair.first.get();

			for (auto& innerPair : outerPair.second) {
				Potential V = innerPair.second.pot;
				vec3 disp = innerPair.second.displacement;
				vec3 relVel = innerPair.second.relVel;

				PE += V.potentialEnergy(disp, relVel, current.getTime(), current.getDt());
			}
		}

		return PE;
	}

	std::vector<std::reference_wrapper<Particle>> particles() {
		std::vector<std::reference_wrapper<Particle>> parts;

		for (auto& pair : graph) {
			parts.push_back(pair.first.get());
		}

		return parts;
	}

private:
    std::map<std::reference_wrapper<Particle>, std::map<std::reference_wrapper<Particle>, interaction, RefWrapperCmp>, RefWrapperCmp> graph;

	enum BoundaryConditions bc;

	int iterationCount = 0;

	void recordState() {
		for (auto& pair : graph) {
			for (auto& subPair : pair.second) {
				Particle& first = pair.first.get();
				Particle& second = subPair.first.get();
				interaction& i = subPair.second;
				i.displacement = first.getPosition() - second.getPosition(); // from other to current
				vec3 cmVelocity = (first.getMass() * first.getVelocity() + second.getMass() * second.getVelocity()) / (first.getMass() + second.getMass());
				i.relVel = first.getVelocity() - cmVelocity; // only record one since we are already storing both directions of the edge
				// if the above comment does not make sense, consider why when insertting an interaction between particle A and B
				// we insert the corresponding particle-potential pair to both A *and* B
				//i.relVel2 = second.getVelocity() - cmVelocity;
			}
		}
	}
};

//struct ParticleInteractionGraph {
//	std::map<std::reference_wrapper<Particle>, std::map<std::reference_wrapper<Particle>, Potential>> graph;
//
//	void addNode(Particle& p) {
//		graph.insert(p, {});
//	}
//
//	void createInteraction(Particle& p1, Particle& p2, Potential pot) {
//		auto pair1 = graph.find(p1);
//		auto pair2 = graph.find(p2);
//
//		pair1->second.insert({ p2, pot});
//		pair2->second.insert({ p1, pot });
//	}
//
//private:
//	std::map<std::reference_wrapper<Particle>, Potential>& interactionsOf(Particle& p) {
//		return graph.find(p)->second;
//	}
//};

/*
* notes (to be turned into contract):
*	- time is inherent to particles, only tracks iterations (at least for now)
*	- using references instead of pointers whenever possible
*	- boundary conditions should be built in
*	- std::vector for parts should be replaced with a more efficient searchable structure (such as bst)
*/
//class System {
//public:
//	System(BoundaryConditions bc) : bc(bc) {}
//
//	void addParticle(Particle& p) {
//		assert(particleInteractionMap.find(p) == particleInteractionMap.end());
//	}
//
//private:
//	std::map<std::reference_wrapper<Particle>, std::set<std::reference_wrapper<Particle>, Potential>> particleInteractionMap;
//
//	enum BoundaryConditions bc;
//
//	//template <typename T>
//	//bool isSubsetOf(std::vector<T> set, std::vector<T> subset) {
//	//	for (T current : subset) {
//	//		bool currentInSet = false;
//	//		for (T element : set) {
//	//			if (element == current) {
//	//				currentInSet = true;
//	//			}
//	//		}
//
//	//		if (currentInSet == false) {
//	//			return false;
//	//		}
//	//	}
//
//	//	return true;
//	//}
//};