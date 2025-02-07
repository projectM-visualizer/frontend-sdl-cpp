add_subdirectory(notifications)
add_subdirectory(gui)

set(PROJECTM_CONFIGURATION_FILE "${CMAKE_CURRENT_BINARY_DIR}/projectMSDL.properties")
set(PROJECTM_CONFIGURATION_FILE "${PROJECTM_CONFIGURATION_FILE}" PARENT_SCOPE)
configure_file(resources/projectMSDL.properties.in "${PROJECTM_CONFIGURATION_FILE}" @ONLY)

add_executable(projectMSDL WIN32
        AudioCapture.cpp
        AudioCapture.h
        FPSLimiter.cpp
        FPSLimiter.h
        ProjectMSDLApplication.cpp
        ProjectMSDLApplication.h
        ProjectMWrapper.cpp
        ProjectMWrapper.h
        RenderLoop.cpp
        RenderLoop.h
        SDLRenderingWindow.cpp
        SDLRenderingWindow.h
        main.cpp
        )

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_sources(projectMSDL
            PRIVATE
            AudioCaptureImpl_WASAPI.h
            AudioCaptureImpl_WASAPI.cpp
            )
    target_compile_definitions(projectMSDL
            PRIVATE
            AUDIO_IMPL_HEADER="AudioCaptureImpl_WASAPI.h"
            )
else()
    target_sources(projectMSDL
            PRIVATE
            AudioCaptureImpl_SDL.h
            AudioCaptureImpl_SDL.cpp
            )
    target_compile_definitions(projectMSDL
            PRIVATE
            AUDIO_IMPL_HEADER="AudioCaptureImpl_SDL.h"
            )
endif()

# GLEW needs to be initialized if libprojectM depends on it.
if(TARGET GLEW::glew OR TARGET GLEW::glew_s)
    target_compile_definitions(projectMSDL
            PRIVATE
            USE_GLEW
            )
endif()

set_source_files_properties(ProjectMSDLApplication.cpp PROPERTIES
        COMPILE_DEFINITIONS PROJECTMSDL_CONFIG_LOCATION=\"${DEFAULT_CONFIG_PATH}\"
        )

target_compile_definitions(projectMSDL
        PRIVATE
        PROJECTMSDL_VERSION="${PROJECT_VERSION}"
        )


target_link_libraries(projectMSDL
        PRIVATE
        ProjectMSDL-GUI
        ProjectMSDL-Notifications
        libprojectM::playlist
        Poco::Util
        SDL2::SDL2$<$<STREQUAL:${SDL2_LINKAGE},static>:-static>
        SDL2::SDL2main
        )

# Add the dylib copying for macOS
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    function(copy_dylibs_to_frameworks TARGET_NAME DYLIB_SOURCE_DIR)
        # Create Frameworks directory
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory 
                "$<TARGET_FILE_DIR:${TARGET_NAME}>/../Frameworks"
        )

        # Find all .dylib files in the source directory
        file(GLOB DYLIB_FILES "${DYLIB_SOURCE_DIR}/*.dylib")
        
        # Copy each .dylib file and fix its rpath
        foreach(DYLIB_FILE ${DYLIB_FILES})
            get_filename_component(DYLIB_NAME ${DYLIB_FILE} NAME)
            add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                    "${DYLIB_FILE}"
                    "$<TARGET_FILE_DIR:${TARGET_NAME}>/../Frameworks/${DYLIB_NAME}"
            )
        endforeach()

        # Add rpath to the main executable
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND install_name_tool -add_rpath "@executable_path/../Frameworks"
                "$<TARGET_FILE:${TARGET_NAME}>"
        )
    endfunction()

    # Try to locate projectm
    find_path(PROJECTM_LIBRARY_DIR
        NAMES libprojectM-4.dylib  # Use a known file from the library directory
        PATHS
            $ENV{PROJECTM_ROOT}/lib         # Check an environment variable
            /usr/local/lib/projectm         # Common install location
            /opt/homebrew/lib/projectm      # Homebrew on Apple Silicon
            /opt/local/lib/projectm         # MacPorts location
        PATH_SUFFIXES
            lib                             # Look automatically appended subdirectories like 'lib'
        NO_DEFAULT_PATH
    )

    if (NOT PROJECTM_LIBRARY_DIR)
        find_path(PROJECTM_LIBRARY_DIR
            NAMES libprojectM.dylib
            PATHS ${CMAKE_PREFIX_PATH}
            PATH_SUFFIXES lib
        )
    endif()
    
    if (PROJECTM_LIBRARY_DIR)
        message(STATUS "Found projectM libraries at: ${PROJECTM_LIBRARY_DIR}")
        # Call the function to copy dylibs found in PROJECTM_LIBRARY_DIR
        copy_dylibs_to_frameworks(projectMSDL "${PROJECTM_LIBRARY_DIR}")
    else()
        message(WARNING "Could not find projectM libraries. Please set PROJECTM_ROOT or specify the library location manually.")
    endif()
endif()


if (MSVC)
    set_target_properties(projectMSDL
            PROPERTIES
            VS_DPI_AWARE "PerMonitor"
            )
endif()
