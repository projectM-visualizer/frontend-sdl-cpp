#pragma once

#include <Poco/Notification.h>

namespace Notification {

/**
 * @brief Navigates the playlist and toggles playback modes.
 */
class PlaybackControl : public Poco::Notification
{
public:
    enum class Action
    {
        NextPreset,
        PreviousPreset,
        LastPreset,
        RandomPreset,
        ToggleShuffle,
        TogglePresetLocked
    };

    explicit PlaybackControl(Action action, bool smoothTransition = false);

    std::string name() const override;

    Action ControlAction() const;

    bool SmoothTransition() const;

private:
    Action _action;
    bool _smoothTransition{};
};

} // namespace Notification
