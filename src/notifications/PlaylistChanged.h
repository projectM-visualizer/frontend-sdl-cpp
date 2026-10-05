#pragma once

#include <Poco/Notification.h>

class PresetPlaylist;

namespace Notification {

/**
 * @brief Notifies observers that a playlist has been changed.
 *
 * This notification will be sent each time at least one playlist item changes
 * in some way, e.g. new item added, item removed or if the playlist has been fully replaced.
 * This allows to cache items/filenames in dialogs etc. without the need to query those each frame
 * from the playlist manager.
 *
 * This notification will be sent by any playlist object, so the subscriber has to decide
 * if it's the correct one.
 */
class PlaylistChanged : public Poco::Notification
{
public:
    std::string name() const override;

    PlaylistChanged() = delete;

    explicit PlaylistChanged(const PresetPlaylist& playlist);

    /**
     * Returns a reference the to playlist which was changed.
     * @note Don't store the reference, as the playlist passed in the notification
     *       might be a temporary object!
     * @return A reference the to playlist which was changed.
     */
    const PresetPlaylist& Playlist() const
    {
        return _playlist;
    }

private:
    const PresetPlaylist& _playlist;
};

} // namespace Notification
