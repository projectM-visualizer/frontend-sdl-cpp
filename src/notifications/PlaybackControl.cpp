#include "PlaybackControl.h"

namespace Notification {

PlaybackControl::PlaybackControl(PlaybackControl::Action action, bool smoothTransition)
    : _action(action)
    , _smoothTransition(smoothTransition)
{
}

std::string PlaybackControl::name() const
{
    return "PlaybackControlNotification";
}

PlaybackControl::Action PlaybackControl::ControlAction() const
{
    return _action;
}

bool PlaybackControl::SmoothTransition() const
{
    return _smoothTransition;
}

} // namespace Notification
