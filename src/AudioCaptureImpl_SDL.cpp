#include "AudioCaptureImpl_SDL.h"

#include <Poco/Util/Application.h>

#include <projectM-4/projectM.h>

AudioCaptureImpl::AudioCaptureImpl()
    : _requestedSampleCount(projectm_pcm_get_max_samples())
{
    auto targetFps = Poco::Util::Application::instance().config().getUInt("projectM.fps", 60);
    if (targetFps > 0)
    {
        _requestedSampleCount = std::min(_requestedSampleFrequency / targetFps, _requestedSampleCount);
        _requestedSampleCount = std::max(_requestedSampleCount, 300U);
    }

#ifdef USE_SDL3
    SDL_InitSubSystem(SDL_INIT_AUDIO);
#else
#ifdef SDL_HINT_AUDIO_INCLUDE_MONITORS
    SDL_SetHint(SDL_HINT_AUDIO_INCLUDE_MONITORS, "1");
#endif
    SDL_InitSubSystem(SDL_INIT_AUDIO);
#endif
}

AudioCaptureImpl::~AudioCaptureImpl()
{
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

std::map<int, std::string> AudioCaptureImpl::AudioDeviceList()
{
    std::map<int, std::string> deviceList{
        {-1, "Default capturing device"}};

#ifdef USE_SDL3
    int deviceCount = 0;
    auto deviceIds = SDL_GetAudioCaptureDevices(&deviceCount);
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

#ifdef USE_SDL3
    poco_debug_f1(_logger, "Using SDL audio driver \"%s\".", std::string(SDL_GetCurrentAudioDriver()));
#else
    poco_debug_f1(_logger, "Using SDL audio driver \"%s\".", std::string(SDL_GetCurrentAudioDriver()));
#endif

    if (OpenAudioDevice())
    {
#ifdef USE_SDL3
        SDL_ResumeAudioDevice(_currentAudioDeviceID);
#else
        SDL_PauseAudioDevice(_currentAudioDeviceID, false);
#endif
        poco_debug(_logger, "Started audio recording.");
    }
}

void AudioCaptureImpl::StopRecording()
{
    if (_currentAudioDeviceID)
    {
#ifdef USE_SDL3
        SDL_PauseAudioDevice(_currentAudioDeviceID);
#else
        SDL_PauseAudioDevice(_currentAudioDeviceID, true);
#endif
        SDL_CloseAudioDevice(_currentAudioDeviceID);
        _currentAudioDeviceID = 0;

        poco_debug(_logger, "Stopped audio recording and closed device.");
    }
}

void AudioCaptureImpl::NextAudioDevice()
{
    StopRecording();

#ifdef USE_SDL3
    int deviceCount = 0;
    SDL_GetAudioCaptureDevices(&deviceCount);
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
    SDL_GetAudioCaptureDevices(&deviceCount);
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
        auto deviceIds = SDL_GetAudioCaptureDevices(&deviceCount);
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
    return "Default capturing device";
#else
    if (_currentAudioDeviceIndex >= 0)
    {
        return SDL_GetAudioDeviceName(_currentAudioDeviceIndex, true);
    }
    else
    {
        return "Default capturing device";
    }
#endif
}

bool AudioCaptureImpl::OpenAudioDevice()
{
#ifdef USE_SDL3
    SDL_AudioSpec spec{};
    spec.freq = _requestedSampleFrequency;
    spec.format = SDL_AUDIO_F32;
    spec.channels = 2;

    // In SDL3, we pass 0 to use the default device.
    SDL_AudioDeviceID deviceId = 0;
    if (_currentAudioDeviceIndex >= 0)
    {
        int deviceCount = 0;
        auto deviceIds = SDL_GetAudioCaptureDevices(&deviceCount);
        if (deviceIds && _currentAudioDeviceIndex < deviceCount)
        {
            deviceId = deviceIds[_currentAudioDeviceIndex];
        }
        if (deviceIds)
        {
            SDL_free(deviceIds);
        }
    }

    _currentAudioDeviceID = SDL_OpenAudioDevice(deviceId, &spec);
    if (_currentAudioDeviceID == 0)
    {
        poco_error_f2(_logger, R"(Failed to open audio device (ID %?d): %s)",
                      _currentAudioDeviceIndex,
                      std::string(SDL_GetError()));
        return false;
    }

    _channels = spec.channels;

    // Get the actual device name for logging
    auto deviceName = SDL_GetAudioDeviceName(_currentAudioDeviceID);

    poco_information_f4(_logger, R"(Opened audio recording device "%s" (ID %?d) with %?d channels at %?d Hz.)",
                        std::string(deviceName != nullptr ? deviceName : "System default capturing device"),
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
                      std::string(deviceName != nullptr ? deviceName : "System default capturing device"),
                      _currentAudioDeviceIndex,
                      std::string(SDL_GetError()));
        return false;
    }

    _channels = actualSpecs.channels;

    poco_information_f4(_logger, R"(Opened audio recording device "%s" (ID %?d) with %?d channels at %?d Hz.)",
                        std::string(deviceName != nullptr ? deviceName : "System default capturing device"),
                        _currentAudioDeviceIndex,
                        actualSpecs.channels,
                        actualSpecs.freq);

    return true;
#endif
}

#ifdef USE_SDL3
void AudioCaptureImpl::FillBuffer()
{
    if (!_currentAudioDeviceID || !_projectMHandle)
    {
        return;
    }

    // Poll available audio data from the capture device
    int available = SDL_GetAudioDeviceQueuedSize(_currentAudioDeviceID);
    if (available <= 0)
    {
        return;
    }

    // Read up to one frame's worth of samples
    unsigned int requestedSamples = _requestedSampleCount;
    std::vector<float> buffer(requestedSamples * _channels);

    unsigned int bytesRead = SDL_DequeueAudio(_currentAudioDeviceID, buffer.data(),
                                               requestedSamples * _channels * sizeof(float));
    if (bytesRead == 0)
    {
        return;
    }

    unsigned int samples = bytesRead / sizeof(float) / _channels;
    projectm_pcm_add_float(_projectMHandle, buffer.data(), samples,
                           static_cast<projectm_channels>(_channels));
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
