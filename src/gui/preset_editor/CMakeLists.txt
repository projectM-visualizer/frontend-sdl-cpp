add_subdirectory(imgui_color_text_editor)

add_library(PresetEditor STATIC
        EditorPreset.cpp
        EditorPreset.h
        PresetEditorGUI.cpp
        PresetEditorGUI.h
        PresetFile.cpp
        PresetFile.h
        EditorMenu.cpp
        EditorMenu.h
        )

target_link_libraries(PresetEditor
        PUBLIC
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
