#include "pch.h"
#include "SystemGLMath.h"

vec3::vec3() : x(0), y(0), z(0) {};
vec3::vec3(long double val) : x(val), y(val), z(val) {}
vec3::vec3(long double x, long double y, long double z) : x(x), y(y), z(z) {}

vec3 vec3::operator-() const {
	return vec3(-x, -y, -z);
}

vec3& vec3::operator+=(const vec3& other) {
	x += other.x; y += other.y; z += other.z;
	return *this;
}
vec3& vec3::operator-=(const vec3& other) {
	x -= other.x; y -= other.y; z -= other.z;
	return *this;
}
vec3& vec3::operator*=(long double scalar) {
	x *= scalar; y *= scalar; z *= scalar;
	return *this;
}
vec3& vec3::operator/=(long double scalar) {
	assert(scalar != 0/*, "division by zero"*/);

	x /= scalar; y /= scalar; z /= scalar;
	return *this;
}

vec3 vec3::normalized() const {
	long double mag = sqrt(x * x + y * y + z * z);
	if (mag == 0) return vec3(0.0f);
	else return vec3(x / mag, y / mag, z / mag);
}

long double vec3::magnitude() const {
	long double mag = sqrt(x * x + y * y + z * z);
	return mag;
}

vec3 operator+(vec3 a, vec3 b) {
	return vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}
vec3 operator-(vec3 a, vec3 b) {
	return vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}
vec3 operator*(vec3 lhs, long double rhs) {
	return vec3(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs);
}
vec3 operator*(long double lhs, vec3 rhs) {
	return vec3(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z);
}
vec3 operator/(vec3 lhs, long double rhs) {
	assert(rhs != 0/*, "division by zero"*/);

	return vec3(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs);
}
vec3 operator/(long double lhs, vec3 rhs) {
	assert(rhs.x != 0 && rhs.y != 0 && rhs.z != 0/*, "division by zero"*/);

	return vec3(lhs / rhs.x, lhs / rhs.y, lhs / rhs.z);
}