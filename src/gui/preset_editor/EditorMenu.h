#pragma once

namespace Poco {
class NotificationCenter;
}

namespace Editor {

class PresetEditorGUI;

class EditorMenu
{
public:
    EditorMenu() = delete;

    explicit EditorMenu(PresetEditorGUI& gui);

    /**
     * @brief Draws the editor menu bar.
     */
    void Draw();

private:
    void DrawFileMenu();
    void DrawHelpMenu();

    Poco::NotificationCenter& _notificationCenter; //!< Notification center instance.

    PresetEditorGUI& _presetEditorGUI;
};


} // namespace Editor
