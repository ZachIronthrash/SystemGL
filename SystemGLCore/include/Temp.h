#pragma once
#include "Particle.h"
//#include "Interaction2.h"
#include "SystemGLMath.h"
#include <vector>
#include <type_traits>
#include <map>
#include <set>
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
		default:
			return 0;
			break;
		}
	}

	bool operator==(const Potential&) const = default;

private:
	std::vector<long double> parameters;
	enum PotentialType type;
};

struct Interaction {
	Potential pot;
	vec3 displacement = vec3(0.0l);
	vec3 relVel = vec3(0.0l);
	//vec3 relVel2 = vec3(0.0l);

	Interaction(Potential pot) : pot(pot) {}
};

struct InteractionCmp {
	bool operator()(const Interaction& a, const Interaction& b) const noexcept {
		return std::addressof(a) < std::addressof(b);
	}
};

enum class BoundaryConditions {
	reflection,
	absorption
};

class System {
public:
	System(BoundaryConditions bc) : bc(bc) {}

	void addNode(Particle& p) {
		graph.emplace(std::ref(p), std::map<std::reference_wrapper<Particle>, std::set<Interaction, InteractionCmp>, RefWrapperCmp>{});
	}

	size_t nodeCount() {
		return graph.size();
	}

	void createInteraction(Particle& p1, Particle& p2, Potential pot) {
		//auto outerPair1 = graph.find(std::ref(p1));
		//auto outerPair2 = graph.find(std::ref(p2));

		//auto innerPair1 = outerPair1->second.find(std::ref(p2));
		//auto innerPair2 = outerPair2->second.find(std::ref(p1));

		//if (innerPair1 == outerPair1->second.end()) {
		//	outerPair1->second.insert({ std::ref(p2), { Interaction(pot) } });
		//}
		//else {
		//	auto interactionSet = innerPair1->second;
		//	
		//	interactionSet.insert(Interaction(pot));
		//}

		//if (innerPair2 == outerPair2->second.end()) {
		//	outerPair2->second.insert({ std::ref(p1), { Interaction(pot) } });
		//}
		//else {
		//	auto interactionSet = innerPair2->second;

		//	interactionSet.insert(Interaction(pot));
		//}

		// MAKE SURE TO ADD AN ASSERTION THAT EACH PARTICLE MUST ALREADY BE IN THE SYSTEM
		// SINCE EACH INTERACTION IS CREATED ONCE THIS CHECK WON'T ADD TOO MUCH RUNTIME

		// ensure node for p1
		auto res1 = graph.emplace(std::ref(p1), std::map<std::reference_wrapper<Particle>, std::set<Interaction, InteractionCmp>, RefWrapperCmp>{});
		auto outerPair1 = res1.first;

		// ensure node for p2
		auto res2 = graph.emplace(std::ref(p2), std::map<std::reference_wrapper<Particle>, std::set<Interaction, InteractionCmp>, RefWrapperCmp>{});
		auto outerPair2 = res2.first;

		// find or create inner entries and emplace interaction (no copies of sets)
		auto& map1 = outerPair1->second;
		auto& map2 = outerPair2->second;

		auto inner1 = map1.emplace(std::ref(p2), std::set<Interaction, InteractionCmp>{}).first;
		inner1->second.emplace(pot);

		auto inner2 = map2.emplace(std::ref(p1), std::set<Interaction, InteractionCmp>{}).first;
		inner2->second.emplace(pot);
	}

	void pushVelocity() {
		recordState();

		for (auto& outerPair : graph) {
			Particle& current = outerPair.first.get();

			vec3 dv = vec3(0.0l);

			// "double counting" the interactions makes this portion a lot simpler
			for (auto& innerPair : outerPair.second) {
				for (Interaction i : innerPair.second) {
					Potential V = i.pot;
					vec3 disp = i.displacement;
					vec3 relVel = i.relVel;

					dv += V.velocityGradientOverM(relVel, current.getMass()) - current.getDt() * V.positionGradientOverM(disp, current.getMass());
				}
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
				for (Interaction i : innerPair.second) {
					Potential V = i.pot;
					vec3 disp = i.displacement;
					vec3 relVel = i.relVel;

					PE += V.potentialEnergy(disp, relVel, current.getTime(), current.getDt());
				}
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
	/*
	* * ABSTRACTION:
	*	Systems are modelled as an undirected graph where particles form the nodes, and interactions the edges.
	*		- Interactions must consist of nodes (particles) which are already inside the system
	*		- Note: nothing is stopping the user from creating multiple systems out of the same set of particles, however:
	*			- currently the time tracking mechanism will always iterate the time of each particle regardless of if it has already been updated
	*			- calculations involving energy will still be in the lab (render/camera) frame (and not a subsystem frame, ie: a spinning one)
	* CORRESPONDENCE:
	*	graph.key represents the nodes in the graph and must be unique
	*	graph.value represents the edges of the graph:
	*		- abstracted as a map of end-nodes corresponding to a set of interactions
	*			- the end nodes indicate the edge in consideration
	*			- the set of interactions store each force/potential along that edge
	*		- insertions for interactions must add each pair, ie: add { p1, { ..., { p2, i } } } & { p2, { ..., { p1, i } } }
	*			- the above design decision makes pushing velocity very easy as each particle has its own reference to its 
	*				corresponding interaction
	*			- in contract, potential energy is harder, since a naive iteration will double-count each interaction
	*	TODO:
	*		- universal interactions
	*/
    std::map<std::reference_wrapper<Particle>, std::map<std::reference_wrapper<Particle>, std::set<Interaction, InteractionCmp>, RefWrapperCmp>, RefWrapperCmp> graph;

	enum BoundaryConditions bc;

	int iterationCount = 0;

	void recordState() {
		for (auto& pair : graph) {
			for (auto& subPair : pair.second) {
				// **** AI ****
				// ... but I think I mostly understand it

				// this call pulls an iterator for our set of interactions
				std::set<Interaction, InteractionCmp>::iterator it = subPair.second.begin();

				// we use the iterator to update each entry until we reach the end
				while (it != subPair.second.end()) {
					// we extract the current node and capture the iterator as we increment this
					// (if we end the loop with it++ or put it anywhere else, this will throw a runtime error)
					auto node = subPair.second.extract(it++);

					// then we pull out and update our values
					Particle& first = pair.first.get();
				  	Particle& second = subPair.first.get();

					Interaction& i = node.value();

					i.displacement = first.getPosition() - second.getPosition(); // from other to current
					vec3 cmVelocity = (first.getMass() * first.getVelocity() + second.getMass() * second.getVelocity()) / (first.getMass() + second.getMass());
					i.relVel = first.getVelocity() - cmVelocity; // only record one since we are already storing both directions of the edge
					// if the above comment does not make sense, consider why when insertting an interaction between particle A and B
					// we insert the corresponding particle-potential pair to both A *and* B
					//i.relVel2 = second.getVelocity() - cmVelocity;

					// and re-insert the node
					// I'm pretty sure emplace would mess up the iterator somehow but maybe not
					subPair.second.insert(std::move(node));
				}
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