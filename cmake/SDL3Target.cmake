# Helper script to set up the SDL3 CMake target and version variable.
# SDL3 has proper CMake config support, so this is simpler than SDL2.
#
# This also defines SDL3_VERSION from the found package and can be used
# to verify a minimum version requirement.

if(NOT TARGET SDL3::SDL3)
    message(FATAL_ERROR "SDL3::SDL3 target not found. Ensure SDL3 is properly installed.")
endif()

if(SDL3_VERSION AND SDL3_VERSION VERSION_LESS "3.0.0")
    message(FATAL_ERROR "SDL3 libraries were found, but have version ${SDL3_VERSION}. At least version 3.0.0 is required.")
endif()
