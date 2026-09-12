# This file will be configured to contain variables for CPack. These variables
# should be set in the CMake list file of the project before CPack module is
# included. The list of available CPACK_xxx variables and their associated
# documentation may be obtained using
#  cpack --help-variable-list
#
# Some variables are common to all generators (e.g. CPACK_PACKAGE_NAME)
# and some are specific to a generator
# (e.g. CPACK_NSIS_EXTRA_INSTALL_COMMANDS). The generator specific variables
# usually begin with CPACK_<GENNAME>_xxxx.


set(CPACK_ARCHIVE_GID "-1")
set(CPACK_ARCHIVE_UID "-1")
set(CPACK_BUILD_SOURCE_DIRS "/Users/entropist/projectS/frontend-sdl-cpp;/Users/entropist/projectS/frontend-sdl-cpp/cmake-build")
set(CPACK_CMAKE_GENERATOR "Unix Makefiles")
set(CPACK_COMPONENT_UNSPECIFIED_HIDDEN "TRUE")
set(CPACK_COMPONENT_UNSPECIFIED_REQUIRED "TRUE")
set(CPACK_DEFAULT_PACKAGE_DESCRIPTION_FILE "/opt/homebrew/share/cmake/Templates/CPack.GenericDescription.txt")
set(CPACK_DEFAULT_PACKAGE_DESCRIPTION_SUMMARY "projectMSDL built using CMake")
set(CPACK_GENERATOR "productbuild")
set(CPACK_INNOSETUP_ARCHITECTURE "x64")
set(CPACK_INSTALL_CMAKE_PROJECTS "/Users/entropist/projectS/frontend-sdl-cpp/cmake-build;projectMSDL;ALL;/")
set(CPACK_INSTALL_PREFIX "/usr/local")
set(CPACK_MODULE_PATH "/Users/entropist/projectS/frontend-sdl-cpp/cmake")
set(CPACK_NSIS_DISPLAY_NAME "projectM 2.0.0")
set(CPACK_NSIS_INSTALLER_ICON_CODE "")
set(CPACK_NSIS_INSTALLER_MUI_ICON_CODE "")
set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES")
set(CPACK_NSIS_PACKAGE_NAME "projectM 2.0.0")
set(CPACK_NSIS_UNINSTALL_NAME "Uninstall")
set(CPACK_OBJDUMP_EXECUTABLE "/usr/bin/objdump")
set(CPACK_OUTPUT_CONFIG_FILE "/Users/entropist/projectS/frontend-sdl-cpp/cmake-build/CPackConfig.cmake")
set(CPACK_PACKAGE_DEFAULT_LOCATION "/")
set(CPACK_PACKAGE_DESCRIPTION_FILE "/Users/entropist/projectS/frontend-sdl-cpp/src/resources/package-description.txt")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A standalone, Milkdrop-like audio visualization application")
set(CPACK_PACKAGE_FILE_NAME "projectM-2.0.0-Darwin")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://projectm-visualizer.org/")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "projectM 2.0.0")
set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "projectM 2.0.0")
set(CPACK_PACKAGE_NAME "projectM")
set(CPACK_PACKAGE_RELOCATABLE "true")
set(CPACK_PACKAGE_VENDOR "The projectM Development Team")
set(CPACK_PACKAGE_VERSION "2.0.0")
set(CPACK_PACKAGE_VERSION_MAJOR "2")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PKGBUILD_IDENTITY_NAME "")
set(CPACK_PRODUCTBUILD_IDENTIFIER "org.projectm-visualizer.projectmsdl")
set(CPACK_PRODUCTBUILD_IDENTITY_NAME "")
set(CPACK_PROJECT_CONFIG_FILE "/Users/entropist/projectS/frontend-sdl-cpp/cmake-build/CPackAdditionalConfig.cmake")
set(CPACK_RESOURCE_FILE_LICENSE "/Users/entropist/projectS/frontend-sdl-cpp/src/resources/gpl-3.0.rtf")
set(CPACK_RESOURCE_FILE_README "/Users/entropist/projectS/frontend-sdl-cpp/src/resources/macos-readme.txt")
set(CPACK_RESOURCE_FILE_WELCOME "/Users/entropist/projectS/frontend-sdl-cpp/src/resources/macos-welcome.txt")
set(CPACK_SET_DESTDIR "OFF")
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_OUTPUT_CONFIG_FILE "/Users/entropist/projectS/frontend-sdl-cpp/cmake-build/CPackSourceConfig.cmake")
set(CPACK_STRIP_FILES "TRUE")
set(CPACK_SYSTEM_NAME "Darwin")
set(CPACK_THREADS "1")
set(CPACK_TOPLEVEL_TAG "Darwin")
set(CPACK_WIX_SIZEOF_VOID_P "8")

if(NOT CPACK_PROPERTIES_FILE)
  set(CPACK_PROPERTIES_FILE "/Users/entropist/projectS/frontend-sdl-cpp/cmake-build/CPackProperties.cmake")
endif()

if(EXISTS ${CPACK_PROPERTIES_FILE})
  include(${CPACK_PROPERTIES_FILE})
endif()

# Configuration for component "projectMSDL"

SET(CPACK_COMPONENTS_ALL projectMSDL)
set(CPACK_COMPONENT_PROJECTMSDL_PLIST "/Users/entropist/projectS/frontend-sdl-cpp/src/resources/projectMSDL-component.plist")
