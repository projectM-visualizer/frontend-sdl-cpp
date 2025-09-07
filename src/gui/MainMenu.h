#pragma once

#include "FileChooser.h"

class ProjectMGUI;
class ProjectMWrapper;
class AudioCapture;

namespace Poco {
class NotificationCenter;
}

class MainMenu
{
public:
    MainMenu() = delete;

    explicit MainMenu(ProjectMGUI& gui);

    /**
     * @brief Draws the main menu bar.
     */
    void Draw();

private:
    void DrawFileMenu();
    void DrawPlaybackMenu();
    void DrawOptionsMenu();
    void DrawHelpMenu();

    Poco::NotificationCenter& _notificationCenter; //!< Notification center instance.
    ProjectMGUI& _gui; //!< Reference to the GUI subsystem.
    ProjectMWrapper& _projectMWrapper; //!< Reference to the projectM wrapper subsystem.
    AudioCapture& _audioCapture; //!< Reference to the audio capture subsystem.

    FileChooser _presetChooser{FileChooser::Mode::File}; //!< The file chooser dialog to select presets for editing.
};
