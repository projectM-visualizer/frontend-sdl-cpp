#include "AudioDataAvailableNotification.h"

#include <Poco/Bugcheck.h>

#include <cstring>

std::string AudioDataAvailableNotification::name() const
{
    return "AudioDataAvailableNotification";
}

AudioDataAvailableNotification::AudioDataAvailableNotification(uint32_t channels, const float* samples, const uint32_t sampleCount)
    : _channels(channels)
{
    poco_assert(channels > 0);
    poco_assert(samples != nullptr);
    poco_assert(sampleCount != 0);
    poco_assert(sampleCount % channels == 0);

    _samples.resize(sampleCount);
    std::memcpy(_samples.data(), samples, sampleCount * sizeof(float));
}

uint32_t AudioDataAvailableNotification::Channels() const
{
    return _channels;
}

const std::vector<float>& AudioDataAvailableNotification::Samples() const
{
    return _samples;
}