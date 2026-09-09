#include "PlaylistChanged.h"

namespace Notification {

std::string PlaylistChanged::name() const
{
    return "PlaylistChangedNotification";
}

PlaylistChanged::PlaylistChanged(const PresetPlaylist& playlist)
    : _playlist(playlist)
{
}

} // namespace Notification
