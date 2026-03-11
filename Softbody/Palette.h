#pragma once
#include <cassert>
#include <vector>

struct color {
	float r = 0.0f;
	float g = 0.0f;
	float b = 0.0f;
	color(float r, float g, float b) : r(r), g(g), b(b) {}
	color(float color[3]) : r(color[0]), g(color[1]), b(color[2]) {}

	color& operator*=(float scalar) {
		r *= scalar; g *= scalar; b *= scalar;
		return *this;
	}

	//color intensifyMultiplicative(float scalar) {
	//	assert(scalar >= 0.0f);

	//	color out = { scalar * r, scalar * g, scalar * b };
	//	if (out.r > 1.0f) out.r = 1.0f;
	//	if (out.g > 1.0f) out.g = 1.0f;
	//	if (out.b > 1.0f) out.b = 1.0f;
	//	return out;
	//}
	//color intensifyAdditive(float scalar) {
	//	assert(scalar >= -1.0f);
	//	assert(scalar <= 1.0f);

	//	color out = { r + scalar, g + scalar, b + scalar };
	//	if (out.r > 1.0f) out.r = 1.0f;
	//	else if (out.r < 0.0f) out.r = 0.0f;
	//	if (out.g > 1.0f) out.g = 1.0f;
	//	else if (out.g < 0.0f) out.g = 0.0f;
	//	if (out.b > 1.0f) out.b = 1.0f;
	//	else if (out.b < 0.0f) out.b = 0.0f;
	//	return out;
	//}
};

color operator*(color lhs, float rhs) {
	return color(lhs.r * rhs, lhs.g * rhs, lhs.b * rhs);
}
color operator*(float lhs, color rhs) {
	return color(lhs * rhs.r, lhs * rhs.g, lhs * rhs.b);
}

enum ColorHarmony {
	Analogous, Monochromatic, Triad, Complementary, SplitComplementary, Square, Compound, Shades, Custom
};

struct palette {
	std::vector<color> colors;


	palette(std::vector<color> colors) : colors(colors) {
		for (color c : colors) {
			lchstarColors.push_back(rgb2lchstar(c));
		}
	};
	
	// IMPORTANT NOTE ABOUT THIS CONSTUCTOR AND PRIVATE METHODS:
	//	this constructor assumes the user is inputing an "sRGB" value, meaning it already non-linearized for monitors (and 0 - 1)
	//  sRGB is distinct from normal rgb we use for calculations so be careful with usage
	// NOTE: LChStar is defined for the purposes of this program as:
	//	- L* = "Lightness" from 0 (black) - 100 (white)
	//  - C* = "Chroma" from 0 (gray) - ~150 (vivid)
	//  - h* = "Hue Angle" from 0 - 360 degrees
	palette(color color0, ColorHarmony harmony) {
		colors.push_back(color0);

		color c1({ 0.0f, 0.0f, 0.0f });
		color c2({ 0.0f, 0.0f, 0.0f });
		color c3({ 0.0f, 0.0f, 0.0f });
		color c4({ 0.0f, 0.0f, 0.0f });
		color c5({ 0.0f, 0.0f, 0.0f });

		color cielabPrimary1 = rgb2cielab(color0);
		color LChStar = cielab2lchstar(cielabPrimary1);

		float sat1 = LChStar.g;
		float sat2 = LChStar.g;
		float sat3 = LChStar.g;
		float sat4 = LChStar.g;
		float sat5 = LChStar.g;

		lchstarColors.push_back(LChStar);

		// all cases should output six base colors
		//	when adjusting chroma, subtract linearly and then take the absolute value
		//  this results in higher saturation values for low saturation inputs
		//  while still allowing for lower saturation value for high saturation inputs
		//  its a little janky and weird but it works (I came up with this stupid logic I'll fix it when it's a problem)
		switch (harmony) {
		case Monochromatic:
			sat1 = std::abs(sat1 - 150.0f * 1.0f / 6.0f);
			sat2 = std::abs(sat2 - 150.0f * 2.0f / 6.0f);
			sat3 = std::abs(sat3 - 150.0f * 3.0f / 6.0f);
			sat4 = std::abs(sat4 - 150.0f * 4.0f / 6.0f);
			sat5 = std::abs(sat5 - 150.0f * 5.0f / 6.0f);

			c1 = { LChStar.r * 5.0f / 6.0f, sat1, LChStar.b };
			c2 = { LChStar.r * 4.0f / 6.0f, sat2, LChStar.b };
			c3 = { LChStar.r * 3.0f / 6.0f, sat3, LChStar.b };
			c4 = { LChStar.r * 2.0f / 6.0f, sat4, LChStar.b };
			c5 = { LChStar.r * 1.0f / 6.0f, sat5, LChStar.b };
			break;
		case Complementary:
			sat2 = std::abs(sat2 - 37.5f);
			sat3 = sat2;
			sat4 = std::abs(sat4 - 75.0f);
			sat5 = sat4;

			c1 = { LChStar.r, sat1, std::fmodf((LChStar.b + 180.0f), 360.0f) };
			c2 = { LChStar.r * 3.0f / 4.0f, sat2, LChStar.b };
			c3 = { LChStar.r * 3.0f / 4.0f, sat3, std::fmodf((LChStar.b + 180.0f), 360.0f)};
			c4 = { LChStar.r * 1.0f / 2.0f, sat4, LChStar.b };
			c5 = { LChStar.r * 1.0f / 2.0f, sat5, std::fmodf((LChStar.b + 180.0f), 360.0f) };
			break;
		case SplitComplementary:
			sat3 = std::abs(sat3 - 75.0f);
			sat4 = std::abs(sat4 - 75.0f);
			sat5 = std::abs(sat5 - 75.0f);

			c1 = { LChStar.r, sat1, std::fmodf((LChStar.b + 195.0f), 360.0f) };
			c2 = { LChStar.r, sat2, std::fmodf((LChStar.b + 170.0f), 360.0f) };
			c3 = { LChStar.r * 0.5f, sat3, LChStar.b };
			c4 = { LChStar.r * 0.5f, sat4, std::fmodf((LChStar.b + 195.0f), 360.0f) };
			c5 = { LChStar.r * 0.5f, sat5, std::fmodf((LChStar.b + 170.0f), 360.0f) };
			break;
		case Analogous:
			sat5 = std::abs(sat5 - 37.5f);

			c1 = { LChStar.r, sat1, std::fmodf((LChStar.b + 30.0f), 360.0f) };
			c2 = { LChStar.r, sat2, std::fmodf((LChStar.b - 30.0f), 360.0f) };
			c3 = { LChStar.r, sat3, std::fmodf((LChStar.b + 15.0f), 360.0f) };
			c4 = { LChStar.r, sat4, std::fmodf((LChStar.b - 15.0f), 360.0f) };
			c5 = { LChStar.r, sat5, LChStar.b };
			break;
		case Triad:
			sat3 = std::abs(sat3 - 50.0f);
			sat4 = std::abs(sat4 - 50.0f);
			sat5 = std::abs(sat5 - 50.0f);

			c1 = { LChStar.r, sat1, std::fmodf((LChStar.b + 120.0f), 360.0f) };
			c2 = { LChStar.r, sat2, std::fmodf((LChStar.b + 240.0f), 360.0f) };
			c3 = { LChStar.r * 2.0f / 3.0f, sat3, LChStar.b };
			c4 = { LChStar.r * 2.0f / 3.0f, sat4, std::fmodf((LChStar.b + 120.0f), 360.0f) };
			c5 = { LChStar.r * 2.0f / 3.0f, sat5, std::fmodf((LChStar.b + 240.0f), 360.0f) };
			break;
		case Square:
			sat4 = std::abs(sat4 - 50.0f);
			sat5 = std::abs(sat5 - 75.0f);

			c1 = { LChStar.r, sat1, std::fmodf((LChStar.b + 30.0f), 360.0f) };
			c2 = { LChStar.r, sat2, std::fmodf((LChStar.b + 150.0f), 360.0f) };
			c3 = { LChStar.r, sat3, std::fmodf((LChStar.b + 180.0f), 360.0f) };
			c4 = { LChStar.r * 2.0f / 3.0f, sat4, LChStar.b };
			c5 = { LChStar.r * 0.5f, sat5, std::fmodf((LChStar.b + 30.0f), 360.0f) };
			break;
		case Compound:
			sat4 = std::abs(sat4 - 50.0f);
			sat5 = std::abs(sat5 - 75.0f);

			c1 = { LChStar.r, sat1, std::fmodf((LChStar.b + 90.0f), 360.0f) };
			c2 = { LChStar.r, sat2, std::fmodf((LChStar.b + 180.0f), 360.0f) };
			c3 = { LChStar.r, sat3, std::fmodf((LChStar.b + 270.0f), 360.0f) };
			c4 = { LChStar.r * 2.0f / 3.0f, sat4, LChStar.b };
			c5 = { LChStar.r * 0.5f, sat5, std::fmodf((LChStar.b + 90.0f), 360.0f) };
			break;
		case Shades:
			c1 = { LChStar.r * 5.0f / 6.0f, sat1, LChStar.b };
			c2 = { LChStar.r * 4.0f / 6.0f, sat2, LChStar.b };
			c3 = { LChStar.r * 3.0f / 6.0f, sat3, LChStar.b };
			c4 = { LChStar.r * 2.0f / 6.0f, sat4, LChStar.b };
			c5 = { LChStar.r * 1.0f / 6.0f, sat5, LChStar.b };
			break;
		default:
			return;
			break;
		}

		lchstarColors.push_back(c1);
		lchstarColors.push_back(c2);
		lchstarColors.push_back(c3);
		lchstarColors.push_back(c4);
		lchstarColors.push_back(c5);

		colors.push_back(lchstar2rgb(c1));
		colors.push_back(lchstar2rgb(c2));
		colors.push_back(lchstar2rgb(c3));
		colors.push_back(lchstar2rgb(c4));
		colors.push_back(lchstar2rgb(c5));
	}
private:
	std::vector<color> lchstarColors;

	// ALL OF THESE COLOR VALUE TRANSLATIONS ARE FROM GOOGLE AI!!!
	color rgb2cielab(color rgb) {
		// gamma correction
		auto cLinear = [](float cSRGB) {
			float condition = 0.04045f;
			if (cSRGB <= condition) {
				return cSRGB / 12.92f;
			}
			else {
				return std::powf((cSRGB + 0.055f) / 1.055f, 2.4f);
			}
			};

		rgb.r = cLinear(rgb.r);
		rgb.g = cLinear(rgb.g);
		rgb.b = cLinear(rgb.b);

		// need to multiply a matrix into rgb to get it into xyz
		// since matrix ops are implemented for color this will be hard-coded
		color xyz(0.0f, 0.0f, 0.0f);
		xyz.r = 0.4124564f * rgb.r + 0.3575761f * rgb.g + 0.1804375f * rgb.b;
		xyz.g = 0.2126729f * rgb.r + 0.7151522f * rgb.g + 0.0721750f * rgb.b;
		xyz.b = 0.0193339f * rgb.r + 0.1191920f * rgb.g + 0.9503041f * rgb.b;

		color referenceXYZ = { 0.95047f, 1.0f, 1.08883f }; // reference white value
		xyz.r = xyz.r / referenceXYZ.r;
		xyz.g = xyz.g / referenceXYZ.g;
		xyz.b = xyz.b / referenceXYZ.b;

		auto f = [](float t) {
			float condition = 216.0f / 24389.0f; // ( 6/29 ) ^3
			if (t > condition) {
				return std::cbrtf(t);
			}
			else {
				return (841.0f / 108.0f) * t + 4.0f / 29.0f; // first term is (1/3) * (29/6)^2 * t
			}
			};

		float fx = f(xyz.r);
		float fy = f(xyz.g);
		float fz = f(xyz.b);
		float lStar = 116.0f * fy - 16.0f;
		float aStar = 500.0f * (fx - fy);
		float bStar = 200.0f * (fy - fz);

		return color(lStar, aStar, bStar);
	}

	color cielab2rgb(color lab) {
		float fy = (lab.r + 16.0f) / 116.0f;
		float fx = fy + lab.g / 500.0f;
		float fz = fy - lab.b / 200.0f;

		auto fInverse = [](float t) {
			float condition = 6.0f / 29.0f;
			if (t > condition) {
				return std::powf(t, 3.0f);
			}
			else {
				return (108.0f / 841.0f) * (t - 4.0f / 29.0f);
			}
			};

		color xyz(0, 0, 0);

		color referenceXYZ = { 0.95047f, 1.0f, 1.08883f }; // reference white value
		xyz.r = referenceXYZ.r * fInverse(fx);
		xyz.g = referenceXYZ.g * fInverse(fy);
		xyz.b = referenceXYZ.b * fInverse(fz);

		auto cSRGB = [](float cLin) {
			float condition = 0.0031308f;
			if (cLin <= condition) {
				return 12.92f * cLin;
			}
			else {
				return 1.055f * std::powf(cLin, 1.0f / 2.4f) - 0.055f;
			}
			};

		color rgb(0.0f, 0.0f, 0.0f);
		rgb.r =  3.2405f * xyz.r - 1.5371f * xyz.g - 0.4985f * xyz.b;
		rgb.g = -0.9693f * xyz.r + 1.8760f * xyz.g + 0.0416f * xyz.b;
		rgb.b =  0.0556f * xyz.r - 0.2040f * xyz.g + 1.0572f * xyz.b;

		rgb.r = cSRGB(rgb.r);
		rgb.g = cSRGB(rgb.g);
		rgb.b = cSRGB(rgb.b);

		rgb.r = std::clamp(rgb.r, 0.0f, 1.0f);
		rgb.g = std::clamp(rgb.g, 0.0f, 1.0f);
		rgb.b = std::clamp(rgb.b, 0.0f, 1.0f);

		return rgb;
	}

	color cielab2lchstar(color lab) {
		color LChStar = lab;
		LChStar.g = std::sqrtf(std::powf(lab.g, 2) + std::powf(lab.b, 2));
		LChStar.b = std::atan2f(lab.b , lab.g) * 180.0f / pi;
		if (LChStar.b < 0.0f) {
			LChStar.b += 360.0f;
		}

		return LChStar;
	}

	color lchstar2cielab(color lch) {
		color lab = lch;
		float hRad = lch.b * pi / 180.0f;
		lab.g = lch.g * std::cosf(hRad);
		lab.b = lch.g * std::sinf(hRad);

		return lab;
	}

	color lchstar2rgb(color lch) {
		return cielab2rgb(lchstar2cielab(lch));
	}

	color rgb2lchstar(color rgb) {
		return cielab2lchstar(rgb2cielab(rgb));
	}
};