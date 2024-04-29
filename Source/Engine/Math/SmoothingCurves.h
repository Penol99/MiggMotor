#pragma once

namespace mi
{
	class SmoothingCurves
	{
	public:
		static float Linear(const float aTValue);
		static float StepStart(const float aTValue);
		static float StepEnd(const float aTValue);
	};
}