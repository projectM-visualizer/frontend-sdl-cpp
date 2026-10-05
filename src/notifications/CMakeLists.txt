add_library(ProjectMSDL-Notifications STATIC
        DisplayToastNotification.cpp
        DisplayToastNotification.h
        PlaybackControlNotification.cpp
        PlaybackControlNotification.h
        QuitNotification.cpp
        QuitNotification.h
        UpdateWindowTitleNotification.cpp
        UpdateWindowTitleNotification.h
        )

target_include_directories(ProjectMSDL-Notifications
        PRIVATE
        "${CMAKE_SOURCE_DIR}/src/"
        )

target_link_libraries(ProjectMSDL-Notifications
        PUBLIC
        Poco::Foundation
        )
