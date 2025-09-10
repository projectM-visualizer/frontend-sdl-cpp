#pragma once

#include "ExpressionCodeTypes.h"
#include "TextEditor.h"

#include <projectm-eval.h>

namespace Editor {

class CodeEditorTab
{
public:
    CodeEditorTab() = delete;

    explicit CodeEditorTab(ExpressionCodeTypes type, std::string& code, int index);

    virtual ~CodeEditorTab() = default;

    CodeEditorTab(const CodeEditorTab& other) = delete;
    CodeEditorTab(CodeEditorTab&& other) noexcept = default;
    CodeEditorTab& operator=(const CodeEditorTab& other) = delete;
    CodeEditorTab& operator=(CodeEditorTab&& other) noexcept;

    bool Draw();

    void SetSelected();

    std::string Title() const;

private:
    using ContextDestructor = void(*)(projectm_eval_context*);
    using ContextHandle = std::unique_ptr<projectm_eval_context, ContextDestructor>;

    /**
     * Draws the editor toolbar for this tab.
     * @return true if an action was executed which changed the editor contents, false if not.
     */
    bool DrawToolBar();

    void CheckCodeSyntax(const std::string& codeToCheck);

    ImGuiTabItemFlags _nextRenderingFlags{}; //!< Additional flags to set when rendering the tab next time, e.g. activate it.
    std::string _tabTitle; //!< The title of the tab.
    ImVec4 _tabColor{0.0f, 0.0f, 1.0f, 1.0f}; //!< The background color of the inactive tab.
    ImVec4 _tabActiveColor{0.0f, 0.0f, 1.0f, 1.0f}; //!< The background color of the active tab.
    ImVec4 _tabHoverColor{0.0f, 0.0f, 1.0f, 1.0f}; //!< The background color of the hovered tab.
    std::string& _code; //!< A reference to the preset code.
    bool _showWhitespace{true}; //!< If true, the editor will display whitespace.
    std::unique_ptr<TextEditor> _textEditor; //!< The expression/shader code editor control.
    bool _documentOpen{true}; //!< This flag holds the opened/closed state of the document, e.g. becomes false if the user closes the tab.

    ContextHandle _compileTextContext{nullptr, projectm_eval_context_destroy}; //!< A projectm-eval context for test-compiling the editor code.
};

} // namespace Editor
