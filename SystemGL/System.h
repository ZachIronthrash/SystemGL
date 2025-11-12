#pragma once

#include <iostream>
#include <vector>
#include <string>

const double BIG_G = 1; //6.67430e-11f; <-- Gravitational constant is one for simplicity
const double PI = 3.14159265358979323846f;

namespace std {
	struct vec3 {
		vec3(double val = 0.0f) : x(val), y(val), z(val) {}
		vec3(double x, double y, double z) : x(x), y(y), z(z) {}

		vec3 operator+(const vec3& other) const {
			return vec3(x + other.x, y + other.y, z + other.z);
		}
		vec3 operator*(double scalar) const {
			return vec3(x * scalar, y * scalar, z * scalar);
		}
		vec3 operator-(const vec3& other) const {
			return vec3(x - other.x, y - other.y, z - other.z);
		}
		vec3 operator/(double scalar) const {
			return vec3(x / scalar, y / scalar, z / scalar);
		}
		vec3 operator-() const {
			return vec3(-x, -y, -z);
		}

		vec3& operator+=(const vec3& other) {
			x += other.x; y += other.y; z += other.z;
			return *this;
		}
		vec3& operator-=(const vec3& other) {
			x -= other.x; y -= other.y; z -= other.z;
			return *this;
		}
		vec3& operator*=(double scalar) {
			x *= scalar; y *= scalar; z *= scalar;
			return *this;
		}
		vec3& operator/=(double scalar) {
			x /= scalar; y /= scalar; z /= scalar;
			return *this;
		}

		vec3 normalized() const {
			double mag = sqrt(x * x + y * y + z * z);
			if (mag == 0) return vec3(0.0f);
			else return vec3(x / mag, y / mag, z / mag);
		}

		double x, y, z;
	};

	ostream& operator<<(ostream& os, const vec3& vec3) {
		os << vec3.x << ", " << vec3.y << ", " << vec3.z;
		return os;
	}

	class System {
	public:
		vector<System> subsystems;

		vec3 position = vec3(); vec3 velocity = vec3();
		double mass = 1.0f;
		double time = 0.0f;
		double deltaTime = 0.001f;

		System(vector<System> subsystems = {}) : subsystems(subsystems) {}
		System(vec3 position, vec3 velocity, double mass, double time = 0.0f, double deltaTime = 0.001f)
			: position(position), velocity(velocity), mass(mass), time(time), deltaTime(deltaTime) {
		}

		void evolve() {
			position += velocity * deltaTime;
			time += deltaTime;

			for (System& subsystem : subsystems) {
				subsystem.evolve();
			}
		}
	};

	class Interaction {
	public:
		System* systemA;
		System* systemB;
		char type;
		Interaction(System* systemA, System* systemB, char type) : systemA(systemA), systemB(systemB), type(type) {}

		void apply() {
			if (type == 'G') {
				vec3 aR = systemB->position - systemA->position;
				double rSquared = aR.x * aR.x + aR.y * aR.y + aR.z * aR.z;
				double forceMag = (BIG_G * systemA->mass * systemB->mass) / rSquared;
				vec3 forceA = aR.normalized() * forceMag;
				systemA->velocity += forceA * (1.0f / systemA->mass) * systemA->deltaTime;
				systemB->velocity -= forceA * (1.0f / systemB->mass) * systemB->deltaTime;
			}
		}
	};
}