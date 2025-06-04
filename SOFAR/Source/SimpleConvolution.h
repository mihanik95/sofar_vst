#ifndef SIMPLE_CONVOLUTION_H
#define SIMPLE_CONVOLUTION_H

#include <vector>
#include <algorithm>
#include <cmath>
#include <JuceHeader.h>

class SimpleConvolution
{
public:
    void loadImpulse(const std::vector<float>& newIR)
    {
        ir = newIR;
        buffers.clear();
    }

    void prepare(int channels)
    {
        buffers.resize(channels);
        for (auto& buf : buffers)
            buf.assign(ir.size(), 0.0f);
    }

    void process(juce::AudioBuffer<float>& buffer)
    {
        auto numChannels = buffer.getNumChannels();
        auto numSamples  = buffer.getNumSamples();
        for (int ch = 0; ch < numChannels; ++ch)
        {
            auto* data = buffer.getWritePointer(ch);
            auto& state = buffers[ch];
            for (int i = 0; i < numSamples; ++i)
            {
                auto in = data[i];
                state.insert(state.begin(), in);
                if (state.size() > ir.size())
                    state.pop_back();

                float out = 0.0f;
                for (size_t n = 0; n < ir.size(); ++n)
                    out += state[n] * ir[n];
                data[i] = out;
            }
        }
    }

private:
    std::vector<float> ir;
    std::vector<std::vector<float>> buffers;
};

#endif // SIMPLE_CONVOLUTION_H
