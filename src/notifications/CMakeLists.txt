add_library(ProjectMSDL-Notifications STATIC
        AudioDataAvailable.cpp
        AudioDataAvailable.h
        DisplayToast.cpp
        DisplayToast.h
        PlaybackControl.cpp
        PlaybackControl.h
        Quit.cpp
        Quit.h
        UpdateWindowTitle.cpp
        UpdateWindowTitle.h
        )

target_include_directories(ProjectMSDL-Notifications
        PRIVATE
        "${CMAKE_SOURCE_DIR}/src/"
        )

target_link_libraries(ProjectMSDL-Notifications
        PUBLIC
        Poco::Foundation
        )
