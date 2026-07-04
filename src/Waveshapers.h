#pragma once

#include <JuceHeader.h>
#include <cmath>
#include <algorithm>

namespace Waveshapers
{
    inline float hardClip(float x)
    {
        return std::clamp(x, -1.0f, 1.0f);
    }

    inline float tanhClip(float x)
    {
        return std::tanh(x);
    }

    inline float atanClip(float x)
    {
        return (2.0f / juce::MathConstants<float>::pi) * std::atan(x);
    }

    inline float cubicSoftClip(float x)
    {
        x = std::clamp(x, -1.5f, 1.5f);

        if (x > 1.0f)
            return 2.0f / 3.0f;

        if (x < -1.0f)
            return -2.0f / 3.0f;

        return x - (x * x * x) / 3.0f;
    }

    inline float processSample(float x, int mode)
   {
        switch (mode)
        {
            case 0:
                return hardClip(x);

            case 1:
                return tanhClip(x);

            case 2:
                return atanClip(x);

            case 3:
                return cubicSoftClip(x);

            default:
                return tanhClip(x);
        }
    }
}