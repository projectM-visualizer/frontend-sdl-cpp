add_subdirectory(imgui_color_text_editor)

add_library(PresetEditor STATIC
        CodeContextInformation.cpp
        CodeContextInformation.h
        CodeEditorTab.cpp
        CodeEditorTab.h
        CodeEditorWindow.cpp
        CodeEditorWindow.h
        EditorMenu.cpp
        EditorMenu.h
        EditorPreset.cpp
        EditorPreset.h
        ExpressionCodeTypes.h
        PresetEditorGUI.cpp
        PresetEditorGUI.h
        PresetFile.cpp
        PresetFile.h
        )

target_link_libraries(PresetEditor
        PUBLIC
        projectM::Eval
        ImGUIColorTextEditor
        ImGui
        Poco::Util
        libprojectM::projectM
        )

target_include_directories(PresetEditor
        PRIVATE
        "${CMAKE_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/.."
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        )
