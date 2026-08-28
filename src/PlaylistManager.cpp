#include "PlaylistManager.h"
#include "ProjectMSDLApplication.h"
#include "ProjectMWrapper.h"

#include "notifications/DisplayToastNotification.h"

#include <Poco/File.h>
#include <Poco/NotificationCenter.h>
#include <Poco/Path.h>
#include <Poco/Util/Application.h>

#include <fstream>

const char* PlaylistManager::name() const
{
    return "Playlist Manager";
}

void PlaylistManager::initialize(Poco::Util::Application& app)
{
    auto& projectMWrapper = app.getSubsystem<ProjectMWrapper>();
    _playlist = projectMWrapper.Playlist();

    // Set up the playlist directory
    Poco::Path dir = Poco::Path::configHome();
    dir.makeDirectory().append("projectM/playlists/");
    _playlistDirectory = dir.toString();

    try
    {
        Poco::File playlistsDir(_playlistDirectory);
        if (!playlistsDir.exists())
        {
            playlistsDir.createDirectories();
            poco_information_f1(_logger, "Created playlist directory: %s", _playlistDirectory);
        }
    }
    catch (Poco::Exception& ex)
    {
        poco_error_f1(_logger, "Failed to create playlist directory: %s", ex.displayText());
    }

    // Restore the last-used playlist from the previous session
    std::string lastPlaylist = CurrentPlaylistName();
    if (!lastPlaylist.empty() && Exists(lastPlaylist))
    {
        Load(lastPlaylist);
        poco_information_f1(_logger, "Restored last session playlist: %s", lastPlaylist);
    }
}

void PlaylistManager::uninitialize()
{
    _playlist = nullptr;
}

std::map<std::string, uint32_t> PlaylistManager::ListPlaylists() const
{
    std::map<std::string, uint32_t> playlists;

    try
    {
        Poco::File dir(_playlistDirectory);
        if (!dir.exists() || !dir.isDirectory())
        {
            return playlists;
        }

        std::vector<std::string> files;
        dir.list(files);

        for (const auto& filename : files)
        {
            Poco::Path filePath(_playlistDirectory, filename);
            if (filePath.getExtension() == "milklist")
            {
                std::string name = filePath.getBaseName();
                uint32_t count = CountPresetsInFile(filePath.toString());
                playlists[name] = count;
            }
        }
    }
    catch (Poco::Exception& ex)
    {
        poco_error_f1(_logger, "Failed to list playlists: %s", ex.displayText());
    }

    return playlists;
}

bool PlaylistManager::Save(const std::string& name)
{
    if (name.empty())
    {
        return false;
    }

    if (!_playlist)
    {
        poco_error(_logger, "No active playlist handle available.");
        return false;
    }

    uint32_t size = projectm_playlist_size(_playlist);
    if (size == 0)
    {
        poco_warning(_logger, "Current playlist is empty — nothing to save.");
        return false;
    }

    // Collect all preset paths from the active playlist
    std::vector<std::string> paths;
    paths.reserve(size);

    char** items = projectm_playlist_items(_playlist, 0, size);
    if (items)
    {
        for (uint32_t i = 0; items[i] != nullptr; i++)
        {
            paths.push_back(std::string(items[i]));
        }
        projectm_playlist_free_string_array(items);
    }

    std::string filePath = PlaylistPath(name);
    if (WritePlaylistFile(filePath, paths))
    {
        poco_information_f2(_logger, "Saved playlist \"%s\" with %?u presets.", name, static_cast<unsigned int>(size));
        SetCurrentPlaylist(name);
        return true;
    }

    return false;
}

bool PlaylistManager::Load(const std::string& name)
{
    if (name.empty() || !_playlist)
    {
        return false;
    }

    std::string filePath = PlaylistPath(name);
    auto paths = ReadPlaylistFile(filePath);

    if (paths.empty())
    {
        poco_warning_f1(_logger, "Playlist \"%s\" is empty or could not be read.", name);
        return false;
    }

    // Clear current playlist and populate from file
    projectm_playlist_clear(_playlist);

    uint32_t loaded = 0;
    uint32_t skipped = 0;

    for (const auto& presetPath : paths)
    {
        Poco::File file(presetPath);
        if (file.exists() && file.isFile())
        {
            projectm_playlist_add_preset(_playlist, presetPath.c_str(), false);
            loaded++;
        }
        else
        {
            poco_debug_f1(_logger, "Skipping missing preset: %s", presetPath);
            skipped++;
        }
    }

    // Sort the loaded playlist
    if (loaded > 0)
    {
        projectm_playlist_sort(_playlist, 0, projectm_playlist_size(_playlist),
                                SORT_PREDICATE_FILENAME_ONLY, SORT_ORDER_ASCENDING);

        // Start playing the first preset
        projectm_playlist_set_position(_playlist, 0, true);
    }

    poco_information_f3(_logger, "Loaded playlist \"%s\": %?u presets loaded, %?u skipped.",
                        name, loaded, skipped);

    SetCurrentPlaylist(name);

    if (skipped > 0)
    {
        Poco::NotificationCenter::defaultCenter().postNotification(
            new DisplayToastNotification(Poco::format("Loaded \"%s\" (%u presets, %u missing)",
                                                      name, loaded, skipped)));
    }
    else
    {
        Poco::NotificationCenter::defaultCenter().postNotification(
            new DisplayToastNotification(Poco::format("Loaded \"%s\" (%u presets)", name, loaded)));
    }

    return loaded > 0;
}

bool PlaylistManager::Delete(const std::string& name)
{
    std::string filePath = PlaylistPath(name);

    try
    {
        Poco::File file(filePath);
        if (file.exists())
        {
            file.remove();
            poco_information_f1(_logger, "Deleted playlist: %s", name);
            return true;
        }
    }
    catch (Poco::Exception& ex)
    {
        poco_error_f2(_logger, "Failed to delete playlist \"%s\": %s", name, ex.displayText());
    }

    return false;
}

bool PlaylistManager::Rename(const std::string& oldName, const std::string& newName)
{
    if (oldName == newName || newName.empty())
    {
        return false;
    }

    std::string oldPath = PlaylistPath(oldName);
    std::string newPath = PlaylistPath(newName);

    try
    {
        Poco::File oldFile(oldPath);
        if (!oldFile.exists())
        {
            poco_warning_f1(_logger, "Cannot rename: playlist \"%s\" does not exist.", oldName);
            return false;
        }

        Poco::File newFile(newPath);
        if (newFile.exists())
        {
            poco_warning_f1(_logger, "Cannot rename: playlist \"%s\" already exists.", newName);
            return false;
        }

        oldFile.renameTo(newPath);
        poco_information_f2(_logger, "Renamed playlist \"%s\" to \"%s\".", oldName, newName);
        return true;
    }
    catch (Poco::Exception& ex)
    {
        poco_error_f2(_logger, "Failed to rename playlist \"%s\": %s", oldName, ex.displayText());
    }

    return false;
}

std::string PlaylistManager::PlaylistDirectory() const
{
    return _playlistDirectory;
}

uint32_t PlaylistManager::GetPlaylistSize(const std::string& name) const
{
    return CountPresetsInFile(PlaylistPath(name));
}

bool PlaylistManager::Exists(const std::string& name) const
{
    return Poco::File(PlaylistPath(name)).exists();
}

bool PlaylistManager::CreateEmpty(const std::string& name)
{
    if (name.empty())
    {
        return false;
    }

    std::string filePath = PlaylistPath(name);
    std::vector<std::string> emptyPaths;

    if (WritePlaylistFile(filePath, emptyPaths))
    {
        poco_information_f1(_logger, "Created empty playlist: %s", name);
        SetCurrentPlaylist(name);
        return true;
    }

    return false;
}

int32_t PlaylistManager::Export(const std::string& destinationDir)
{
    if (destinationDir.empty() || !_playlist)
    {
        return -1;
    }

    uint32_t size = projectm_playlist_size(_playlist);
    if (size == 0)
    {
        poco_warning(_logger, "Current playlist is empty — nothing to export.");
        return 0;
    }

    try
    {
        Poco::File destDir(destinationDir);
        if (!destDir.exists())
        {
            destDir.createDirectories();
        }

        if (!destDir.isDirectory())
        {
            poco_error_f1(_logger, "Export destination is not a directory: %s", destinationDir);
            return -1;
        }
    }
    catch (Poco::Exception& ex)
    {
        poco_error_f1(_logger, "Failed to create export directory: %s", ex.displayText());
        return -1;
    }

    char** items = projectm_playlist_items(_playlist, 0, size);
    if (!items)
    {
        return -1;
    }

    int32_t exported = 0;
    int32_t failed = 0;

    for (uint32_t i = 0; items[i] != nullptr; i++)
    {
        try
        {
            Poco::Path sourcePath(items[i]);
            Poco::Path destPath(destinationDir);
            destPath.makeDirectory().setFileName(sourcePath.getFileName());

            Poco::File sourceFile(items[i]);
            if (sourceFile.exists())
            {
                sourceFile.copyTo(destPath.toString());
                exported++;
            }
            else
            {
                poco_debug_f1(_logger, "Skipping missing preset for export: %s", items[i]);
                failed++;
            }
        }
        catch (Poco::Exception& ex)
        {
            poco_error_f2(_logger, "Failed to export preset \"%s\": %s", items[i], ex.displayText());
            failed++;
        }
    }

    projectm_playlist_free_string_array(items);

    poco_information_f3(_logger, "Exported %?d presets to \"%s\" (%?d failed).",
                        exported, destinationDir, failed);

    return exported;
}

bool PlaylistManager::AddPreset(const std::string& name, const std::string& presetPath)
{
    if (name.empty() || presetPath.empty())
    {
        return false;
    }

    std::string filePath = PlaylistPath(name);
    auto paths = ReadPlaylistFile(filePath);

    // Check for duplicates
    for (const auto& p : paths)
    {
        if (p == presetPath)
        {
            return false; // already present
        }
    }

    paths.push_back(presetPath);
    return WritePlaylistFile(filePath, paths);
}

void PlaylistManager::SetCurrentPlaylist(const std::string& name)
{
    try
    {
        auto& app = Poco::Util::Application::instance();
        auto& projectMSDLApp = dynamic_cast<ProjectMSDLApplication&>(app);
        projectMSDLApp.UserConfiguration()->setString("playlist.currentPlaylist", name);
    }
    catch (...)
    {
        // Best effort
    }
}

std::string PlaylistManager::CurrentPlaylistName() const
{
    try
    {
        auto& app = Poco::Util::Application::instance();
        auto& projectMSDLApp = dynamic_cast<ProjectMSDLApplication&>(app);
        return projectMSDLApp.UserConfiguration()->getString("playlist.currentPlaylist", "");
    }
    catch (...)
    {
        return {};
    }
}

std::string PlaylistManager::PlaylistPath(const std::string& name) const
{
    Poco::Path path(_playlistDirectory, name);
    return path.makeFile().toString() + ".milklist";
}

std::vector<std::string> PlaylistManager::ReadPlaylistFile(const std::string& path) const
{
    std::vector<std::string> paths;

    try
    {
        std::ifstream file(path);
        if (!file.is_open())
        {
            poco_warning_f1(_logger, "Could not open playlist file: %s", path);
            return paths;
        }

        std::string line;
        while (std::getline(file, line))
        {
            // Trim whitespace
            size_t start = line.find_first_not_of(" \t\r\n");
            if (start == std::string::npos)
            {
                continue; // Empty line
            }
            size_t end = line.find_last_not_of(" \t\r\n");
            std::string trimmed = line.substr(start, end - start + 1);

            // Skip comment lines
            if (trimmed.empty() || trimmed[0] == '#')
            {
                continue;
            }

            paths.push_back(trimmed);
        }
    }
    catch (std::exception& ex)
    {
        poco_error_f2(_logger, "Error reading playlist file \"%s\": %s", path, std::string(ex.what()));
    }

    return paths;
}

bool PlaylistManager::WritePlaylistFile(const std::string& path, const std::vector<std::string>& paths) const
{
    try
    {
        std::ofstream file(path, std::ios::out | std::ios::trunc);
        if (!file.is_open())
        {
            poco_error_f1(_logger, "Could not open playlist file for writing: %s", path);
            return false;
        }

        for (const auto& presetPath : paths)
        {
            file << presetPath << '\n';
        }

        return file.good();
    }
    catch (std::exception& ex)
    {
        poco_error_f2(_logger, "Error writing playlist file \"%s\": %s", path, std::string(ex.what()));
        return false;
    }
}

uint32_t PlaylistManager::CountPresetsInFile(const std::string& path) const
{
    uint32_t count = 0;

    try
    {
        std::ifstream file(path);
        if (!file.is_open())
        {
            return 0;
        }

        std::string line;
        while (std::getline(file, line))
        {
            // Trim and skip empty/comment lines
            size_t start = line.find_first_not_of(" \t\r\n");
            if (start != std::string::npos && line[start] != '#')
            {
                count++;
            }
        }
    }
    catch (...)
    {
        return 0;
    }

    return count;
}
