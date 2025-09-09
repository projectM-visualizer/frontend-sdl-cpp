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

    virtual ~CodeEditorTab();

    bool Draw();

    void SetSelected();

    std::string Title() const;

private:
    void CheckCodeSyntax(const std::string& codeToCheck);

    ImGuiTabItemFlags _nextRenderingFlags{}; //!< Additional flags to set when rendering the tab next time, e.g. activate it.
    std::string _tabTitle; //!< The title of the tab.
    std::string& _code; //!< A reference to the preset code.
    TextEditor _textEditor; //!< The expression/shader code editor control.
    bool _documentOpen{true}; //!< This flag holds the opened/closed state of the document, e.g. becomes false if the user closes the tab.

    projectm_eval_context* _compileTextContext{nullptr}; //!< A projectm-eval context for test-compiling the editor code.
};

} // namespace Editor
