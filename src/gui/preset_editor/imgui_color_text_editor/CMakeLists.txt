add_library(ImGUIColorTextEditor STATIC
        TextEditor.cpp
        TextEditor.h
        )

target_link_libraries(ImGUIColorTextEditor
        PUBLIC
        ImGui
        )

target_include_directories(ImGUIColorTextEditor
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        )
