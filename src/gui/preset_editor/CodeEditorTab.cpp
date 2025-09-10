#include "CodeEditorTab.h"

#include "CodeContextInformation.h"
#include "IconsFontAwesome7.h"

#include <variant>

void projectm_eval_memory_host_lock_mutex()
{
}
void projectm_eval_memory_host_unlock_mutex()
{
}

namespace Editor {

using CodeDestructor = void (*)(projectm_eval_code*);
using CodeHandle = std::unique_ptr<projectm_eval_code, CodeDestructor>;

CodeEditorTab::CodeEditorTab(ExpressionCodeTypes type, std::string& code, int index)
    : _code(code)
    , _textEditor(std::make_unique<TextEditor>())
{
    _tabTitle = CodeContextInformation::GetContextName(type, index);
    _textEditor->SetTabsAsSpaces(true);
    _textEditor->SetTabSize(4);
    _textEditor->SetLanguageDefinition(CodeContextInformation::GetLanguageDefinition(type));
    _textEditor->SetText(code);

    if (type != ExpressionCodeTypes::WarpShader && type != ExpressionCodeTypes::CompositeShader)
    {
        _compileTextContext.reset(projectm_eval_context_create(nullptr, nullptr));
        CheckCodeSyntax(code);
    }

    switch (type)
    {
        case ExpressionCodeTypes::PerFrameInit:
        case ExpressionCodeTypes::PerFrame:
        case ExpressionCodeTypes::PerVertex:
            _tabHoverColor = {0xe7 / 256.f, 0x4c / 256.f, 0x3c / 256.f, 1.0f};
            break;

        case ExpressionCodeTypes::CustomWaveInit:
        case ExpressionCodeTypes::CustomWavePerFrame:
        case ExpressionCodeTypes::CustomWavePerPoint:
            _tabHoverColor = {0x29 / 256.f, 0x80 / 256.f, 0xb9 / 256.f, 1.0f};
            break;

        case ExpressionCodeTypes::CustomShapeInit:
        case ExpressionCodeTypes::CustomShapePerFrame:
            _tabHoverColor = {0x8e / 256.f, 0x44 / 256.f, 0xad / 256.f, 1.0f};
            break;

        case ExpressionCodeTypes::WarpShader:
            _tabHoverColor = {0x27 / 256.f, 0xae / 256.f, 0x60 / 256.f, 1.0f};
            break;

        case ExpressionCodeTypes::CompositeShader:
            _tabHoverColor = {0xf3 / 256.f, 0x9c / 256.f, 0x12 / 256.f, 1.0f};
            break;
    }

    _tabActiveColor = {_tabHoverColor.x * 0.8f, _tabHoverColor.y * 0.8f, _tabHoverColor.z * 0.8f, _tabHoverColor.w};
    _tabColor = {_tabHoverColor.x * 0.5f, _tabHoverColor.y * 0.5f, _tabHoverColor.z * 0.5f, _tabHoverColor.w * 0.75f};
}

CodeEditorTab& CodeEditorTab::operator=(CodeEditorTab&& other) noexcept
{
    _nextRenderingFlags = other._nextRenderingFlags;
    _tabTitle = std::move(other._tabTitle);
    _tabColor = other._tabColor;
    _tabActiveColor = other._tabActiveColor;
    _tabHoverColor = other._tabHoverColor;
    _code = other._code;
    _textEditor = std::move(other._textEditor);
    _documentOpen = other._documentOpen;
    _compileTextContext = std::move(other._compileTextContext);

    return *this;
}

bool CodeEditorTab::Draw()
{
    ImGui::PushID(_tabTitle.c_str());

    ImGui::PushStyleColor(ImGuiCol_Tab, _tabColor);
    ImGui::PushStyleColor(ImGuiCol_TabActive, _tabActiveColor);
    ImGui::PushStyleColor(ImGuiCol_TabHovered, _tabHoverColor);
    bool tabVisible = ImGui::BeginTabItem(_tabTitle.c_str(), &_documentOpen, _nextRenderingFlags);

    if (tabVisible)
    {
        bool textChangedByToolBar = DrawToolBar();

        _textEditor->Render((_tabTitle + "##EditorControl").c_str());

        if (_textEditor->IsTextChanged() || textChangedByToolBar)
        {
            std::string changedText = _textEditor->GetText();

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

    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();

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

bool CodeEditorTab::DrawToolBar()
{
    bool textChanged = false;

    ImGui::BeginDisabled(!_textEditor->HasSelection());
    if (ImGui::Button(ICON_FA_COPY " Copy##ToolBar"))
    {
        _textEditor->Copy();
    }
    ImGui::SameLine();
    if (ImGui::Button(ICON_FA_SCISSORS " Cut##ToolBar"))
    {
        _textEditor->Cut();
        textChanged= true;
    }
    ImGui::EndDisabled();

    ImGui::BeginDisabled(strlen(ImGui::GetClipboardText()) == 0);
    ImGui::SameLine();
    if (ImGui::Button(ICON_FA_PASTE " Paste##ToolBar"))
    {
        _textEditor->Paste();
        textChanged= true;
    }
    ImGui::EndDisabled();

    ImGui::SameLine();
    ImGui::Separator();

    ImGui::BeginDisabled(!_textEditor->CanUndo());
    ImGui::SameLine();
    if (ImGui::Button(ICON_FA_ARROW_ROTATE_LEFT " Undo##ToolBar"))
    {
        _textEditor->Undo();
        textChanged= true;
    }
    ImGui::EndDisabled();

    ImGui::BeginDisabled(!_textEditor->CanRedo());
    ImGui::SameLine();
    if (ImGui::Button(ICON_FA_ARROW_ROTATE_RIGHT " Redo##ToolBar"))
    {
        _textEditor->Redo();
        textChanged= true;
    }
    ImGui::EndDisabled();

    ImGui::SameLine();
    if (ImGui::Checkbox("Whitespace##ToolBar", &_showWhitespace))
    {
        _textEditor->SetShowWhitespaces(_showWhitespace);
    }

    return textChanged;
}

void CodeEditorTab::CheckCodeSyntax(const std::string& codeToCheck)
{
    if (_compileTextContext)
    {
        auto code = CodeHandle(projectm_eval_code_compile(_compileTextContext.get(), codeToCheck.c_str()), projectm_eval_code_destroy);
        if (code)
        {
            _textEditor->SetErrorMarkers({});
        }
        else
        {
            int line{};
            std::string errorMessage = projectm_eval_get_error(_compileTextContext.get(), &line, nullptr);
            _textEditor->SetErrorMarkers({{line, errorMessage}});
        }
    }
}

} // namespace Editor
