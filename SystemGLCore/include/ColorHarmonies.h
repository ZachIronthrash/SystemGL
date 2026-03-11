#pragma once
#include <vector>

/*
* A simple struct for storing 0 -> 1 rgb values as floats.
* 
* REPRESENTATION INVARIANT:
*	1. color = { red, green, blue }
*	2. red, green, and blue all from 0 to 1
*   3. User must specify initial value
* ABSTRACTION FUNCTION:
*	Represents a 3-vector of pixel brightness values, in percentage:
*		pixel = { red%, green%, blue% }
*/
struct color {

	// REPRESENTATION

	float r;
	float g;
	float b;

	// CONSTRUCTORS

	color(float r, float g, float b);
	color(float color[3]);

	// OPERATORS
	//	- No vector operations (color + color) b/c I want to discorage using raw sRGB values.
	//		Users are expected to handle values directly so the burden of "correctness" 
	//		(which doesn't exist with colors) is in their hands.
	
	/*
	* Scalar multiplication operator for simple brightness changes. Doubles as a division operator.
	* 
	* @param scalar - floating point value to multiply into elements
	* 
	* @requires 0.0f <= scalar
	* @returns ceil( scalar * color, 1.0f )
	*/
	color& operator*=(float scalar);

	/*
	* Scalar addition operator for simple brightness changes.
	* 
	* @param scalar - floating point value to add into elements
	* 
	* @returns clamp( color + { scalar, scalar, scalar }, 0.0f, 1.0f )
	*/
	color& operator+=(float scalar);
	/*
	* Scalar subtraction operator for simple brightness changes.
	*
	* @param scalar - floating point value to subtract into elements
	*
	* @returns clamp( color - { scalar, scalar, scalar }, 0.0f, 1.0f )
	*/
	color& operator-=(float scalar);
};

/*
* Scalar multiplication operator for simple brightness changes. Doubles as a division operator.
* 
* @param lhs - the color being multiplied 
* @param rhs - the floating point scalar to multiply in
* 
* @requires 0.0f <= scalar
* @returns ceil( lhs * rhs, 1.0f )
*/
color operator*(color lhs, float rhs);
/*
* Scalar multiplication operator for simple brightness changes. Doubles as a division operator.
*
* @param lhs - the floating point scalar to multiply in
* @param rhs - the color being multiplied 
*
* @requires 0.0f <= scalar
* @returns ceil( lhs * rhs, 1.0f )
*/
color operator*(float lhs, color rhs);

/* 
* Scalar addition operator for simple brightness changes.
* 
* @param lhs - the initial color
* @param rhs - the additional color as the magnitude of a scaled unit 3-vector
* 
* @returns clamp( lhs + { rhs, rhs, rhs }, 0.0f, 1.0f )
*/
color operator+(color lhs, float rhs);
/*
* Scalar addition operator for simple brightness changes.
*
* @param lhs - the additional color as the magnitude of a scaled unit 3-vector
* @param rhs - the initial color
*
* @returns clamp( { lhs, lhs, lhs } + rhs, 0.0f, 1.0f )
*/
color operator+(float left, color rhs);
/*
* Scalar subtraction operator for simple brightness changes.
*
* @param lhs - the initial color
* @param rhs - the negative color as the magnitude of a scaled unit 3-vector
*
* @returns clamp( lhs - { rhs, rhs, rhs }, 0.0f, 1.0f )
*/
color operator-(color lhs, float rhs);
/*
* Scalar subtraction operator for simple brightness changes.
*
* @param lhs - the negative color as the magnitude of a scaled unit 3-vector
* @param rhs - the initial color
*
* @returns clamp( { lhs, lhs, lhs } - rhs, 0.0f, 1.0f )
*/
color operator-(float left, color rhs);

/*
* Enumeration used in generating accent colors for a primary color using color harmony theory.
*	This class currently supports the following selections:
*		- Analogous
*		- Monochromatic
*		- Triad
*		- Complementary
*		- SplitComplementary
*		- Square
*		- Compound
*		- Shades
*		- Custom
*/
enum class Harmony {
	Analogous, // change hue, leave luminosity and chroma
	Monochromatic, // change luminosity and chroma, leave hue
	Triad, // rotate hue by 120 degrees, adjust luminosity and chroma after full revolution
	Complementary, // rotate hue by 180 degrees, adjust luminosity and chroma after full revolution
	SplitComplementary, // rotate hue by 165 degrees, an additional 15, and finally an additional 165, adjusting luminosity and chroma after full revolution
	Square, // rotate hue by 90 degrees, adjust luminosity and chroma after full revolution
	Compound, // rotate hue by 30 degrees, an additional 120, an additional 30, and finally an additional 180, adjusting luminosity and chroma after full revolution
	Shades, // change luminosity, leave hue and chroma
	Custom // add six copies of the primary shades, for the user to change
};

/*
* Wrapper for a vector of colors. 
*	Contains a constructor which will generate accent colors from a primary color and a harmony.
* 
* REPRESENTATION INVARIANT:
*	1. this->colors is an ordered list of objects of type color
*	2. this->colors is generated through harmony constructor following established norms in color theory on user selection
*   3. this->lchstarColors is an ordered list of objects of type color, for which the rgb channels of each 
*		element correspond to an L*C*h* representation of the element in the same position in this->colors
* ABSTRACTION FUNCTION:
*	colors[0] -> primary color
*	colors[i > 0] -> accent/secondary color
*/
struct palette {

	// REPRESENTATION

	std::vector<color> colors;

	// CONSTRUCTORS

	/*
	* Defaults constructor. Also functions as a second version of selecting "custom" as a harmony.
	*/
	palette(std::vector<color> colors);

	// IMPORTANT NOTE ABOUT HARMONY CONSTUCTOR AND PRIVATE METHODS:
	//	harmony constructor assumes the user is inputing an "sRGB" value, meaning it already linearized for monitors (and 0 - 1)
	//  sRGB is distinct from normal rgb we use for calculations so be careful with usage
	// NOTE: LChStar is defined for the purposes of this program as:
	//	- L* = "Luminosity" from 0 (black) - 100 (white)
	//  - C* = "Chroma" from 0 (gray) - ~150 (vivid)
	//  - h* = "Hue Angle" from 0 - 360 degrees

	/*
	* Constructor which generates accent colors from a primary color and harmony.
	* 
	* @param color0		- the primary color
	* @param harmony	- the harmony to generate colors from
	* 
	* @requires color0 is an array of floats with size 3 and values between 0 and 1
	* @ensures 
	*	|this->colors| = 6
	*	this->colors = { color0, ... [ accent colors generated by harmony ] }
	*/
	palette(color color0, Harmony harmony);
private:
	// INTERAL REPRESENTATION

	/*
	* TODO: use stored LChStar values to allow the user to modify color values in a consistent way
	*	(implement functions for outputting versions of palette colors with a change in luminosity, chroma, and/or hue)
	*/
	std::vector<color> lchstarColors;

	// UNCHECKED COLOR TRANSLATION FUNCTIONS (AI!!!!)

	color rgb2cielab(color rgb);

	color cielab2rgb(color lab);

	color cielab2lchstar(color lab);

	color lchstar2cielab(color lch);

	color lchstar2rgb(color lch);

	color rgb2lchstar(color rgb);
};