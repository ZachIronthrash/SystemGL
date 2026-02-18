#pragma once

#include <ostream>

/*
* constants
*/
// const long double BIG_G = 1; //6.67430e-11; <-- Gravitational constant is one for simplicity
const long double BOLTZMANN = 1.380649e-23l;
const long double PI = 3.14159265358979323846l;
const long double MOLE = 6.02214076e23;
const long double R_GAS = 8.314462618l; // universal: other one will be added when needed

/*
* A simple 3D vector class for basic vector operations
* Technically obselete due to glm::vec3, but used to learn about structs and operator overloading
*/
struct vec3 {
	long double x, y, z;

	/*
	* default constructor
	*
	* @ensures this.x = 0, this.y = 0, this.z = 0
	*/
	vec3();
	/*
	* constant constructor
	*
	* @param val - initial value to set all components to
	*
	* @ensures this.x = val, this.y = val, this.z = val
	*/
	vec3(long double val);
	/*
	* component constructor
	*
	* @param x - initial x component
	* @param y - initial y component
	* @param z - initial z component
	*
	* @ensures this.x = x, this.y = y, this.z = z
	*/
	vec3(long double x, long double y, long double z);

	/*
	* reverse the direction of the vector
	*
	* @returns a new vec3 s.t. result = -this
	*/
	vec3 operator-() const;

	/*
	* add other to this and return this
	*
	* @param other - vec3 to add
	*
	* @ensures this = #this + other
	* @returns this
	*/
	vec3& operator+=(const vec3& other);
	/*
	* substract other from this and return this
	*
	* @param other - vec3 to subtract
	*
	* @ensures this = #this - other
	* @returns this
	*/
	vec3& operator-=(const vec3& other);
	/*
	* multiply this by scalar and return this
	*
	* @param scalar - scalar to multiply by
	*
	* @ensures this = #this * scalar
	* @returns this
	*/
	vec3& operator*=(long double scalar);
	/*
	* divide this by scalar and return this
	*
	* @param scalar - scalar to divide by
	*
	* @requires scalar != 0
	*
	* @ensures this = #this / scalar
	* @returns this
	*/
	vec3& operator/=(long double scalar);

	/*
	* scales vector to unit length and returns the result
	*
	* @restores this
	*
	* @ensures result has length 1, unless this has length 0, in which case result is vec3(0)
	* @returns normalized vector
	*/
	vec3 normalized() const;

	long double magnitude() const;

	long double dot(vec3 avec) const { return x * avec.x + y * avec.y + z * avec.z; };

	vec3 cross(vec3 avec) const { return vec3(y*avec.z - z*avec.y, z*avec.x - x*avec.z, x*avec.y - y*avec.x); }

	long double volume() const { return x * y * z; };

	/*
	* helper function for printing vec3's
	* outputs in the format "x, y, z"
	* does not add an endl
	*
	* @param os - output stream to write to
	* @param vec - vec3 to output
	*
	* @ensures os contains the string representation of vec
	* @returns os
	*/
	friend std::ostream& operator<<(std::ostream& os, vec3 vec) {
		os << vec.x << ", " << vec.y << ", " << vec.z;
		return os;
	}

	friend bool operator==(const vec3& a, const vec3& b) {
		return (a.x == b.x && a.y == b.y && a.z == b.z);
	}
};

/*
* vec3 operator overloads for basic vector arithmetic
*/

/*
* adds two vec3's component-wise
*
* @param a - first vec3
* @param b - second vec3
*
* @ensures result = a + b
* @returns result
*/
vec3 operator+(vec3 a, vec3 b);
/*
* subtracts two vec3's component-wise
*
* @param a - first vec3
* @param b - second vec3
*
* @ensures result = a - b
* @returns result
*/
vec3 operator-(vec3 a, vec3 b);
/*
* multiplies vec3 by scalar component-wise
* this method handles the vec3 * constant case
*
* @param lhs - vec3
* @param rhs - scalar
*
* @ensures result = lhs * rhs
* @returns result
*/
vec3 operator*(vec3 lhs, long double rhs);
/*
* multiplies vec3 by scalar component-wise
* this method handles the constant * vec3 case
*
* @param lhs - scalar
* @param rhs - vec3
*
* @ensures result = lhs * rhs
* @returns result
*/
vec3 operator*(long double lhs, vec3 rhs);
/*
* divides vec3 by scalar component-wise
* this method handles the vec3 / constant case
*
* @param lhs - vec3
* @param rhs - scalar
*
* @requires rhs != 0
*
* @ensures result = lhs / rhs
* @returns result
*/
vec3 operator/(vec3 lhs, long double rhs);
/*
* divides vec3 by scalar component-wise
* this method handles the constant / vec3 case
*
* @param lhs - scalar
* @param rhs - vec3
*
* @requires rhs.x, rhs.y, rhs.z != 0
*
* @ensures result = lhs / rhs
* @returns result
*/
vec3 operator/(long double lhs, vec3 rhs);

