#pragma once

#include <Poco/Logger.h>

#include <map>
#include <string>

class projectm;
class CoreAudioTapCapture;
class SdlAudioCapture;

/**
 * @brief macOS audio-capture backend selector.
 *
 * macOS can capture system output directly with the Core Audio process-tap API,
 * but that API only exists on macOS 14.4 and newer. To keep working on older
 * systems, this class picks a backend at runtime: the Core Audio tap on 14.4+,
 * and the shared SDL microphone capture (the same backend used on Linux) below
 * that. Exactly one backend is instantiated for the lifetime of the object.
 *
 * It exposes the interface AudioCapture expects and forwards every call to the
 * selected backend, so AudioCapture itself stays platform-agnostic.
 */
class AudioCaptureImpl
{
public:
    AudioCaptureImpl();

    ~AudioCaptureImpl();

    std::map<int, std::string> AudioDeviceList();

    void StartRecording(projectm* projectMHandle, int audioDeviceIndex);

    void StopRecording();

    void NextAudioDevice();

    void AudioDeviceIndex(int index);

    int AudioDeviceIndex() const;

    std::string AudioDeviceName() const;

    void FillBuffer();

protected:
    CoreAudioTapCapture* _tap{nullptr}; //!< Core Audio process-tap backend (macOS 14.4+), or null.
    SdlAudioCapture* _sdl{nullptr}; //!< SDL microphone backend (pre-14.4 fallback), or null.

    Poco::Logger& _logger{Poco::Logger::get("AudioCapture.macOS")}; //!< The class logger.
};
