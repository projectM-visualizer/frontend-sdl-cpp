#pragma once

#include "FileChooser.h"

#include <Poco/Logger.h>

class ProjectMWrapper;

/**
 * @brief Preset folder and file browser window
 *
 * Allows users to browse and select preset folders and individual preset files to load into the playlist.
 */
class PresetBrowser
{
public:
    enum class BrowseMode {
        Folder,  //!< Browsing for preset folders
        Files    //!< Browsing for individual preset files within a folder
    };

    /**
     * @brief Creates a new preset browser window.
     */
    PresetBrowser() = default;

    /**
     * @brief Shows the preset browser window.
     */
    void Show();

    /**
     * @brief Draws the preset browser window and processes interactions.
     */
    void Draw(ProjectMWrapper& projectMWrapper);

    /**
     * @brief Returns the currently selected folder path.
     */
    const std::string& SelectedFolderPath() const;

private:
    FileChooser _folderChooser{FileChooser::Mode::Directory}; //!< Directory chooser for folders.
    FileChooser _fileChooser{FileChooser::Mode::File}; //!< File chooser for individual preset files.
    bool _visible{false}; //!< Window visibility flag.
    std::string _selectedFolderPath; //!< Currently selected preset folder path.
    BrowseMode _browseMode{BrowseMode::Folder}; //!< Current browsing mode.

    Poco::Logger& _logger{Poco::Logger::get("PresetBrowser")}; //!< The class logger.
};
