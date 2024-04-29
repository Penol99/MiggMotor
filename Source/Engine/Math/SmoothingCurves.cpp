#include "engine_pch.h"
#include "SmoothingCurves.h"

float mi::SmoothingCurves::Linear(const float aTValue)
{
    return aTValue;
}
float mi::SmoothingCurves::StepStart(const float aTValue)
{
    if (aTValue > 0.000001f)
        return 1.0f;
    return 0.0f;
}

float mi::SmoothingCurves::StepEnd(const float aTValue)
{
    if (aTValue >= 1.0f)
        return 1.0f;
    return 0.0f;
}