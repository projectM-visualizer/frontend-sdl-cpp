#pragma once

#include <SDL2/SDL.h>

#include <Poco/Logger.h>

#include <string>
#include <vector>

class ProjectMWrapper;

/**
 * @brief Playlist browser window.
 *
 * Displays all presets in the current playlist as a scrollable,
 * filterable list. Users can click any preset to jump to it.
 */
class PresetSelection
{
public:
    explicit PresetSelection(ProjectMWrapper& projectMWrapper);

    /**
     * @brief Shows the playlist browser window.
     */
    void Show();

    /**
     * @brief Draws the playlist browser window if visible.
     */
    void Draw();

private:
    /**
     * @brief Refreshes the cached playlist items from the playlist manager.
     */
    void RefreshPlaylist();

    /**
     * @brief Returns the basename of a file path.
     */
    static std::string Basename(const std::string& path);

    ProjectMWrapper& _projectMWrapper; //!< Reference to the projectM wrapper.

    bool _visible{false}; //!< Window visibility flag.

    char _filterBuffer[256]{}; //!< Text filter input buffer.
    std::string _lastFilter; //!< Last applied filter string.

    /// Cached playlist entries for filtered display.
    struct PlaylistEntry
    {
        uint32_t index;        //!< Playlist index.
        std::string fullPath;  //!< Full preset file path.
        std::string basename;  //!< Just the filename.
    };
    std::vector<PlaylistEntry> _cachedEntries; //!< All (unfiltered) playlist entries.
    std::vector<PlaylistEntry> _filteredEntries; //!< Entries matching current filter.
    bool _dirty{true}; //!< True when the cache needs refresh.

    char _exportPathBuffer[512]{}; //!< Export destination path buffer.
    bool _showExport{false}; //!< Show the export section.

    Poco::Logger& _logger{Poco::Logger::get("PresetSelection")}; //!< The class logger.
};
