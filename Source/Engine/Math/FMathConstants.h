#pragma once

namespace mi
{
	namespace FMath
	{

		constexpr float Pi = 3.14159265358979323846f;
		constexpr float Pi_Half = Pi / 2.f;
		constexpr float DegToRad = Pi / 180.f;
		constexpr float RadToDeg = 180.f / Pi;

		constexpr float SmallNumber = (1.e-8f);
		constexpr float KindaSmallNumber = (1.e-4f);

		constexpr float Tau = Pi * 2.0f;
		constexpr double PiD = 3.1415926535897932384626433832795;
		constexpr double PiHalfD = PiD * 0.5;
		constexpr double TauD = PiD * 2.0;
	}
}