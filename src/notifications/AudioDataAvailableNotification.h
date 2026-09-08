#pragma once

#include <Poco/Notification.h>

#include <cstdint>
#include <vector>

/**
 * @brief Passes new audio data to all active projectM instances.
 * @note Channels must be >0 and samples must contain at least one audio frame/sample.
 */
class AudioDataAvailableNotification : public Poco::Notification
{
public:
    std::string name() const override;

    AudioDataAvailableNotification() = delete;

    /**
     * @brief Creates a new AudioDataAvailableNotification instance
     * The number of @a samples must be divisible by @a channels.
     * @param channels The channel count of the current capture device.
     * @param samples The new audio samples captured from the active audio device.
     * @param sampleCount The number of float samples passed to @a samples.
     */
    explicit AudioDataAvailableNotification(uint32_t channels, const float* samples, uint32_t sampleCount);

    /**
     * Returns the number of audio channels of the recording device.
     * @return The number of audio channels of the recording device.
     */
    [[nodiscard]] uint32_t Channels() const;

    /**
     * Returns the new audio samples captured from the active audio device.
     * @return The new audio samples captured from the active audio device.
     */
    [[nodiscard]] const std::vector<float>& Samples() const;

private:
    uint32_t _channels; //!< Number of audio channels of the capture device.
    std::vector<float> _samples; //!< The new audio samples, containing frames x channels floats.
};
