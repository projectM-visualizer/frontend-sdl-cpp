#include "AudioCaptureImpl_CoreAudioTap.h"

#include <projectM-4/projectM.h>

#import <CoreAudio/CoreAudio.h>
#import <Foundation/Foundation.h>

// CATapDescription and the process-tap C API live in the CoreAudio umbrella framework
// on macOS 14.4+. The Objective-C class is declared in <CoreAudio/CATapDescription.h>;
// the AudioHardwareCreate/DestroyProcessTap C functions live in AudioHardwareTapping.h,
// which the CoreAudio umbrella header does not pull in automatically.
#import <CoreAudio/AudioHardwareTapping.h>
#import <CoreAudio/CATapDescription.h>

CoreAudioTapCapture::CoreAudioTapCapture() = default;

CoreAudioTapCapture::~CoreAudioTapCapture()
{
    StopTap();
}

std::map<int, std::string> CoreAudioTapCapture::AudioDeviceList()
{
    // v1: a single synthetic entry for whole-system audio. Kept as a map so per-process
    // entries can be added later without changing the AudioCapture proxy.
    return {{-1, _systemAudioDeviceName}};
}

void CoreAudioTapCapture::StartRecording(projectm* projectMHandle, int audioDeviceIndex)
{
    _projectMHandle = projectMHandle;
    _currentAudioDeviceIndex = -1; // Only one source in v1.

    if (StartTap())
    {
        poco_information(_logger, "Started system audio capture via Core Audio tap.");
    }
    else
    {
        poco_error(_logger, "Failed to start system audio capture. Visualizer will idle until audio is available.");
    }
}

void CoreAudioTapCapture::StopRecording()
{
    StopTap();
}

void CoreAudioTapCapture::NextAudioDevice()
{
    // v1 exposes a single system-audio source, so there is nothing to switch to.
}

void CoreAudioTapCapture::AudioDeviceIndex(int index)
{
    // v1 exposes a single system-audio source; ignore any other index.
    _currentAudioDeviceIndex = -1;
}

int CoreAudioTapCapture::AudioDeviceIndex() const
{
    return _currentAudioDeviceIndex;
}

std::string CoreAudioTapCapture::AudioDeviceName() const
{
    return _systemAudioDeviceName;
}

std::string CoreAudioTapCapture::DefaultOutputDeviceUID() const
{
    AudioObjectPropertyAddress defaultDeviceAddress{
        kAudioHardwarePropertyDefaultOutputDevice,
        kAudioObjectPropertyScopeGlobal,
        kAudioObjectPropertyElementMain};

    AudioObjectID defaultDeviceID{kAudioObjectUnknown};
    UInt32 dataSize = sizeof(defaultDeviceID);
    OSStatus status = AudioObjectGetPropertyData(kAudioObjectSystemObject, &defaultDeviceAddress,
                                                 0, nullptr, &dataSize, &defaultDeviceID);
    if (status != noErr || defaultDeviceID == kAudioObjectUnknown)
    {
        poco_error_f1(_logger, "Could not resolve default output device: OSStatus %?d", static_cast<int>(status));
        return {};
    }

    AudioObjectPropertyAddress uidAddress{
        kAudioDevicePropertyDeviceUID,
        kAudioObjectPropertyScopeGlobal,
        kAudioObjectPropertyElementMain};

    CFStringRef deviceUID = nullptr;
    dataSize = sizeof(deviceUID);
    status = AudioObjectGetPropertyData(defaultDeviceID, &uidAddress, 0, nullptr, &dataSize, &deviceUID);
    if (status != noErr || deviceUID == nullptr)
    {
        poco_error_f1(_logger, "Could not read default output device UID: OSStatus %?d", static_cast<int>(status));
        return {};
    }

    std::string uid([(__bridge NSString*) deviceUID UTF8String]);
    CFRelease(deviceUID);
    return uid;
}

bool CoreAudioTapCapture::StartTap()
{
    // The process-tap API is only available at runtime on macOS 14.4+. Guarding the
    // call sites lets the symbols weak-link and avoids a hard crash on older systems;
    // there we simply report and capture nothing rather than failing the whole app.
    if (@available(macOS 14.4, *))
    {
        @autoreleasepool
        {
            // 1. Create a global tap that captures output from all processes.
            CATapDescription* tapDescription =
                [[CATapDescription alloc] initStereoGlobalTapButExcludeProcesses:@[]];
            tapDescription.name = @"projectMSDL System Audio Tap";
            tapDescription.privateTap = YES;
            tapDescription.muteBehavior = CATapUnmuted;

            OSStatus status = AudioHardwareCreateProcessTap(tapDescription, &_tapObjectID);
            if (status != noErr || _tapObjectID == kAudioObjectUnknown)
            {
                poco_error_f1(_logger, "AudioHardwareCreateProcessTap failed: OSStatus %?d", static_cast<int>(status));
                _tapObjectID = kAudioObjectUnknown;
                return false;
            }

            NSString* tapUUID = tapDescription.UUID.UUIDString;

            // 2. Create a private aggregate device wrapping the tap. It needs a real output
            //    device as its main sub-device; the tap is attached via the tap list.
            std::string mainDeviceUID = DefaultOutputDeviceUID();
            if (mainDeviceUID.empty())
            {
                StopTap();
                return false;
            }

            NSString* mainUID = [NSString stringWithUTF8String:mainDeviceUID.c_str()];

            NSDictionary* aggregateDescription = @{
                @(kAudioAggregateDeviceNameKey): @"projectMSDL System Audio",
                @(kAudioAggregateDeviceUIDKey): @"org.projectm-visualizer.projectmsdl.systemaudio",
                @(kAudioAggregateDeviceMainSubDeviceKey): mainUID,
                @(kAudioAggregateDeviceIsPrivateKey): @YES,
                @(kAudioAggregateDeviceIsStackedKey): @NO,
                @(kAudioAggregateDeviceTapAutoStartKey): @YES,
                @(kAudioAggregateDeviceTapListKey): @[
                    @{
                        @(kAudioSubTapUIDKey): tapUUID,
                        @(kAudioSubTapDriftCompensationKey): @YES,
                    },
                ],
            };

            status = AudioHardwareCreateAggregateDevice((__bridge CFDictionaryRef) aggregateDescription,
                                                        &_aggregateDeviceID);
            if (status != noErr || _aggregateDeviceID == kAudioObjectUnknown)
            {
                poco_error_f1(_logger, "AudioHardwareCreateAggregateDevice failed: OSStatus %?d", static_cast<int>(status));
                _aggregateDeviceID = kAudioObjectUnknown;
                StopTap();
                return false;
            }

            // 3. Determine the captured stream format (sample rate + channels).
            AudioObjectPropertyAddress formatAddress{
                kAudioDevicePropertyStreamFormat,
                kAudioObjectPropertyScopeInput,
                kAudioObjectPropertyElementMain};

            AudioStreamBasicDescription streamFormat{};
            UInt32 dataSize = sizeof(streamFormat);
            status = AudioObjectGetPropertyData(_aggregateDeviceID, &formatAddress, 0, nullptr,
                                                &dataSize, &streamFormat);
            if (status != noErr || streamFormat.mChannelsPerFrame == 0)
            {
                poco_error_f1(_logger, "Could not read aggregate device stream format: OSStatus %?d",
                              static_cast<int>(status));
                StopTap();
                return false;
            }

            _channels = streamFormat.mChannelsPerFrame;

            // 4. Install the IO proc. Runs on Core Audio's realtime thread; keep it allocation-
            //    and lock-free. Core Audio delivers deinterleaved float buffers (one buffer per
            //    channel) for tap aggregates, so forward the first buffer's frames to projectM.
            projectm* handle = _projectMHandle;
            uint32_t channels = _channels;

            status = AudioDeviceCreateIOProcIDWithBlock(
                &_ioProcID, _aggregateDeviceID, nullptr,
                ^(const AudioTimeStamp* /*inNow*/, const AudioBufferList* inInputData,
                  const AudioTimeStamp* /*inInputTime*/, AudioBufferList* /*outOutputData*/,
                  const AudioTimeStamp* /*inOutputTime*/) {
                  if (handle == nullptr || inInputData == nullptr || inInputData->mNumberBuffers == 0)
                  {
                      return;
                  }

                  const AudioBuffer& buffer = inInputData->mBuffers[0];
                  if (buffer.mData == nullptr || buffer.mDataByteSize == 0)
                  {
                      return;
                  }

                  auto* samples = static_cast<float*>(buffer.mData);
                  // mNumberChannels reflects the interleaving of this buffer.
                  uint32_t bufferChannels = buffer.mNumberChannels > 0 ? buffer.mNumberChannels : channels;
                  unsigned int frameCount = buffer.mDataByteSize / sizeof(float) / bufferChannels;

                  projectm_pcm_add_float(handle, samples, frameCount,
                                         static_cast<projectm_channels>(bufferChannels));
                });

            if (status != noErr || _ioProcID == nullptr)
            {
                poco_error_f1(_logger, "AudioDeviceCreateIOProcIDWithBlock failed: OSStatus %?d",
                              static_cast<int>(status));
                _ioProcID = nullptr;
                StopTap();
                return false;
            }

            // 5. Start the aggregate device.
            status = AudioDeviceStart(_aggregateDeviceID, _ioProcID);
            if (status != noErr)
            {
                poco_error_f1(_logger, "AudioDeviceStart failed: OSStatus %?d", static_cast<int>(status));
                StopTap();
                return false;
            }

            poco_information_f2(_logger, "System audio tap running (%hu channels, %.0f Hz).",
                                static_cast<unsigned short>(_channels), streamFormat.mSampleRate);
            return true;
        }
    }
    else
    {
        poco_error(_logger, "System audio capture requires macOS 14.4 or newer; no audio will be captured.");
        return false;
    }
}

void CoreAudioTapCapture::StopTap()
{
    // Tear down in reverse order of creation. Guard each handle so a partially-created
    // setup cleans up correctly. Both the aggregate device and the tap must be destroyed
    // together to avoid the documented "all-zero samples" state on restart.
    if (_aggregateDeviceID != kAudioObjectUnknown && _ioProcID != nullptr)
    {
        AudioDeviceStop(_aggregateDeviceID, _ioProcID);
    }

    if (_aggregateDeviceID != kAudioObjectUnknown && _ioProcID != nullptr)
    {
        AudioDeviceDestroyIOProcID(_aggregateDeviceID, _ioProcID);
        _ioProcID = nullptr;
    }

    if (_aggregateDeviceID != kAudioObjectUnknown)
    {
        AudioHardwareDestroyAggregateDevice(_aggregateDeviceID);
        _aggregateDeviceID = kAudioObjectUnknown;
    }

    if (_tapObjectID != kAudioObjectUnknown)
    {
        AudioHardwareDestroyProcessTap(_tapObjectID);
        _tapObjectID = kAudioObjectUnknown;
    }
}
