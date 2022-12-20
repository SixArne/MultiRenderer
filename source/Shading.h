#pragma once
#include <SDL_stdinc.h>

#include "ColorRGB.h"
#include "Vector3.h"


namespace Shading
{
	static ColorRGB Lambert(float kd, const ColorRGB& cd)
	{
		return cd * (kd / (float)M_PI);
	}

	static ColorRGB Phong(ColorRGB ks, ColorRGB exp, const Vector3& l, const Vector3& v, const Vector3& n)
	{
		const auto reflect = (2.f * (Vector3::Dot(n, l) * n)) - l;
		const auto angle = std::max(0.f, Vector3::Dot(reflect, v));
		const auto reflection = ks * std::powf(angle, exp.r);

		// return reflection for all color
		return ColorRGB{ reflection.r, reflection.g, reflection.b };
	}
}
