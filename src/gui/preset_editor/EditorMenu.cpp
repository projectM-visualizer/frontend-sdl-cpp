#include "EditorMenu.h"

#include "PresetEditorGUI.h"

#include "gui/SystemBrowser.h"

#include "notifications/QuitNotification.h"

#include <Poco/NotificationCenter.h>

namespace Editor {

EditorMenu::EditorMenu(PresetEditorGUI& gui)
    : _notificationCenter(Poco::NotificationCenter::defaultCenter())
    , _presetEditorGUI(gui)
{
}

void EditorMenu::Draw()
{
    if (ImGui::BeginMainMenuBar())
    {
        DrawFileMenu();

        ImGui::Separator();
        if (ImGui::Button("Update Preset Preview"))
        {
            _presetEditorGUI.UpdatePresetPreview();
        }
        ImGui::Separator();

        DrawHelpMenu();

        ImGui::EndMainMenuBar();
    }
}

void EditorMenu::DrawFileMenu()
{
    if (ImGui::BeginMenu("File"))
    {
        if (ImGui::MenuItem("New Preset"))
        {
            _presetEditorGUI.Show("");
        }

        if (ImGui::MenuItem("Open Preset..."))
        {
        }

        if (ImGui::MenuItem("Save Preset"))
        {
        }

        if (ImGui::MenuItem("Save Preset As..."))
        {
        }

        ImGui::Separator();

        if (ImGui::MenuItem("Exit Preset Editor"))
        {
            _presetEditorGUI.Close();
        }

        ImGui::Separator();

        if (ImGui::MenuItem("Quit projectM", "Ctrl+q"))
        {
            _notificationCenter.postNotification(new QuitNotification);
        }

        ImGui::EndMenu();
    }
}

void EditorMenu::DrawHelpMenu()
{

    if (ImGui::BeginMenu("Help"))
    {

        if (ImGui::BeginMenu("Online Documentation"))
        {
            if (ImGui::MenuItem("Milkdrop Preset Authoring Guide"))
            {
                SystemBrowser::OpenURL("https://www.geisswerks.com/milkdrop/milkdrop_preset_authoring.html");
            }

            if (ImGui::MenuItem("Milkdrop Expression Syntax"))
            {
                SystemBrowser::OpenURL("https://github.com/projectM-visualizer/projectm-eval/blob/master/docs/Expression-Syntax.md");
            }

            if (ImGui::MenuItem("DirectX HLSL Reference"))
            {
                SystemBrowser::OpenURL("https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-reference");
            }

            ImGui::EndMenu();
        }

        ImGui::EndMenu();
    }
}

} // namespace Editor
