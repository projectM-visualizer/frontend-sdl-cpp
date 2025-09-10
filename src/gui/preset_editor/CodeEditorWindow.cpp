#include "CodeEditorWindow.h"

#include "CodeContextInformation.h"
#include "IconsFontAwesome7.h"

#include "imgui.h"

namespace Editor {

void CodeEditorWindow::Draw()
{
    if (!_visible)
    {
        return;
    }

    ImGui::PushID("CodeEditorWindow");

    if (ImGui::Button(_collapsed ? ICON_FA_CARET_RIGHT "##CollapseButton" : ICON_FA_CARET_DOWN "##CollapseButton"))
    {
        _collapsed = !_collapsed;
    }

    ImGui::SameLine();

    if (!_collapsed)
    {
        ImGui::SetNextWindowSize(ImVec2(0, 500), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowBgAlpha(_collapsed ? 0.0f : 0.5f);
        if (ImGui::BeginChild("CodeEditorChild", ImVec2(0, 500), ImGuiChildFlags_ResizeY, ImGuiWindowFlags_None))
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
            }

            ImGui::EndTabBar();
        }
        ImGui::EndChild();
    }
    else
    {
        ImGui::SameLine();
        ImGui::TextDisabled("%s", ("(" + std::to_string(_codeEditorTabs.size()) + " Tabs Open)").c_str());
    }

    if (_codeEditorTabs.empty())
    {
        _visible = false;
    }

    ImGui::PopID();
}

void CodeEditorWindow::OpenCodeInTab(ExpressionCodeTypes type, std::string& code, int index)
{
    // Fold out the editor if the user opens a new tab or focuses an existing one.
    _collapsed = false;

    std::string newTabTitle = CodeContextInformation::GetContextName(type, index);
    for (auto& tab : _codeEditorTabs)
    {
        if (tab.Title() == newTabTitle)
        {
            tab.SetSelected();
            return;
        }
    }

    CodeEditorTab newCodeEditorTab(type, code, index);
    newCodeEditorTab.SetSelected();
    _codeEditorTabs.push_back(std::move(newCodeEditorTab));

    _visible = true;
}


} // namespace Editor
