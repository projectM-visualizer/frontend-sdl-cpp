#include "EditorMenu.h"

#include "IconsFontAwesome7.h"
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
        if (ImGui::Button(ICON_FA_PLAY " Update Preset Preview"))
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
        if (ImGui::MenuItem(ICON_FA_SQUARE_PLUS " New Preset"))
        {
            _presetEditorGUI.Show("");
        }

        if (ImGui::MenuItem(ICON_FA_FOLDER_OPEN " Open Preset..."))
        {
        }

        if (ImGui::MenuItem(ICON_FA_FLOPPY_DISK " Save Preset"))
        {
        }

        if (ImGui::MenuItem(ICON_FA_FLOPPY_DISK " Save Preset As..."))
        {
        }

        ImGui::Separator();

        if (ImGui::MenuItem(ICON_FA_CIRCLE_XMARK " Exit Preset Editor"))
        {
            _presetEditorGUI.Close();
        }

        ImGui::Separator();

        if (ImGui::MenuItem(ICON_FA_DOOR_OPEN " Quit projectM", "Ctrl+q"))
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

        if (ImGui::BeginMenu(ICON_FA_INFO " Online Documentation"))
        {
            if (ImGui::MenuItem(ICON_FA_ARROW_UP_RIGHT_FROM_SQUARE " Milkdrop Preset Authoring Guide"))
            {
                SystemBrowser::OpenURL("https://www.geisswerks.com/milkdrop/milkdrop_preset_authoring.html");
            }

            if (ImGui::MenuItem(ICON_FA_ARROW_UP_RIGHT_FROM_SQUARE " Milkdrop Expression Syntax"))
            {
                SystemBrowser::OpenURL("https://github.com/projectM-visualizer/projectm-eval/blob/master/docs/Expression-Syntax.md");
            }

            if (ImGui::MenuItem(ICON_FA_ARROW_UP_RIGHT_FROM_SQUARE " DirectX HLSL Reference"))
            {
                SystemBrowser::OpenURL("https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-reference");
            }

            ImGui::EndMenu();
        }

        ImGui::EndMenu();
    }
}

} // namespace Editor
