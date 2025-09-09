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
    bool _visible{false};
    std::list<CodeEditorTab> _codeEditorTabs;
};

} // namespace Editor
