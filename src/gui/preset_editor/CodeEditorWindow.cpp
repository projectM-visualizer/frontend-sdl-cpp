#include "CodeEditorWindow.h"

#include "CodeContextInformation.h"

#include "imgui.h"

namespace Editor {

void CodeEditorWindow::Draw()
{
    if (!_visible)
    {
        return;
    }

    ImGui::PushID("CodeEditorWindow");

    if (ImGui::BeginChild("CodeEditorChild", ImVec2(0, 500), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeY, ImGuiWindowFlags_None))
    {
        if (ImGui::BeginTabBar("OpenCodeTabs", ImGuiTabBarFlags_AutoSelectNewTabs))
        {
            for (auto it = begin(_codeEditorTabs); it != end(_codeEditorTabs);)
            {
                if (it->Draw())
                {
                    ++it;
                    continue;
                }

                it = _codeEditorTabs.erase(it);
            }

            ImGui::EndTabBar();
        }
    }
    ImGui::EndChild();

    if (_codeEditorTabs.empty())
    {
        _visible = false;
    }

    ImGui::PopID();
}

void CodeEditorWindow::OpenCodeInTab(ExpressionCodeTypes type, std::string& code, int index)
{
    std::string newTabTitle = CodeContextInformation::GetContextName(type, index);
    for (auto& tab : _codeEditorTabs)
    {
        if (tab.Title() == newTabTitle)
        {
            tab.SetSelected();
            return;
        }
    }

    _codeEditorTabs.emplace_back(type, code, index);
    _visible = true;
}

} // namespace Editor
