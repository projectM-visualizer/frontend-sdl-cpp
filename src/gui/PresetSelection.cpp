#include "PresetSelection.h"

#include "PlaylistManager.h"
#include "ProjectMSDLApplication.h"
#include "ProjectMWrapper.h"

#include "notifications/DisplayToastNotification.h"
#include "notifications/PlaybackControlNotification.h"

#include "imgui.h"

#include <Poco/NotificationCenter.h>
#include <Poco/Path.h>
#include <Poco/Util/Application.h>

PresetSelection::PresetSelection(ProjectMWrapper& projectMWrapper)
    : _projectMWrapper(projectMWrapper)
{
}

void PresetSelection::Show()
{
    _visible = true;
    _dirty = true;
}

void PresetSelection::Draw()
{
    if (!_visible)
    {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(500, 550), ImGuiCond_FirstUseEver);

    std::string windowTitle = "Playlist Browser###PlaylistBrowser";
    if (ImGui::Begin(windowTitle.c_str(), &_visible))
    {
        auto playlist = _projectMWrapper.Playlist();
        if (!playlist)
        {
            ImGui::TextUnformatted("No playlist loaded.");
            ImGui::End();
            return;
        }

        // Refresh cached entries when playlist changes
        if (_dirty)
        {
            RefreshPlaylist();
            _dirty = false;
        }

        // --- Filter input ---
        ImGui::SetNextItemWidth(-1);
        if (ImGui::InputTextWithHint("##PlaylistFilter", "Filter presets...",
                                     _filterBuffer, IM_ARRAYSIZE(_filterBuffer)))
        {
            _lastFilter = _filterBuffer;
        }

        // Clear filter button
        ImGui::SameLine();
        if (ImGui::Button("X"))
        {
            _filterBuffer[0] = '\0';
            _lastFilter.clear();
        }
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Clear filter");
        }

        // Apply filter
        std::string filter(_filterBuffer);
        _filteredEntries.clear();
        if (filter.empty())
        {
            _filteredEntries = _cachedEntries;
        }
        else
        {
            for (const auto& entry : _cachedEntries)
            {
                // Case-insensitive substring match on basename
                auto it = std::search(
                    entry.basename.begin(), entry.basename.end(),
                    filter.begin(), filter.end(),
                    [](char a, char b) { return std::tolower(a) == std::tolower(b); });
                if (it != entry.basename.end())
                {
                    _filteredEntries.push_back(entry);
                }
            }
        }

        // --- Playlist list ---
        uint32_t currentPosition = projectm_playlist_get_position(playlist);
        uint32_t totalSize = projectm_playlist_size(playlist);

        ImGui::SeparatorText("Presets");

        // Reserve enough height for the list
        ImVec2 listSize = ImVec2(-1, -ImGui::GetFrameHeightWithSpacing() * 2.5f);
        if (ImGui::BeginChild("PlaylistItems", listSize, ImGuiChildFlags_Borders))
        {
            if (_filteredEntries.empty())
            {
                ImGui::TextUnformatted(filter.empty()
                                           ? "Playlist is empty."
                                           : "No presets match the filter.");
            }
            else
            {
                for (const auto& entry : _filteredEntries)
                {
                    bool isCurrent = (entry.index == currentPosition);

                    // Highlight the currently playing preset
                    if (isCurrent)
                    {
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 0.9f, 0.3f, 1.0f));
                    }

                    // Display with index number
                    char label[512];
                    snprintf(label, sizeof(label), "%u. %s", entry.index + 1, entry.basename.c_str());

                    if (ImGui::Selectable(label, isCurrent))
                    {
                        projectm_playlist_set_position(playlist, entry.index, false);
                        _dirty = true; // Position may have changed via callback
                    }

                    if (isCurrent)
                    {
                        ImGui::PopStyleColor();
                    }

                    // Show full path on hover
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::BeginTooltip();
                        ImGui::TextUnformatted(entry.fullPath.c_str());
                        ImGui::EndTooltip();
                    }
                }
            }
        }
        ImGui::EndChild();

        // --- Bottom bar ---
        bool shuffleEnabled = projectm_playlist_get_shuffle(playlist);

        ImGui::Separator();
        ImGui::Text("Shuffle: %s", shuffleEnabled ? "ON" : "OFF");
        ImGui::SameLine();

        if (ImGui::Button(shuffleEnabled ? "Disable" : "Enable"))
        {
            Poco::NotificationCenter::defaultCenter().postNotification(
                new PlaybackControlNotification(PlaybackControlNotification::Action::ToggleShuffle));
            _dirty = true;
        }

        ImGui::SameLine();

        ImGui::Text("| %u / %u presets", currentPosition + 1, totalSize);
        ImGui::SameLine();

        if (ImGui::Button("Refresh"))
        {
            _dirty = true;
        }
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Reload playlist from disk");
        }

        ImGui::Spacing();

        // Navigation buttons
        if (ImGui::Button("< Prev"))
        {
            Poco::NotificationCenter::defaultCenter().postNotification(
                new PlaybackControlNotification(PlaybackControlNotification::Action::PreviousPreset, true));
            _dirty = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Next >"))
        {
            Poco::NotificationCenter::defaultCenter().postNotification(
                new PlaybackControlNotification(PlaybackControlNotification::Action::NextPreset, true));
            _dirty = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Random"))
        {
            Poco::NotificationCenter::defaultCenter().postNotification(
                new PlaybackControlNotification(PlaybackControlNotification::Action::RandomPreset));
            _dirty = true;
        }

        ImGui::Spacing();

        // --- Export section ---
        ImGui::SeparatorText("Export Playlist");

        if (!_showExport)
        {
            if (ImGui::Button("Show Export Options"))
            {
                _showExport = true;
                // Pre-fill with saved export path from user config
                try
                {
                    auto& app = dynamic_cast<ProjectMSDLApplication&>(
                        Poco::Util::Application::instance());
                    std::string savedPath = app.UserConfiguration()->getString(
                        "playlist.exportPath", "");
                    strncpy(_exportPathBuffer, savedPath.c_str(), sizeof(_exportPathBuffer) - 1);
                }
                catch (...) {}
            }
        }
        else
        {
            ImGui::SetNextItemWidth(-1);
            ImGui::InputTextWithHint("##ExportPath", "Destination folder path...",
                                     _exportPathBuffer, IM_ARRAYSIZE(_exportPathBuffer));

            if (ImGui::Button("Export All to Folder"))
            {
                std::string exportPath(_exportPathBuffer);
                if (exportPath.empty())
                {
                    Poco::NotificationCenter::defaultCenter().postNotification(
                        new DisplayToastNotification("Please enter a destination folder path."));
                }
                else
                {
                    auto& playlistMgr = Poco::Util::Application::instance()
                                            .getSubsystem<PlaylistManager>();
                    int32_t count = playlistMgr.Export(exportPath);

                    if (count > 0)
                    {
                        // Save export path for next time
                        try
                        {
                            auto& app = dynamic_cast<ProjectMSDLApplication&>(
                                Poco::Util::Application::instance());
                            app.UserConfiguration()->setString("playlist.exportPath", exportPath);
                        }
                        catch (...) {}

                        Poco::NotificationCenter::defaultCenter().postNotification(
                            new DisplayToastNotification(
                                Poco::format("Exported %d presets.", static_cast<int>(count))));
                    }
                    else if (count == 0)
                    {
                        Poco::NotificationCenter::defaultCenter().postNotification(
                            new DisplayToastNotification("Playlist is empty."));
                    }
                    else
                    {
                        Poco::NotificationCenter::defaultCenter().postNotification(
                            new DisplayToastNotification("Export failed. Check the log for details."));
                    }
                }
            }

            ImGui::SameLine();
            if (ImGui::Button("Hide"))
            {
                _showExport = false;
            }
        }
    }
    ImGui::End();
}

void PresetSelection::RefreshPlaylist()
{
    _cachedEntries.clear();

    auto playlist = _projectMWrapper.Playlist();
    if (!playlist)
    {
        return;
    }

    uint32_t size = projectm_playlist_size(playlist);
    if (size == 0)
    {
        return;
    }

    // Fetch all items in one call for efficiency
    char** items = projectm_playlist_items(playlist, 0, size);
    if (!items)
    {
        return;
    }

    _cachedEntries.reserve(size);
    for (uint32_t i = 0; i < size && items[i] != nullptr; ++i)
    {
        PlaylistEntry entry;
        entry.index = i;
        entry.fullPath = items[i];
        entry.basename = Basename(entry.fullPath);
        _cachedEntries.push_back(std::move(entry));
    }

    projectm_playlist_free_string_array(items);
}

std::string PresetSelection::Basename(const std::string& path)
{
    // Use Poco::Path for cross-platform basename extraction
    Poco::Path p(path);
    return p.getFileName();
}
