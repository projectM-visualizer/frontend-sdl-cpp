#pragma once

#include "EditorPreset.h"
#include "PresetFile.h"
#include "TextEditor.h"
#include "EditorMenu.h"

#include <string>

class ProjectMGUI;
class ProjectMSDLApplication;
class ProjectMWrapper;

namespace Editor {

class PresetEditorGUI
{
public:
    explicit PresetEditorGUI(ProjectMGUI& gui);

    ~PresetEditorGUI() = default;

    /**
     * @brief Displays the preset editor screen and associated windows.
     * @param presetFile The preset file name to load and edit.
     */
    void Show(const std::string& presetFile);

    /**
     * @brief Tells the editor UI to close.
     * If there are unsaved changes, the user will be asked to save or abort the close.
     */
    void Close();

    /**
     * @brief Draws the preset editor.
     * @return true if the preset editor is visible and has been drawn, false otherwise.
     */
    bool Draw();

    /**
     * @brief Reloads the rendered preview with the current changes.
     */
    void UpdatePresetPreview();

private:
    void TakeProjectMControl();
    void ReleaseProjectMControl();

    void DrawLeftSideBar();

    void DrawPresetCompatibilitySettings();
    void DrawGeneralParameters();
    void DrawDefaultWaveformSettings();
    void DrawWaveformModeSelection();
    void DrawMotionVectorSettings();
    void DrawBorderSettings();
    void DrawShaderLossWarning() const;

    static void DrawHelpTooltip(const std::string& helpText);

    void EditCode(const std::string& code, bool isShaderCode);

    ProjectMGUI& _gui; //!< Reference to the projectM GUI instance
    ProjectMSDLApplication& _application;
    ProjectMWrapper& _projectMWrapper;

    bool _wantClose{false};
    bool _visible{false}; //!< true if the editor is visible, false if not.

    EditorMenu _menu; //!< The editor-specific main menu bar.

    std::string _loadedPresetPath; //!< The full path of the currently loaded preset. Can be empty.
    PresetFile _presetFile; //!< The raw preset data.
    EditorPreset _editorPreset; //!< The preset data in a parsed, strongly-typed container.

    TextEditor _textEditor; //!< The expression/shader code editor.
};

} // namespace Editor
