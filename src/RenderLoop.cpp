#include "RenderLoop.h"

#include "FPSLimiter.h"

#include "gui/ProjectMGUI.h"

#include "notifications/DisplayToastNotification.h"
#include "notifications/PlaybackControlNotification.h"

#include <Poco/NotificationCenter.h>

#include <Poco/Util/Application.h>
#include <Poco/File.h>
#include <Poco/Path.h>

#ifdef USE_SDL3
# include <SDL3/SDL.h>
#else
# include <SDL2/SDL.h>
#endif

#include "ProjectMSDLApplication.h"

RenderLoop::RenderLoop()
    : _audioCapture(Poco::Util::Application::instance().getSubsystem<AudioCapture>())
    , _projectMWrapper(Poco::Util::Application::instance().getSubsystem<ProjectMWrapper>())
    , _sdlRenderingWindow(Poco::Util::Application::instance().getSubsystem<SDLRenderingWindow>())
    , _projectMHandle(_projectMWrapper.ProjectM())
    , _playlistHandle(_projectMWrapper.Playlist())
    , _projectMGui(Poco::Util::Application::instance().getSubsystem<ProjectMGUI>())
    , _userConfig(ProjectMSDLApplication::instance().UserConfiguration())
{
}

void RenderLoop::Run()
{
    FPSLimiter limiter;

    auto& notificationCenter{Poco::NotificationCenter::defaultCenter()};

    notificationCenter.addObserver(_quitNotificationObserver);

    _projectMWrapper.DisplayInitialPreset();

    while (!_wantsToQuit)
    {
        limiter.TargetFPS(_projectMWrapper.TargetFPS());
        limiter.StartFrame();

        PollEvents();
        CheckViewportSize();
        _audioCapture.FillBuffer();
        _projectMWrapper.RenderFrame();
        _projectMGui.Draw();

        _sdlRenderingWindow.Swap();

        limiter.EndFrame();

        // Pass projectM the actual FPS value of the last frame.
        _projectMWrapper.UpdateRealFPS(limiter.FPS());
    }

    notificationCenter.removeObserver(_quitNotificationObserver);

    projectm_playlist_set_preset_switched_event_callback(_playlistHandle, nullptr, nullptr);
}

void RenderLoop::PollEvents()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        _projectMGui.ProcessInput(event);

        switch (event.type)
        {
#ifdef USE_SDL3
            case SDL_EVENT_MOUSE_WHEEL:
#else
            case SDL_MOUSEWHEEL:
#endif
                if (!_projectMGui.WantsMouseInput())
                {
                    ScrollEvent(event.wheel);
                }
                break;

#ifdef USE_SDL3
            case SDL_EVENT_KEY_DOWN:
#else
            case SDL_KEYDOWN:
#endif
                if (!_projectMGui.WantsKeyboardInput())
                {
                    KeyEvent(event.key, true);
                }
                break;

#ifdef USE_SDL3
            case SDL_EVENT_KEY_UP:
#else
            case SDL_KEYUP:
#endif
                if (!_projectMGui.WantsKeyboardInput())
                {
                    KeyEvent(event.key, false);
                }
                break;

#ifdef USE_SDL3
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
#else
            case SDL_MOUSEBUTTONDOWN:
#endif
                if (!_projectMGui.WantsMouseInput())
                {
                    MouseDownEvent(event.button);
                }
                break;

#ifdef USE_SDL3
            case SDL_EVENT_MOUSE_BUTTON_UP:
#else
            case SDL_MOUSEBUTTONUP:
#endif
                if (!_projectMGui.WantsMouseInput())
                {
                    MouseUpEvent(event.button);
                }
                break;

#ifdef USE_SDL3
            case SDL_EVENT_DROP_FILE:
#else
            case SDL_DROPFILE:
#endif
                HandleDropFile(event);
                break;

#ifdef USE_SDL3
            case SDL_EVENT_QUIT:
#else
            case SDL_QUIT:
#endif
                _wantsToQuit = true;
                break;

#ifdef USE_SDL3
            case SDL_EVENT_AUDIO_DEVICE_ADDED:
            case SDL_EVENT_AUDIO_DEVICE_REMOVED:
                _audioCapture.RefreshDeviceList();
                break;
#endif
        }
    }
}

void RenderLoop::HandleDropFile(const SDL_Event& event)
{
#ifdef USE_SDL3
    const char* droppedFilePath = event.drop.data;
#else
    char* droppedFilePath = event.drop.file;
#endif

    bool skipToDropped = _userConfig->getBool("projectM.skipToDropped", true);
    bool droppedFolderOverride = _userConfig->getBool("projectM.droppedFolderOverride", false);

    bool shuffle = projectm_playlist_get_shuffle(_playlistHandle);
    if (shuffle && skipToDropped)
    {
        projectm_playlist_set_shuffle(_playlistHandle, false);
    }

    int index = projectm_playlist_get_position(_playlistHandle) + 1;

    do
    {
        Poco::File droppedFile(droppedFilePath);
        if (!droppedFile.isDirectory())
        {
            Poco::Path droppedFileP(droppedFilePath);
            if (!droppedFile.exists() || (droppedFileP.getExtension() != "milk" && droppedFileP.getExtension() != "prjm"))
            {
                std::string toastMessage = std::string("Invalid preset file: ") + droppedFilePath;
                Poco::NotificationCenter::defaultCenter().postNotification(new DisplayToastNotification(toastMessage));
                poco_information_f1(_logger, "%s", toastMessage);
                break;
            }

            if (projectm_playlist_insert_preset(_playlistHandle, droppedFilePath, index, true))
            {
                if (skipToDropped)
                {
                    projectm_playlist_play_next(_playlistHandle, true);
                }
                poco_information_f1(_logger, "Added preset: %s", std::string(droppedFilePath));
            }
        }
        else
        {
            if (droppedFolderOverride)
            {
                projectm_playlist_clear(_playlistHandle);
                index = 0;
            }

            uint32_t addedFilesCount = projectm_playlist_insert_path(_playlistHandle, droppedFilePath, index, true, true);
            if (addedFilesCount > 0)
            {
                std::string toastMessage = "Added " + std::to_string(addedFilesCount) + " presets from " + droppedFilePath;
                poco_information_f1(_logger, "%s", toastMessage);
                if (skipToDropped || droppedFolderOverride)
                {
                    projectm_playlist_play_next(_playlistHandle, true);
                }
                Poco::NotificationCenter::defaultCenter().postNotification(new DisplayToastNotification(toastMessage));
            }
            else
            {
                std::string toastMessage = std::string("No presets found in: ") + droppedFilePath;
                Poco::NotificationCenter::defaultCenter().postNotification(new DisplayToastNotification(toastMessage));
                poco_information_f1(_logger, "%s", toastMessage);
            }
        }
    } while (false);

    if (shuffle && skipToDropped)
    {
        projectm_playlist_set_shuffle(_playlistHandle, true);
    }

#ifndef USE_SDL3
    SDL_free(droppedFilePath);
#endif
}

void RenderLoop::CheckViewportSize()
{
    int renderWidth;
    int renderHeight;
    _sdlRenderingWindow.GetDrawableSize(renderWidth, renderHeight);

    if (renderWidth != _renderWidth || renderHeight != _renderHeight)
    {
        projectm_set_window_size(_projectMHandle, renderWidth, renderHeight);
        _renderWidth = renderWidth;
        _renderHeight = renderHeight;

        _projectMGui.UpdateFontSize();

        poco_debug_f2(_logger, "Resized rendering canvas to %?dx%?d.", renderWidth, renderHeight);
    }
}

void RenderLoop::KeyEvent(const SDL_KeyboardEvent& event, bool down)
{
#ifdef USE_SDL3
    auto keyModifier{static_cast<SDL_Keymod>(event.mod)};
    auto keyCode{event.key};
    bool modifierPressed{false};

    if (keyModifier & SDL_KMOD_LGUI || keyModifier & SDL_KMOD_RGUI || keyModifier & SDL_KMOD_LCTRL)
    {
        modifierPressed = true;
    }
#else
    auto keyModifier{static_cast<SDL_Keymod>(event.keysym.mod)};
    auto keyCode{event.keysym.sym};
    bool modifierPressed{false};

    if (keyModifier & KMOD_LGUI || keyModifier & KMOD_RGUI || keyModifier & KMOD_LCTRL)
    {
        modifierPressed = true;
    }
#endif

    // Handle modifier keys and save state for use in other methods, e.g. mouse events
    switch (keyCode)
    {
        case SDLK_LCTRL:
        case SDLK_RCTRL:
            _keyStates._ctrlPressed = down;
            break;

        case SDLK_LSHIFT:
        case SDLK_RSHIFT:
            _keyStates._shiftPressed = down;
            break;

        case SDLK_LALT:
        case SDLK_RALT:
            _keyStates._altPressed = down;
            break;

        case SDLK_LGUI:
        case SDLK_RGUI:
            _keyStates._metaPressed = down;
            break;

        default:
            break;
    }

    if (!down)
    {
        return;
    }

    switch (keyCode)
    {
#ifdef USE_SDL3
// SDL3 renamed key constants to uppercase. Remap lowercase to uppercase.
#undef SDLK_a
#define SDLK_a SDLK_A
#undef SDLK_c
#define SDLK_c SDLK_C
#undef SDLK_d
#define SDLK_d SDLK_D
#undef SDLK_f
#define SDLK_f SDLK_F
#undef SDLK_i
#define SDLK_i SDLK_I
#undef SDLK_m
#define SDLK_m SDLK_M
#undef SDLK_n
#define SDLK_n SDLK_N
#undef SDLK_p
#define SDLK_p SDLK_P
#undef SDLK_q
#define SDLK_q SDLK_Q
#undef SDLK_r
#define SDLK_r SDLK_R
#undef SDLK_y
#define SDLK_y SDLK_Y
#endif
        case SDLK_ESCAPE:
            _projectMGui.Toggle();
            _sdlRenderingWindow.ShowCursor(_projectMGui.Visible());
            break;

        case SDLK_a: {
            bool aspectCorrectionEnabled = !projectm_get_aspect_correction(_projectMHandle);
            projectm_set_aspect_correction(_projectMHandle, aspectCorrectionEnabled);
        }
        break;

        case SDLK_c:
            if (modifierPressed)
            {
                _projectMWrapper.PresetFileNameToClipboard();
            }
            break;

#ifdef _DEBUG
        case SDLK_d:
            // Write next rendered frame to file
            projectm_write_debug_image_on_next_frame(_projectMHandle, nullptr);
            break;
#endif

        case SDLK_f:
            if (modifierPressed)
            {
                _sdlRenderingWindow.ToggleFullscreen();
            }
            break;

        case SDLK_i:
            if (modifierPressed)
            {
                _audioCapture.NextAudioDevice();
            }
            break;

        case SDLK_m:
            if (modifierPressed)
            {
                _sdlRenderingWindow.NextDisplay();
                break;
            }
            break;

        case SDLK_n:
            Poco::NotificationCenter::defaultCenter().postNotification(new PlaybackControlNotification(PlaybackControlNotification::Action::NextPreset, _keyStates._shiftPressed));
            break;

        case SDLK_p:
            Poco::NotificationCenter::defaultCenter().postNotification(new PlaybackControlNotification(PlaybackControlNotification::Action::PreviousPreset, _keyStates._shiftPressed));
            break;

        case SDLK_r: {
            Poco::NotificationCenter::defaultCenter().postNotification(new PlaybackControlNotification(PlaybackControlNotification::Action::RandomPreset, _keyStates._shiftPressed));
            break;
        }

        case SDLK_q:
            if (modifierPressed)
            {
                _wantsToQuit = true;
            }
            break;

        case SDLK_y:
            Poco::NotificationCenter::defaultCenter().postNotification(new PlaybackControlNotification(PlaybackControlNotification::Action::ToggleShuffle));
            break;

        case SDLK_BACKSPACE:
            Poco::NotificationCenter::defaultCenter().postNotification(new PlaybackControlNotification(PlaybackControlNotification::Action::LastPreset, _keyStates._shiftPressed));
            break;

        case SDLK_SPACE:
            Poco::NotificationCenter::defaultCenter().postNotification(new PlaybackControlNotification(PlaybackControlNotification::Action::TogglePresetLocked));
            break;

        case SDLK_UP:
            _projectMWrapper.ChangeBeatSensitivity(0.01f);
            break;

        case SDLK_DOWN:
            _projectMWrapper.ChangeBeatSensitivity(-0.01f);
            break;
    }
}

void RenderLoop::ScrollEvent(const SDL_MouseWheelEvent& event)
{
    if (event.y > 0)
    {
        projectm_playlist_play_next(_playlistHandle, true);
    }
    else if (event.y < 0)
    {
        projectm_playlist_play_previous(_playlistHandle, true);
    }
}

void RenderLoop::MouseDownEvent(const SDL_MouseButtonEvent& event)
{
    if (_projectMGui.WantsMouseInput())
    {
        return;
    }

    switch (event.button)
    {
        case SDL_BUTTON_LEFT:
            if (!_mouseDown && _keyStates._shiftPressed)
            {
#ifdef USE_SDL3
                float x;
                float y;
#else
                int x;
                int y;
#endif
                int width;
                int height;

                _sdlRenderingWindow.GetDrawableSize(width, height);

                SDL_GetMouseState(&x, &y);

                float scaledX = (static_cast<float>(x) / static_cast<float>(width));
                float scaledY = (static_cast<float>(height - y) / static_cast<float>(height));

                projectm_touch(_projectMHandle, scaledX, scaledY, 0, PROJECTM_TOUCH_TYPE_RANDOM);
                poco_debug_f2(_logger, "Added new random waveform at %?f,%?f", static_cast<float>(x), static_cast<float>(y));

                _mouseDown = true;
            }
            break;

        case SDL_BUTTON_RIGHT:
            if (!_keyStates.AnyPressed())
            {
                _sdlRenderingWindow.ToggleFullscreen();
            }
            break;

        case SDL_BUTTON_MIDDLE:
            projectm_touch_destroy_all(_projectMHandle);
            poco_debug(_logger, "Cleared all custom waveforms.");
            break;
    }
}

void RenderLoop::MouseUpEvent(const SDL_MouseButtonEvent& event)
{
    if (event.button == SDL_BUTTON_LEFT)
    {
        _mouseDown = false;
    }
}

void RenderLoop::QuitNotificationHandler(const Poco::AutoPtr<QuitNotification>& notification)
{
    _wantsToQuit = true;
}
