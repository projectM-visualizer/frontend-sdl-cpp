#pragma once

#include <Poco/Logger.h>

#include <CoreAudio/CoreAudio.h>

#include <map>
#include <string>

class projectm;

/**
 * @brief macOS Core Audio process-tap capturing implementation.
 *
 * Captures system-wide audio output using the Core Audio process-tap API
 * (available since macOS 14.4). A global tap is attached to a private aggregate
 * device; an IO proc on that aggregate device forwards the captured PCM data
 * directly into projectM. This is the macOS equivalent of the WASAPI loopback
 * backend used on Windows and requires no third-party virtual audio driver.
 *
 * v1 exposes a single synthetic "System Audio" device. The device-list structure
 * is preserved so per-process capture can be added later without changing the
 * AudioCapture proxy.
 */
class CoreAudioTapCapture
{
public:
    CoreAudioTapCapture();

    ~CoreAudioTapCapture();

    /**
     * @brief Returns a map of available capture sources.
     *
     * v1 returns a single synthetic entry representing whole-system audio output.
     *
     * @return A map with device index/name pairs.
     */
    std::map<int, std::string> AudioDeviceList();

    /**
     * @brief Starts capturing system audio and forwarding it to projectM.
     * @param projectMHandle projectM instance handle that will receive the captured data.
     * @param audioDeviceIndex Ignored in v1 (single system-audio source).
     */
    void StartRecording(projectm* projectMHandle, int audioDeviceIndex);

    /**
     * @brief Stops capturing and tears down the tap and aggregate device.
     */
    void StopRecording();

    /**
     * @brief Switches to the next available capture source. No-op in v1.
     */
    void NextAudioDevice();

    /**
     * @brief Activates the source with the given index. Clamped/no-op in v1.
     * @param index The index, as listed by @a AudioDeviceList().
     */
    void AudioDeviceIndex(int index);

    /**
     * @brief Returns the currently used source index.
     * @return The index of the current source.
     */
    int AudioDeviceIndex() const;

    /**
     * @brief Retrieves the current source name.
     * @return The human-readable name of the current capture source.
     */
    std::string AudioDeviceName() const;

    /**
     * @brief Asks the capture client to fill projectM's audio buffer for the next frame.
     *
     * No-op: the Core Audio IO proc pushes PCM directly to projectM as it arrives,
     * matching the SDL backend's callback-driven model.
     */
    void FillBuffer() {};

protected:
    /**
     * @brief Creates the process tap and private aggregate device, then starts the IO proc.
     * @return True on success, false if any Core Audio call failed (state is torn down on failure).
     */
    bool StartTap();

    /**
     * @brief Stops the IO proc and destroys the aggregate device and tap in the correct order.
     */
    void StopTap();

    /**
     * @brief Returns the UID of the current default output device, or empty on failure.
     *
     * The aggregate device requires a real output device as its main sub-device.
     */
    std::string DefaultOutputDeviceUID() const;

    projectm* _projectMHandle{nullptr}; //!< projectM instance that receives the audio data.
    int _currentAudioDeviceIndex{-1}; //!< Currently selected source index (always -1 in v1).

    AudioObjectID _tapObjectID{kAudioObjectUnknown}; //!< The process tap object.
    AudioObjectID _aggregateDeviceID{kAudioObjectUnknown}; //!< The private aggregate device wrapping the tap.
    AudioDeviceIOProcID _ioProcID{nullptr}; //!< The installed IO proc handle.

    uint32_t _channels{2}; //!< Channel count of the captured stream, from the aggregate device format.

    static constexpr char _systemAudioDeviceName[] = "System Audio (Core Audio Tap)";

    Poco::Logger& _logger{Poco::Logger::get("AudioCapture.CoreAudioTap")}; //!< The class logger.
};
