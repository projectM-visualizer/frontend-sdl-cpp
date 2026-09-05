#include "AudioCaptureImpl_SDL.h"

#include <Poco/Util/Application.h>

#include <projectM-4/projectM.h>

#ifdef USE_SDL3
#include <vector>
#endif

AudioCaptureImpl::AudioCaptureImpl()
    : _requestedSampleCount(projectm_pcm_get_max_samples())
{
    auto targetFps = Poco::Util::Application::instance().config().getUInt("projectM.fps", 60);
    if (targetFps > 0)
    {
        _requestedSampleCount = std::min(_requestedSampleFrequency / targetFps, _requestedSampleCount);
        _requestedSampleCount = std::max(_requestedSampleCount, 300U);
    }

    SDL_InitSubSystem(SDL_INIT_AUDIO);
#ifndef USE_SDL3
#ifdef SDL_HINT_AUDIO_INCLUDE_MONITORS
    SDL_SetHint(SDL_HINT_AUDIO_INCLUDE_MONITORS, "1");
#endif
#endif
}

AudioCaptureImpl::~AudioCaptureImpl()
{
    StopRecording();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

std::map<int, std::string> AudioCaptureImpl::AudioDeviceList()
{
    std::map<int, std::string> deviceList{
        {-1, "Default recording device"}};

#ifdef USE_SDL3
    int deviceCount = 0;
    auto deviceIds = SDL_GetAudioRecordingDevices(&deviceCount);
    if (deviceIds)
    {
        for (int i = 0; i < deviceCount; i++)
        {
            auto deviceName = SDL_GetAudioDeviceName(deviceIds[i]);
            if (deviceName)
            {
                deviceList.insert(std::make_pair(i, deviceName));
            }
            else
            {
                poco_error_f2(_logger, "Could not get device name for device ID %d: %s", i, std::string(SDL_GetError()));
            }
        }
        SDL_free(deviceIds);
    }
#else
    auto recordingDeviceCount = SDL_GetNumAudioDevices(true);

    for (int i = 0; i < recordingDeviceCount; i++)
    {
        auto deviceName = SDL_GetAudioDeviceName(i, true);
        if (deviceName)
        {
            deviceList.insert(std::make_pair(i, deviceName));
        }
        else
        {
            poco_error_f2(_logger, "Could not get device name for device ID %d: %s", i, std::string(SDL_GetError()));
        }
    }
#endif

    return deviceList;
}

void AudioCaptureImpl::StartRecording(projectm* projectMHandle, int audioDeviceIndex)
{
    _projectMHandle = projectMHandle;
    _currentAudioDeviceIndex = audioDeviceIndex;

    poco_debug_f1(_logger, "Using SDL audio driver \"%s\".", std::string(SDL_GetCurrentAudioDriver()));

    if (OpenAudioDevice())
    {
        poco_debug(_logger, "Started audio recording.");
    }
}

void AudioCaptureImpl::StopRecording()
{
#ifdef USE_SDL3
    if (_recordingStream)
    {
        SDL_DestroyAudioStream(_recordingStream);
        _recordingStream = nullptr;
        poco_debug(_logger, "Stopped audio recording and closed device.");
    }
#else
    if (_currentAudioDeviceID)
    {
        SDL_PauseAudioDevice(_currentAudioDeviceID, true);
        SDL_CloseAudioDevice(_currentAudioDeviceID);
        _currentAudioDeviceID = 0;
        poco_debug(_logger, "Stopped audio recording and closed device.");
    }
#endif
}

void AudioCaptureImpl::NextAudioDevice()
{
    StopRecording();

#ifdef USE_SDL3
    int deviceCount = 0;
    SDL_GetAudioRecordingDevices(&deviceCount);
    int nextAudioDeviceId = ((_currentAudioDeviceIndex + 2) % (deviceCount + 1)) - 1;
#else
    int nextAudioDeviceId = ((_currentAudioDeviceIndex + 2) % (SDL_GetNumAudioDevices(true) + 1)) - 1;
#endif

    StartRecording(_projectMHandle, nextAudioDeviceId);
}

void AudioCaptureImpl::AudioDeviceIndex(int index)
{
#ifdef USE_SDL3
    int deviceCount = 0;
    SDL_GetAudioRecordingDevices(&deviceCount);
    if (index >= -1 && index < deviceCount)
    {
        StopRecording();
        _currentAudioDeviceIndex = index;
        StartRecording(_projectMHandle, index);
    }
#else
    if (index >= -1 && index < SDL_GetNumAudioDevices(true))
    {
        StopRecording();
        _currentAudioDeviceIndex = index;
        StartRecording(_projectMHandle, index);
    }
#endif
}

int AudioCaptureImpl::AudioDeviceIndex() const
{
    return _currentAudioDeviceIndex;
}

std::string AudioCaptureImpl::AudioDeviceName() const
{
#ifdef USE_SDL3
    if (_currentAudioDeviceIndex >= 0)
    {
        int deviceCount = 0;
        auto deviceIds = SDL_GetAudioRecordingDevices(&deviceCount);
        if (deviceIds && _currentAudioDeviceIndex < deviceCount)
        {
            auto name = SDL_GetAudioDeviceName(deviceIds[_currentAudioDeviceIndex]);
            std::string result(name ? name : "Unknown device");
            SDL_free(deviceIds);
            return result;
        }
        if (deviceIds)
        {
            SDL_free(deviceIds);
        }
    }
    return "Default recording device";
#else
    if (_currentAudioDeviceIndex >= 0)
    {
        return SDL_GetAudioDeviceName(_currentAudioDeviceIndex, true);
    }
    return "Default recording device";
#endif
}

bool AudioCaptureImpl::OpenAudioDevice()
{
#ifdef USE_SDL3
    SDL_AudioSpec spec{};
    spec.freq = static_cast<int>(_requestedSampleFrequency);
    spec.format = SDL_AUDIO_F32;
    spec.channels = 2;

    SDL_AudioDeviceID deviceId = SDL_AUDIO_DEVICE_DEFAULT_RECORDING;
    if (_currentAudioDeviceIndex >= 0)
    {
        int deviceCount = 0;
        auto deviceIds = SDL_GetAudioRecordingDevices(&deviceCount);
        if (deviceIds && _currentAudioDeviceIndex < deviceCount)
        {
            deviceId = deviceIds[_currentAudioDeviceIndex];
        }
        if (deviceIds)
        {
            SDL_free(deviceIds);
        }
    }

    _recordingStream = SDL_OpenAudioDeviceStream(deviceId, &spec, nullptr, nullptr);
    if (!_recordingStream)
    {
        poco_error_f2(_logger, R"(Failed to open audio recording device (ID %?d): %s)",
                      _currentAudioDeviceIndex,
                      std::string(SDL_GetError()));
        return false;
    }

    _channels = spec.channels;

    SDL_ResumeAudioStreamDevice(_recordingStream);

    auto deviceName = SDL_GetAudioDeviceName(deviceId);
    poco_information_f4(_logger, R"(Opened audio recording device "%s" (ID %?d) with %?d channels at %?d Hz.)",
                        std::string(deviceName ? deviceName : "System default recording device"),
                        _currentAudioDeviceIndex,
                        spec.channels,
                        spec.freq);

    return true;
#else
    SDL_AudioSpec requestedSpecs{};
    SDL_AudioSpec actualSpecs{};

    requestedSpecs.freq = _requestedSampleFrequency;
    requestedSpecs.format = AUDIO_F32;
    requestedSpecs.channels = 2;
    requestedSpecs.samples = _requestedSampleCount;
    requestedSpecs.callback = AudioCaptureImpl::AudioInputCallback;
    requestedSpecs.userdata = this;

    auto deviceName = SDL_GetAudioDeviceName(_currentAudioDeviceIndex, true);
    _currentAudioDeviceID = SDL_OpenAudioDevice(deviceName, true, &requestedSpecs, &actualSpecs, SDL_AUDIO_ALLOW_CHANNELS_CHANGE);

    if (_currentAudioDeviceID == 0)
    {
        poco_error_f3(_logger, R"(Failed to open audio device "%s" (ID %?d): %s)",
                      std::string(deviceName != nullptr ? deviceName : "System default recording device"),
                      _currentAudioDeviceIndex,
                      std::string(SDL_GetError()));
        return false;
    }

    _channels = actualSpecs.channels;

    poco_information_f4(_logger, R"(Opened audio recording device "%s" (ID %?d) with %?d channels at %?d Hz.)",
                        std::string(deviceName != nullptr ? deviceName : "System default recording device"),
                        _currentAudioDeviceIndex,
                        actualSpecs.channels,
                        actualSpecs.freq);

    return true;
#endif
}

#ifdef USE_SDL3
void AudioCaptureImpl::FillBuffer()
{
    if (!_recordingStream || !_projectMHandle)
    {
        return;
    }

    int available = SDL_GetAudioStreamAvailable(_recordingStream);
    if (available <= 0)
    {
        return;
    }

    std::vector<float> buffer(available / sizeof(float));
    int bytesRead = SDL_GetAudioStreamData(_recordingStream, buffer.data(), available);

    if (bytesRead > 0)
    {
        unsigned int samples = bytesRead / sizeof(float) / _channels;
        projectm_pcm_add_float(_projectMHandle, buffer.data(), samples,
                               static_cast<projectm_channels>(_channels));
    }
}
#else
void AudioCaptureImpl::FillBuffer()
{
    // SDL2 uses async callbacks to directly fill projectM's audio buffer.
}

void AudioCaptureImpl::AudioInputCallback(void* userData, unsigned char* stream, int len)
{
    poco_assert_dbg(userData);
    auto instance = reinterpret_cast<AudioCaptureImpl*>(userData);

    unsigned int samples = len / sizeof(float) / instance->_channels;

    projectm_pcm_add_float(instance->_projectMHandle, reinterpret_cast<float*>(stream), samples,
                           static_cast<projectm_channels>(instance->_channels));
}
#endif

float AudioCaptureImpl::CurrentAudioLevel() const
{
    return -1.0f;
}

void AudioCaptureImpl::RefreshDeviceList()
{
    // Device list is re-enumerated on next AudioDeviceList() call.
}
