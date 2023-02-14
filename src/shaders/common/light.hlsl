#pragma once

float GetShadowAdjustedBias(float bias_min, float bias_max, float NoL)
{
    return max(bias_min, bias_max * (1.0 - NoL));
}
