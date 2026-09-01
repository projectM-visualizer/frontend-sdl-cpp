#pragma once

#include "ExpressionCodeTypes.h"

#include "TextEditor.h"

#include <string>
#include <vector>

namespace Editor {

/**
 * @class CodeContextInformation
 * @brief Retains a static list of identifiers specific to each code context.
 *
 * This is used in the code editor to highlight the pre-defined variables and shader inputs,
 * giving the user additional hints about which variable is user-defined in the context and
 * which is not.
 */
class CodeContextInformation final
{
public:
    CodeContextInformation() = delete;

    /**
     * @brief Returns the name of the context, to be displayed in the editor tab.
     * @param type The type to return the context name for.
     * @param index The index of a custom wave or shape.
     * @return The name of the code context for displaying in the editor tab.
     */
    static auto GetContextName(ExpressionCodeTypes type, int index = 0) -> std::string;

    /**
     * @brief Return a list of pre-defined variables/inputs for a given code context.
     * @param type The type to return the identifier list for.
     * @return A list of identifiers with description representing the pre-defined in/out variables for this expression context.
     */
    static auto GetIdentifierList(ExpressionCodeTypes type) -> std::vector<std::pair<const char*, const char*>>;

    /**
     * @brief Returns the correct editor language definition for the given type.
     * The context-specific identifiers are already set up properly.
     * @param type The type to return the language definition for.
     * @return
     */
    static const TextEditor::LanguageDefinition& GetLanguageDefinition(ExpressionCodeTypes type);

private:
    static auto PopulateLanguageDefinitionForType(ExpressionCodeTypes type) -> TextEditor::LanguageDefinition;
};

} // namespace Editor
