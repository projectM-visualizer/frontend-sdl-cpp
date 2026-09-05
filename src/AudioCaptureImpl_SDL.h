#pragma once

#ifdef USE_SDL3
# include <SDL3/SDL.h>
#else
# include <SDL2/SDL.h>
#endif

#include <Poco/Logger.h>

#include <string>
#include <vector>

class projectm;

/**
 * @brief SDL-based audio capturing thread.
 *
 * Uses SDL's audio API to capture PCM data from any supported drivers.
 */
class AudioCaptureImpl
{
public:
    AudioCaptureImpl();

    ~AudioCaptureImpl();

    /**
     * @brief Returns a map of available recording devices.
     * @return A vector of available audio device IDs and names.
     */
    std::map<int, std::string> AudioDeviceList();

    /**
     * @brief Starts audio capturing with the first available device.
     * @param projectMHandle projectM instance handle that will receive the captured data.
     * @param audioDeviceIndex The initial audio device ID to capture from. Use -1 to select the implementation's
     *                      default device.
     */
    void StartRecording(projectm* projectMHandle, int audioDeviceIndex);

    /**
     * @brief Stops audio recording.
     */
    void StopRecording();

    /**
     * @brief Switches to the next available audio recording device.
     */
    void NextAudioDevice();

    /**
     * @brief Activates the audio device with the given idnex for recording.
     * @param index The index, as listed by @a AudioDeviceList()
     */
    void AudioDeviceIndex(int index);

    /**
     * @brief Returns the currently used Audio device index.
     * @return The index of the current audio device, as listed by @a AudioDeviceList()
     */
    int AudioDeviceIndex() const;

    /**
     * @brief Retrieves the current audio device name.
     * @return The name of the currently selected audio recording device.
     */
    std::string AudioDeviceName() const;

    /**
     * @brief Asks the capture client to fill projectM's audio buffer for the next frame.
     *
     * In SDL2, audio is captured via callback. In SDL3, audio data is polled here.
     */
    void FillBuffer();

    /**
     * @brief Returns the current audio peak level.
     * @return A value between 0.0 and 1.0, or -1.0 if not available.
     */
    float CurrentAudioLevel() const;

    /**
     * @brief Refreshes the audio device list (e.g. after a hotplug event).
     */
    void RefreshDeviceList();

protected:
    /**
     * @brief Opens the SDL audio device with the currently selected index.
     * @return
     */
    bool OpenAudioDevice();

#ifndef USE_SDL3
    /**
     * @brief SDL2 audio capture callback.
     *
     * Called everytime if there is new data available in the audio recording buffer.
     *
     * @param userData
     * @param stream
     * @param len
     */
    static void AudioInputCallback(void* userData, unsigned char* stream, int len);
#endif

    projectm* _projectMHandle{nullptr}; //!< Handle if the projectM instance that will receive the audio data.
    int32_t _currentAudioDeviceIndex{-1}; //!< Currently selected audio device index.
#ifdef USE_SDL3
    SDL_AudioStream* _recordingStream{nullptr}; //!< SDL3 audio stream for recording.
#else
    SDL_AudioDeviceID _currentAudioDeviceID{0}; //!< Device ID of the currently opened audio device.
#endif
    uint32_t _channels{2};

    constexpr static uint32_t _requestedSampleFrequency{44100}; //!< Requested sample frequency. Currently hardcoded as 44100 Hz, as this is what the spectrum analyzer expects.
    uint32_t _requestedSampleCount{44100U / 60U}; //!< Requested audio buffer size. Determines how often SDL will call AudioInputCallback() with new data, and how much data is delivered on each call.

    Poco::Logger& _logger{Poco::Logger::get("AudioCapture.SDL")}; //!< The class logger.
};
