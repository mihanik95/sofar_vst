#ifndef IR_DATA_H
#define IR_DATA_H

#include <vector>
#include <JuceHeader.h>

inline std::vector<float> getIR(int index)
{
    constexpr int len = 256;
    static std::vector<float> irA(len), irB(len), irC(len), irD(len);
    static bool initialised = false;
    if (!initialised)
    {
        for (int i = 0; i < len; ++i)
        {
            float t = (float)i / (float)len;
            irA[i] = std::exp(-5.0f * t) * (0.8f + 0.2f * std::sin(juce::MathConstants<float>::twoPi * 0.05f * i));
            irB[i] = std::exp(-3.0f * t) * (0.7f + 0.3f * std::cos(juce::MathConstants<float>::twoPi * 0.07f * i));
            irC[i] = std::exp(-7.0f * t) * (0.5f - 0.5f * std::cos(juce::MathConstants<float>::twoPi * i / len));
            irD[i] = std::exp(-2.0f * t) * std::sin(juce::MathConstants<float>::twoPi * 0.1f * i);
        }
        initialised = true;
    }

    switch (index)
    {
        case 0:  return irA;
        case 1:  return irB;
        case 2:  return irC;
        default: return irD;
    }
}

#endif // IR_DATA_H
