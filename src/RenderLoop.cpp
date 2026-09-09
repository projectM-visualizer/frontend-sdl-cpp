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

            case SDL_DROPFILE: {
                char* droppedFilePath = event.drop.file;

                // first we want to get the config settings that are relevant ehre
                // namely skipToDropped and droppedFolderOverride
                // we can get them from the projectMWrapper, in the _projectMConfigView available on it
                bool skipToDropped = _userConfig->getBool("projectM.skipToDropped", true);
                bool droppedFolderOverride = _userConfig->getBool("projectM.droppedFolderOverride", false);

                bool shuffle = _projectMWrapper.Playlist().ShuffleEnabled();
                if (shuffle && skipToDropped)
                {
                    // if shuffle is enabled, we disable it temporarily, so the dropped preset is played next
                    // if skipToDropped is false, we also keep shuffle enabled, as it doesn't matter since the current preset is unaffected
                    _projectMWrapper.Playlist().ShuffleEnabled(false);
                }

                auto insertIndex = _projectMWrapper.Playlist().CurrentIndex() + 1;

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
                            Poco::NotificationCenter::defaultCenter().postNotification(new Notification::DisplayToast(toastMessage));
                            poco_information_f1(_logger, "%s", toastMessage);
                            break; // exit the block and go to the shuffle check
                        }

                        if (_projectMWrapper.Playlist().InsertItem(PresetPlaylist::Item(droppedFileP.toString()), insertIndex, true))
                        {
                            if (skipToDropped)
                            {
                                _projectMWrapper.Playlist().Next(true);
                            }
                            poco_information_f1(_logger, "Added preset: %s", std::string(droppedFilePath));
                            // no need to toast single presets, as its obvious if a preset was loaded.
                        }
                    }
                    else
                    {
                        // handle dropped directory
                        _projectMWrapper.Playlist().BeginBatchEdit();

                        // if droppedFolderOverride is enabled, we clear the playlist first
                        // current edge case: if the dropped directory is invalid or contains no presets, then it still clears the playlist
                        if (droppedFolderOverride)
                        {
                            _projectMWrapper.Playlist().Clear();
                            insertIndex = 0;
                        }

                        uint32_t addedFilesCount = _projectMWrapper.Playlist().InsertPath(droppedFilePath, insertIndex, true, true);
                        if (addedFilesCount > 0)
                        {
                            std::string toastMessage = "Added " + std::to_string(addedFilesCount) + " presets from " + droppedFilePath;
                            poco_information_f1(_logger, "%s", toastMessage);
                            if (skipToDropped || droppedFolderOverride)
                            {
                                // if skip to dropped is true, or if a folder was dropped and it overrode the playlist, we skip to the next preset
                                _projectMWrapper.Playlist().Next(true);
                            }
                            Poco::NotificationCenter::defaultCenter().postNotification(new Notification::DisplayToast(toastMessage));
                        }
                        else
                        {
                            std::string toastMessage = std::string("No presets found in: ") + droppedFilePath;
                            Poco::NotificationCenter::defaultCenter().postNotification(new Notification::DisplayToast(toastMessage));
                            poco_information_f1(_logger, "%s", toastMessage);
                        }

                        _projectMWrapper.Playlist().EndBatchEdit();
                    }
                } while (false);

                if (shuffle && skipToDropped)
                {
                    _projectMWrapper.Playlist().ShuffleEnabled(true);
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
            _projectMGui.ShowPresetChooser();
            break;

        case SDLK_n:
            Poco::NotificationCenter::defaultCenter().postNotification(new Notification::PlaybackControl(Notification::PlaybackControl::Action::NextPreset, _keyStates._shiftPressed));
            break;

        case SDLK_p:
            Poco::NotificationCenter::defaultCenter().postNotification(new Notification::PlaybackControl(Notification::PlaybackControl::Action::PreviousPreset, _keyStates._shiftPressed));
            break;

        case SDLK_r: {
            Poco::NotificationCenter::defaultCenter().postNotification(new Notification::PlaybackControl(Notification::PlaybackControl::Action::RandomPreset, _keyStates._shiftPressed));
            break;
        }

        case SDLK_q:
            if (modifierPressed)
            {
                _wantsToQuit = true;
            }
            break;

        case SDLK_y:
            Poco::NotificationCenter::defaultCenter().postNotification(new Notification::PlaybackControl(Notification::PlaybackControl::Action::ToggleShuffle));
            break;

        case SDLK_BACKSPACE:
            Poco::NotificationCenter::defaultCenter().postNotification(new Notification::PlaybackControl(Notification::PlaybackControl::Action::LastPreset, _keyStates._shiftPressed));
            break;

        case SDLK_SPACE:
            Poco::NotificationCenter::defaultCenter().postNotification(new Notification::PlaybackControl(Notification::PlaybackControl::Action::TogglePresetLocked));
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
        _projectMWrapper.Playlist().Next(true);
    }
    // Wheel down is negative
    else if (event.y < 0)
    {
        _projectMWrapper.Playlist().Previous(true);
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

void RenderLoop::QuitNotificationHandler(const Poco::AutoPtr<Notification::Quit>& notification)
{
    _wantsToQuit = true;
}
