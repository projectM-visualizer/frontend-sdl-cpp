#include "RenderLoop.h"

#include "FPSLimiter.h"

#include "gui/ProjectMGUI.h"

#include <Poco/NotificationCenter.h>

#include <Poco/Util/Application.h>

#include <SDL2/SDL.h>

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
            case SDL_MOUSEWHEEL:

                if (!_projectMGui.WantsMouseInput())
                {
                    ScrollEvent(event.wheel);
                }

                break;

            case SDL_KEYDOWN:
                if (!_projectMGui.WantsKeyboardInput())
                {
                    KeyEvent(event.key, true);
                }
                break;

            case SDL_KEYUP:
                if (!_projectMGui.WantsKeyboardInput())
                {
                    KeyEvent(event.key, false);
                }
                break;

            case SDL_MOUSEBUTTONDOWN:
                if (!_projectMGui.WantsMouseInput())
                {
                    MouseDownEvent(event.button);
                }

                break;

            case SDL_MOUSEBUTTONUP:
                if (!_projectMGui.WantsMouseInput())
                {
                    MouseUpEvent(event.button);
                }

                break;

            case SDL_CONTROLLERDEVICEADDED:
                poco_debug(_logger, "Controller added event received");
                _sdlRenderingWindow.ControllerAdd(event.cdevice.which);

                break;

            case SDL_CONTROLLERDEVICEREMOVED:
                poco_debug(_logger, "Controller remove event received");
                _sdlRenderingWindow.ControllerRemove(event.cdevice.which);

                break;

            case SDL_CONTROLLERBUTTONDOWN:
                ControllerDownEvent(event);

                break;

            case SDL_CONTROLLERBUTTONUP:
                ControllerUpEvent(event);

                break;

            case SDL_DROPFILE: {
                char* droppedFilePath = event.drop.file;

                // first we want to get the config settings that are relevant ehre
                // namely skipToDropped and droppedFolderOverride
                // we can get them from the projectMWrapper, in the _projectMConfigView available on it
                bool skipToDropped = _userConfig->getBool("projectM.skipToDropped", true);
                bool droppedFolderOverride = _userConfig->getBool("projectM.droppedFolderOverride", false);


                bool shuffle = projectm_playlist_get_shuffle(_playlistHandle);
                if (shuffle && skipToDropped)
                {
                    // if shuffle is enabled, we disable it temporarily, so the dropped preset is played next
                    // if skipToDropped is false, we also keep shuffle enabled, as it doesn't matter since the current preset is unaffected
                    projectm_playlist_set_shuffle(_playlistHandle, false);
                }

                int index = projectm_playlist_get_position(_playlistHandle) + 1;

                do
                {
                    Poco::File droppedFile(droppedFilePath);
                    if (!droppedFile.isDirectory())
                    {
                        // handle dropped preset file
                        Poco::Path droppedFileP(droppedFilePath);
                        if (!droppedFile.exists() || (droppedFileP.getExtension() != "milk" && droppedFileP.getExtension() != "prjm"))
                        {
                            std::string toastMessage = std::string("Invalid preset file: ") + droppedFilePath;
                            Poco::NotificationCenter::defaultCenter().postNotification(new DisplayToastNotification(toastMessage));
                            poco_information_f1(_logger, "%s", toastMessage);
                            break; // exit the block and go to the shuffle check
                        }

                        if (projectm_playlist_insert_preset(_playlistHandle, droppedFilePath, index, true))
                        {
                            if (skipToDropped)
                            {
                                projectm_playlist_play_next(_playlistHandle, true);
                            }
                            poco_information_f1(_logger, "Added preset: %s", std::string(droppedFilePath));
                            // no need to toast single presets, as its obvious if a preset was loaded.
                        }
                    }
                    else
                    {
                        // handle dropped directory

                        // if droppedFolderOverride is enabled, we clear the playlist first
                        // current edge case: if the dropped directory is invalid or contains no presets, then it still clears the playlist
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
                                // if skip to dropped is true, or if a folder was dropped and it overrode the playlist, we skip to the next preset
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

                SDL_free(droppedFilePath);
                break;
            }


            case SDL_QUIT:
                _wantsToQuit = true;
                break;
        }
    }
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
    auto keyModifier{static_cast<SDL_Keymod>(event.keysym.mod)};
    auto keyCode{event.keysym.sym};
    bool modifierPressed{false};

    if (keyModifier & KMOD_LGUI || keyModifier & KMOD_RGUI || keyModifier & KMOD_LCTRL)
    {
        modifierPressed = true;
    }

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
            // Increase beat sensitivity
            _projectMWrapper.ChangeBeatSensitivity(0.01f);
            break;

        case SDLK_DOWN:
            // Decrease beat sensitivity
            _projectMWrapper.ChangeBeatSensitivity(-0.01f);
            break;
    }
}

void RenderLoop::ScrollEvent(const SDL_MouseWheelEvent& event)
{
    // Wheel up is positive
    if (event.y > 0)
    {
        projectm_playlist_play_next(_playlistHandle, true);
    }
    // Wheel down is negative
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
                // ToDo: Improve this to differentiate between single click (add waveform) and drag (move waveform).
                int x;
                int y;
                int width;
                int height;

                _sdlRenderingWindow.GetDrawableSize(width, height);

                SDL_GetMouseState(&x, &y);

                // Scale those coordinates. libProjectM uses a scale of 0..1 instead of absolute pixel coordinates.
                float scaledX = (static_cast<float>(x) / static_cast<float>(width));
                float scaledY = (static_cast<float>(height - y) / static_cast<float>(height));

                // Add a new waveform.
                projectm_touch(_projectMHandle, scaledX, scaledY, 0, PROJECTM_TOUCH_TYPE_RANDOM);
                poco_debug_f2(_logger, "Added new random waveform at %?d,%?d", x, y);

                _mouseDown = true;
            }
            break;

        case SDL_BUTTON_RIGHT:
            _sdlRenderingWindow.ToggleFullscreen();
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

void RenderLoop::ControllerDownEvent(const SDL_Event& event)
{
    if (!_sdlRenderingWindow.ControllerIsOurs(event.cdevice.which) )
    {
        return;
    }

    switch (event.cbutton.button)
    {
        case SDL_CONTROLLER_BUTTON_A:
            _sdlRenderingWindow.ToggleFullscreen();
            poco_debug(_logger, "A pressed!");
            break;

        case SDL_CONTROLLER_BUTTON_B:
            Poco::NotificationCenter::defaultCenter().postNotification(new PlaybackControlNotification(PlaybackControlNotification::Action::RandomPreset, _keyStates._shiftPressed));
            poco_debug(_logger, "B pressed!");
            break;

        case SDL_CONTROLLER_BUTTON_X:
            Poco::NotificationCenter::defaultCenter().postNotification(new PlaybackControlNotification(PlaybackControlNotification::Action::TogglePresetLocked));
            poco_debug(_logger, "X pressed!");
            break;

        case SDL_CONTROLLER_BUTTON_Y:
            Poco::NotificationCenter::defaultCenter().postNotification(new PlaybackControlNotification(PlaybackControlNotification::Action::ToggleShuffle));
            poco_debug(_logger, "Y pressed!");
            break;

        case SDL_CONTROLLER_BUTTON_BACK:
            _wantsToQuit = true;
            poco_debug(_logger, "Back pressed!");
            break;

        case SDL_CONTROLLER_BUTTON_GUIDE:
            Poco::NotificationCenter::defaultCenter().postNotification(new PlaybackControlNotification(PlaybackControlNotification::Action::LastPreset, _keyStates._shiftPressed));
            poco_debug(_logger, "Guide pressed!");
            break;

        case SDL_CONTROLLER_BUTTON_START:
            _projectMGui.Toggle();
            _sdlRenderingWindow.ShowCursor(_projectMGui.Visible());
            poco_debug(_logger, "Start pressed!");
            break;

        case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
            _audioCapture.NextAudioDevice();
            poco_debug(_logger, "Shoulder left pressed!");
            break;

        case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
            _sdlRenderingWindow.NextDisplay();
            poco_debug(_logger, "Shoulder right pressed!");
            break;

        case SDL_CONTROLLER_BUTTON_DPAD_UP:
            // Increase beat sensitivity
            _projectMWrapper.ChangeBeatSensitivity(0.05f);
            poco_debug(_logger, "DPad up pressed!");
            break;

        case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
            // Decrease beat sensitivity
            _projectMWrapper.ChangeBeatSensitivity(-0.05f);
            poco_debug(_logger, "DPad down pressed!");
            break;

        case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
            Poco::NotificationCenter::defaultCenter().postNotification(new PlaybackControlNotification(PlaybackControlNotification::Action::PreviousPreset, _keyStates._shiftPressed));
            poco_debug(_logger, "DPad left pressed!");
            break;

        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
            Poco::NotificationCenter::defaultCenter().postNotification(new PlaybackControlNotification(PlaybackControlNotification::Action::NextPreset, _keyStates._shiftPressed));
            poco_debug(_logger, "DPad right pressed!");
            break;
    }
}

void RenderLoop::ControllerUpEvent(const SDL_Event& event)
{

}

void RenderLoop::QuitNotificationHandler(const Poco::AutoPtr<QuitNotification>& notification)
{
    _wantsToQuit = true;
}
