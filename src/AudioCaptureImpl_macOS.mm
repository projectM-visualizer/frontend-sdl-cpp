#include "AudioCaptureImpl_macOS.h"

#include "AudioCaptureImpl_CoreAudioTap.h"

// The SDL backend is the shared cross-platform microphone capture. On macOS it is
// compiled a second time under a distinct class name (see src/CMakeLists.txt) so it
// can coexist with the Core Audio tap in the same binary. Pull in its declaration
// here under that same name.
#define AudioCaptureImpl SdlAudioCapture
#include "AudioCaptureImpl_SDL.h"
#undef AudioCaptureImpl

AudioCaptureImpl::AudioCaptureImpl()
{
    // The Core Audio process-tap API landed in macOS 14.4. On 14.4+ we tap system
    // output directly; below that we fall back to the SDL microphone backend so the
    // visualizer still reacts to a (user-selectable) input device.
    if (@available(macOS 14.4, *))
    {
        poco_information(_logger, "macOS 14.4 or newer: capturing system audio via Core Audio tap.");
        _tap = new CoreAudioTapCapture();
    }
    else
    {
        poco_information(_logger, "macOS older than 14.4: falling back to SDL microphone capture.");
        _sdl = new SdlAudioCapture();
    }
}

AudioCaptureImpl::~AudioCaptureImpl()
{
    delete _tap;
    delete _sdl;
}

std::map<int, std::string> AudioCaptureImpl::AudioDeviceList()
{
    if (_tap)
    {
        return _tap->AudioDeviceList();
    }
    return _sdl->AudioDeviceList();
}

void AudioCaptureImpl::StartRecording(projectm* projectMHandle, int audioDeviceIndex)
{
    if (_tap)
    {
        _tap->StartRecording(projectMHandle, audioDeviceIndex);
        return;
    }
    _sdl->StartRecording(projectMHandle, audioDeviceIndex);
}

void AudioCaptureImpl::StopRecording()
{
    if (_tap)
    {
        _tap->StopRecording();
        return;
    }
    _sdl->StopRecording();
}

void AudioCaptureImpl::NextAudioDevice()
{
    if (_tap)
    {
        _tap->NextAudioDevice();
        return;
    }
    _sdl->NextAudioDevice();
}

void AudioCaptureImpl::AudioDeviceIndex(int index)
{
    if (_tap)
    {
        _tap->AudioDeviceIndex(index);
        return;
    }
    _sdl->AudioDeviceIndex(index);
}

int AudioCaptureImpl::AudioDeviceIndex() const
{
    if (_tap)
    {
        return _tap->AudioDeviceIndex();
    }
    return _sdl->AudioDeviceIndex();
}

std::string AudioCaptureImpl::AudioDeviceName() const
{
    if (_tap)
    {
        return _tap->AudioDeviceName();
    }
    return _sdl->AudioDeviceName();
}

void AudioCaptureImpl::FillBuffer()
{
    if (_tap)
    {
        _tap->FillBuffer();
        return;
    }
    _sdl->FillBuffer();
}
