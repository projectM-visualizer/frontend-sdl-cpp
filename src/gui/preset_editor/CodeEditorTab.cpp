#include "CodeEditorTab.h"

#include "CodeContextInformation.h"

#include <variant>

void projectm_eval_memory_host_lock_mutex()
{
}
void projectm_eval_memory_host_unlock_mutex()
{
}

namespace Editor {

CodeEditorTab::CodeEditorTab(ExpressionCodeTypes type, std::string& code, int index)
    : _code(code)
{
    _tabTitle = CodeContextInformation::GetContextName(type, index);
    _textEditor.SetLanguageDefinition(CodeContextInformation::GetLanguageDefinition(type));
    _textEditor.SetText(code);

    if (type != ExpressionCodeTypes::WarpShader && type != ExpressionCodeTypes::CompositeShader)
    {
        _compileTextContext = projectm_eval_context_create(nullptr, nullptr);
        CheckCodeSyntax(code);
    }
}

CodeEditorTab::~CodeEditorTab()
{
    if (_compileTextContext)
    {
        projectm_eval_context_destroy(_compileTextContext);
        _compileTextContext = nullptr;
    }
}

bool CodeEditorTab::Draw()
{
    ImGui::PushID(_tabTitle.c_str());

    bool tabVisible = ImGui::BeginTabItem(_tabTitle.c_str(), &_documentOpen, _nextRenderingFlags);

    if (tabVisible)
    {
        _textEditor.Render((_tabTitle + "##EditorControl").c_str());

        if (_textEditor.IsTextChanged())
        {
            std::string changedText = _textEditor.GetText();

            // The text editor always leaves a newline if empty, check & clear if that's the case.
            if (changedText.size() == 1 && changedText.at(0) == '\n')
            {
                changedText = "";
            }

            CheckCodeSyntax(changedText);

            _code = changedText;
        }

        ImGui::EndTabItem();
    }

    ImGui::PopID();

    _nextRenderingFlags = ImGuiTabItemFlags_None;

    return _documentOpen;
}

void CodeEditorTab::SetSelected()
{
    _nextRenderingFlags |= ImGuiTabItemFlags_SetSelected;
}

std::string CodeEditorTab::Title() const
{
    return _tabTitle;
}

void CodeEditorTab::CheckCodeSyntax(const std::string& codeToCheck)
{
    if (_compileTextContext)
    {
        auto* code = projectm_eval_code_compile(_compileTextContext, codeToCheck.c_str());
        if (code)
        {
            _textEditor.SetErrorMarkers({});
            projectm_eval_code_destroy(code);
        }
        else
        {
            int line{};
            std::string errorMessage = projectm_eval_get_error(_compileTextContext, &line, nullptr);
            _textEditor.SetErrorMarkers({{line, errorMessage}});
        }
    }
}

} // namespace Editor
