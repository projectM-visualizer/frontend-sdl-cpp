#pragma once

#include <string>

class ProjectMGUI;
class ProjectMWrapper;
class AudioCapture;
class PlaylistManager;

namespace Poco {
class NotificationCenter;
}

class MainMenu
{
public:
    MainMenu() = delete;

    explicit MainMenu(ProjectMGUI& gui);

    /**
     * @brief Draws the main menu bar and any open popups.
     */
    void Draw();

private:
    /**
     * @brief Draws the Playlists menu and its submenus.
     */
    void DrawPlaylistsMenu();

    /**
     * @brief Draws the "Save Playlist As..." popup.
     */
    void DrawSavePlaylistPopup();

    /**
     * @brief Draws the "New Empty Playlist" popup.
     */
    void DrawNewPlaylistPopup();

    /**
     * @brief Draws the "Manage Playlists" popup with a table of saved playlists.
     */
    void DrawManagePlaylistsPopup();

    Poco::NotificationCenter& _notificationCenter; //!< Notification center instance.
    ProjectMGUI& _gui; //!< Reference to the GUI subsystem.
    ProjectMWrapper& _projectMWrapper; //!< Reference to the projectM wrapper subsystem.
    AudioCapture& _audioCapture; //!< Reference to the audio capture subsystem.
    PlaylistManager& _playlistManager; //!< Reference to the playlist manager subsystem.

    // Popup state
    bool _showSavePlaylistPopup{false};
    bool _showNewPlaylistPopup{false};
    bool _showManagePlaylistsPopup{false};

    // Text input buffer for playlist name entry
    char _playlistNameBuffer[256]{};
};
