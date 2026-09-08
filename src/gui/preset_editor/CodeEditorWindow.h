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

    /**
     * Draws the code editor window and opened editor tabs within.
     */
    void Draw();

    /**
     * Opens a code block in a new editor tab, or switches to the tab is it's already open.
     * @param type The type of code the new tab should display (e.g. expression or shader code)
     * @param code The actual code in the editor tab.
     * @param index The expression index, used if there are multiple instances of a single type such as wave and shape code.
     */
    void OpenCodeInTab(ExpressionCodeTypes type, std::string& code, int index);

    /**
     * Returns whether any text has been changed in a open tab or not.
     * @return true if text in any open tab has been changed, false if not.
     */
    bool IsTextChanged() const;

private:
    bool _visible{false}; //!< Determines if the code editor window is visible.
    bool _collapsed{false}; //!< If true, the window is displayed collapsed, e.g. the tab contents aren't rendered.
    bool _isTextChanged{false}; //!< If true, the text in an open tab has been changed.

    std::list<CodeEditorTab> _codeEditorTabs; //!< Currently opened editor tabs.
};

} // namespace Editor
