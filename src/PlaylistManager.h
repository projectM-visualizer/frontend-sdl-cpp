#pragma once

#include <projectM-4/playlist.h>

#include <Poco/Logger.h>
#include <Poco/Util/Subsystem.h>

#include <map>
#include <string>
#include <vector>

/**
 * @brief Manages named, persistent playlists stored on disk.
 *
 * Playlists are saved as simple text files (one preset path per line) in
 * ~/.config/projectM/playlists/<name>.milklist
 *
 * This subsystem integrates with ProjectMWrapper to load saved playlists
 * into the active projectM session.
 */
class PlaylistManager : public Poco::Util::Subsystem
{
public:
    const char* name() const override;

    void initialize(Poco::Util::Application& app) override;

    void uninitialize() override;

    /**
     * @brief Returns all saved playlists with their preset counts.
     * @return A map of playlist name to number of presets.
     */
    std::map<std::string, uint32_t> ListPlaylists() const;

    /**
     * @brief Saves the current in-memory playlist under the given name.
     * @param name The playlist name (used as the filename).
     * @return True on success.
     */
    bool Save(const std::string& name);

    /**
     * @brief Loads a saved playlist, replacing the current session playlist.
     *
     * Clears the active playlist and populates it from the file. If any
     * preset files no longer exist on disk, they are silently skipped.
     *
     * @param name The playlist name to load.
     * @return True on success.
     */
    bool Load(const std::string& name);

    /**
     * @brief Deletes a saved playlist file.
     * @param name The playlist name to delete.
     * @return True on success.
     */
    bool Delete(const std::string& name);

    /**
     * @brief Renames a saved playlist file.
     * @param oldName The current playlist name.
     * @param newName The new playlist name.
     * @return True on success.
     */
    bool Rename(const std::string& oldName, const std::string& newName);

    /**
     * @brief Returns the directory where playlist files are stored.
     * @return The playlist directory path.
     */
    std::string PlaylistDirectory() const;

    /**
     * @brief Returns the preset count for a specific saved playlist.
     * @param name The playlist name.
     * @return The number of presets, or 0 if the playlist doesn't exist.
     */
    uint32_t GetPlaylistSize(const std::string& name) const;

    /**
     * @brief Returns whether a playlist with the given name exists.
     * @param name The playlist name.
     * @return True if the playlist file exists.
     */
    bool Exists(const std::string& name) const;

    /**
     * @brief Creates an empty playlist file with the given name.
     * @param name The playlist name.
     * @return True on success.
     */
    bool CreateEmpty(const std::string& name);

    /**
     * @brief Exports all presets in the active playlist to a folder.
     *
     * Copies each preset file into the destination directory. If a file
     * with the same name already exists, it is overwritten.
     *
     * @param destinationDir The folder to copy presets into.
     * @return The number of presets exported, or -1 on error.
     */
    int32_t Export(const std::string& destinationDir);

    /**
     * @brief Appends a preset path to a saved playlist file.
     * @param name The playlist name.
     * @param presetPath The preset file path to add.
     * @return True if added, false if already present or on error.
     */
    bool AddPreset(const std::string& name, const std::string& presetPath);

    /**
     * @brief Stores the given name as the current playlist for session restore.
     * @param name The playlist name to remember, or empty to clear.
     */
    void SetCurrentPlaylist(const std::string& name);

    /**
     * @brief Returns the last saved current playlist name, or empty string.
     */
    std::string CurrentPlaylistName() const;

private:
    /**
     * @brief Returns the full path to a playlist file.
     * @param name The playlist name.
     * @return The full file path.
     */
    std::string PlaylistPath(const std::string& name) const;

    /**
     * @brief Reads a playlist file and returns the list of preset paths.
     * @param path The file path to read.
     * @return Vector of preset file paths (non-empty, non-comment lines).
     */
    std::vector<std::string> ReadPlaylistFile(const std::string& path) const;

    /**
     * @brief Writes a list of preset paths to a playlist file.
     * @param path The file path to write.
     * @param paths The preset file paths to write.
     * @return True on success.
     */
    bool WritePlaylistFile(const std::string& path, const std::vector<std::string>& paths) const;

    /**
     * @brief Counts presets in a playlist file.
     * @param path The file path.
     * @return The number of non-comment, non-empty lines.
     */
    uint32_t CountPresetsInFile(const std::string& path) const;

    projectm_playlist_handle _playlist{nullptr}; //!< Handle to the active projectM playlist.

    std::string _playlistDirectory; //!< Directory where playlist files are stored.

    Poco::Logger& _logger{Poco::Logger::get("PlaylistManager")}; //!< The class logger.
};
