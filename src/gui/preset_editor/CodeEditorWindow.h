#pragma once

#include "CodeEditorTab.h"
#include "ExpressionCodeTypes.h"

#include <list>
#include <string>

namespace Editor {

class CodeEditorWindow
{
public:
    CodeEditorWindow() = default;

    virtual ~CodeEditorWindow() = default;

    void Draw();

    void OpenCodeInTab(ExpressionCodeTypes type, std::string& code, int index);

private:
    bool _visible{false}; //!< Determines if the code editor window is visible.
    bool _collapsed{false}; //!< If true, the window is displayed collapsed, e.g. the tab contents aren't rendered.

    std::list<CodeEditorTab> _codeEditorTabs; //!< Currently opened editor tabs.
};

} // namespace Editor
