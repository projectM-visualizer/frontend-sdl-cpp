#include "gui/MainMenu.h"

#include "AudioCapture.h"
#include "PlaylistManager.h"
#include "ProjectMSDLApplication.h"
#include "ProjectMWrapper.h"

#include "gui/ProjectMGUI.h"
#include "gui/SystemBrowser.h"

#include "notifications/DisplayToastNotification.h"
#include "notifications/PlaybackControlNotification.h"
#include "notifications/QuitNotification.h"
#include "notifications/UpdateWindowTitleNotification.h"

#include "imgui.h"

#include <Poco/NotificationCenter.h>
#include <Poco/Util/Application.h>


MainMenu::MainMenu(ProjectMGUI& gui)
    : _notificationCenter(Poco::NotificationCenter::defaultCenter())
    , _gui(gui)
    , _projectMWrapper(Poco::Util::Application::instance().getSubsystem<ProjectMWrapper>())
    , _audioCapture(Poco::Util::Application::instance().getSubsystem<AudioCapture>())
    , _playlistManager(Poco::Util::Application::instance().getSubsystem<PlaylistManager>())
{
}

void MainMenu::Draw()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Settings...", "Ctrl+s"))
            {
                _gui.ShowSettingsWindow();
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Quit projectM", "Ctrl+q"))
            {
                _notificationCenter.postNotification(new QuitNotification);
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Playback"))
        {
            auto& app = ProjectMSDLApplication::instance();

            if (ImGui::MenuItem("Play Next Preset", "n"))
            {
                _notificationCenter.postNotification(new PlaybackControlNotification(PlaybackControlNotification::Action::LastPreset));
            }
            if (ImGui::MenuItem("Play Previous Preset", "p"))
            {
                _notificationCenter.postNotification(new PlaybackControlNotification(PlaybackControlNotification::Action::PreviousPreset));
            }
            if (ImGui::MenuItem("Go Back One Preset", "Backspace"))
            {
                _notificationCenter.postNotification(new PlaybackControlNotification(PlaybackControlNotification::Action::LastPreset));
            }
            if (ImGui::MenuItem("Random Preset", "r"))
            {
                _notificationCenter.postNotification(new PlaybackControlNotification(PlaybackControlNotification::Action::RandomPreset));
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Lock Preset", "Spacebar", app.config().getBool("projectM.presetLocked", false)))
            {
                _notificationCenter.postNotification(new PlaybackControlNotification(PlaybackControlNotification::Action::TogglePresetLocked));
            }
            if (ImGui::MenuItem("Enable Shuffle", "y", app.config().getBool("projectM.shuffleEnabled", true)))
            {
                _notificationCenter.postNotification(new PlaybackControlNotification(PlaybackControlNotification::Action::ToggleShuffle));
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Copy Current Preset Filename", "Ctrl+c"))
            {
                _projectMWrapper.PresetFileNameToClipboard();
            }

            ImGui::EndMenu();
        }

        DrawPlaylistsMenu();

        if (ImGui::BeginMenu("Options"))
        {
            auto& app = ProjectMSDLApplication::instance();

            if (ImGui::BeginMenu("Audio Capture Device"))
            {
                auto devices = _audioCapture.AudioDeviceList();
                auto currentIndex = _audioCapture.AudioDeviceIndex();

                for (const auto& device : devices)
                {
                    if (ImGui::MenuItem(device.second.c_str(), "", device.first == currentIndex))
                    {
                        _audioCapture.AudioDeviceIndex(device.first);
                    }
                }
                ImGui::EndMenu();
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Display Toast Messages", "", app.config().getBool("projectM.displayToasts", true)))
            {
                app.UserConfiguration()->setBool("projectM.displayToasts", !app.config().getBool("projectM.displayToasts", true));
            }
            if (ImGui::MenuItem("Display Preset Name in Window Title", "", app.config().getBool("window.displayPresetNameInTitle", true)))
            {
                app.UserConfiguration()->setBool("window.displayPresetNameInTitle", !app.config().getBool("window.displayPresetNameInTitle", true));
                _notificationCenter.postNotification(new UpdateWindowTitleNotification);
            }

            ImGui::Separator();

            float beatSensitivity = projectm_get_beat_sensitivity(_projectMWrapper.ProjectM());
            if (ImGui::SliderFloat("Beat Sensitivity", &beatSensitivity, 0.0f, 2.0f))
            {
                projectm_set_beat_sensitivity(_projectMWrapper.ProjectM(), beatSensitivity);
                app.UserConfiguration()->setDouble("projectM.beatSensitivity", beatSensitivity);
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("Quick Help..."))
            {
                _gui.ShowHelpWindow();
            }

            ImGui::Separator();

            if (ImGui::MenuItem("About projectM..."))
            {
                _gui.ShowAboutWindow();
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Visit the projectM Wiki on GitHub"))
            {
                SystemBrowser::OpenURL("https://github.com/projectM-visualizer/projectm/wiki");
            }
            if (ImGui::MenuItem("Report a Bug or Request a Feature"))
            {
                SystemBrowser::OpenURL("https://github.com/projectM-visualizer/projectm/issues/new/choose");
            }
            if (ImGui::MenuItem("Sponsor projectM on OpenCollective"))
            {
                SystemBrowser::OpenURL("https://opencollective.com/projectm");
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    // Render popups outside the menu bar
    DrawSavePlaylistPopup();
    DrawNewPlaylistPopup();
    DrawManagePlaylistsPopup();
}

void MainMenu::DrawPlaylistsMenu()
{
    if (!ImGui::BeginMenu("Playlists"))
    {
        return;
    }

    if (ImGui::MenuItem("Save Current Playlist..."))
    {
        _playlistNameBuffer[0] = '\0';
        _showSavePlaylistPopup = true;
    }

    if (ImGui::MenuItem("New Empty Playlist..."))
    {
        _playlistNameBuffer[0] = '\0';
        _showNewPlaylistPopup = true;
    }

    ImGui::Separator();

    // List all saved playlists
    auto playlists = _playlistManager.ListPlaylists();
    if (playlists.empty())
    {
        ImGui::MenuItem("(No saved playlists)", "", false, false);
    }
    else
    {
        for (const auto& playlist : playlists)
        {
            std::string label = playlist.first + " (" + std::to_string(playlist.second) + ")";
            if (ImGui::MenuItem(label.c_str()))
            {
                _playlistManager.Load(playlist.first);
            }
        }
    }

    ImGui::Separator();

    if (ImGui::MenuItem("Manage Playlists..."))
    {
        _showManagePlaylistsPopup = true;
    }

    ImGui::EndMenu();
}

void MainMenu::DrawSavePlaylistPopup()
{
    if (!_showSavePlaylistPopup)
    {
        return;
    }

    ImGui::OpenPopup("Save Playlist As");

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Save Playlist As", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextUnformatted("Enter a name for the playlist:");

        ImGui::SetNextItemWidth(300);
        if (ImGui::InputText("##playlist_name", _playlistNameBuffer, sizeof(_playlistNameBuffer),
                             ImGuiInputTextFlags_EnterReturnsTrue))
        {
            if (_playlistNameBuffer[0] != '\0')
            {
                if (_playlistManager.Save(_playlistNameBuffer))
                {
                    _notificationCenter.postNotification(
                        new DisplayToastNotification(std::string("Saved playlist: ") + _playlistNameBuffer));
                }
                else
                {
                    _notificationCenter.postNotification(
                        new DisplayToastNotification("Failed to save playlist (may be empty)"));
                }
                _showSavePlaylistPopup = false;
                ImGui::CloseCurrentPopup();
            }
        }

        ImGui::Spacing();

        if (ImGui::Button("Save", ImVec2(120, 0)))
        {
            if (_playlistNameBuffer[0] != '\0')
            {
                if (_playlistManager.Save(_playlistNameBuffer))
                {
                    _notificationCenter.postNotification(
                        new DisplayToastNotification(std::string("Saved playlist: ") + _playlistNameBuffer));
                }
                else
                {
                    _notificationCenter.postNotification(
                        new DisplayToastNotification("Failed to save playlist (may be empty)"));
                }
                _showSavePlaylistPopup = false;
                ImGui::CloseCurrentPopup();
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            _showSavePlaylistPopup = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void MainMenu::DrawNewPlaylistPopup()
{
    if (!_showNewPlaylistPopup)
    {
        return;
    }

    ImGui::OpenPopup("New Empty Playlist");

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("New Empty Playlist", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextUnformatted("Enter a name for the new playlist:");

        ImGui::SetNextItemWidth(300);
        if (ImGui::InputText("##new_playlist_name", _playlistNameBuffer, sizeof(_playlistNameBuffer),
                             ImGuiInputTextFlags_EnterReturnsTrue))
        {
            if (_playlistNameBuffer[0] != '\0')
            {
                // Create an empty playlist file and clear current
                _playlistManager.CreateEmpty(_playlistNameBuffer);
                projectm_playlist_clear(_projectMWrapper.Playlist());

                _notificationCenter.postNotification(
                    new DisplayToastNotification(std::string("Created empty playlist: ") + _playlistNameBuffer));
                _showNewPlaylistPopup = false;
                ImGui::CloseCurrentPopup();
            }
        }

        ImGui::Spacing();

        if (ImGui::Button("Create", ImVec2(120, 0)))
        {
            if (_playlistNameBuffer[0] != '\0')
            {
                _playlistManager.CreateEmpty(_playlistNameBuffer);
                projectm_playlist_clear(_projectMWrapper.Playlist());

                _notificationCenter.postNotification(
                    new DisplayToastNotification(std::string("Created empty playlist: ") + _playlistNameBuffer));
                _showNewPlaylistPopup = false;
                ImGui::CloseCurrentPopup();
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            _showNewPlaylistPopup = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void MainMenu::DrawManagePlaylistsPopup()
{
    if (!_showManagePlaylistsPopup)
    {
        return;
    }

    ImGui::OpenPopup("Manage Playlists");

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(450, 350), ImGuiCond_Appearing);

    if (ImGui::BeginPopupModal("Manage Playlists", &_showManagePlaylistsPopup))
    {
        auto playlists = _playlistManager.ListPlaylists();

        if (playlists.empty())
        {
            ImGui::TextUnformatted("No saved playlists found.");
            ImGui::Spacing();
            ImGui::TextUnformatted("Add presets by dropping .milk files onto the window,");
            ImGui::TextUnformatted("then use \"Save Current Playlist...\" to save them.");
        }
        else
        {
            ImGui::TextUnformatted("Click Load to switch to a playlist. Right-click a name to rename.");
            ImGui::Separator();

            if (ImGui::BeginTable("playlists_table", 3,
                                  ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                                      ImGuiTableFlags_Resizable))
            {
                ImGui::TableSetupColumn("Playlist", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Presets", ImGuiTableColumnFlags_WidthFixed, 70.0f);
                ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed, 160.0f);
                ImGui::TableHeadersRow();

                std::string playlistToDelete;

                for (const auto& playlist : playlists)
                {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);

                    // Editable name via double-click
                    ImGui::TextUnformatted(playlist.first.c_str());

                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%u", playlist.second);

                    ImGui::TableSetColumnIndex(2);
                    ImGui::PushID(playlist.first.c_str());

                    std::string loadLabel = "Load##" + playlist.first;
                    if (ImGui::SmallButton(loadLabel.c_str()))
                    {
                        _playlistManager.Load(playlist.first);
                    }

                    ImGui::SameLine();

                    std::string deleteLabel = "Delete##" + playlist.first;
                    if (ImGui::SmallButton(deleteLabel.c_str()))
                    {
                        playlistToDelete = playlist.first;
                    }

                    ImGui::PopID();
                }

                ImGui::EndTable();

                // Handle deletion outside the table iteration
                if (!playlistToDelete.empty())
                {
                    ImGui::OpenPopup("Confirm Delete");
                    _playlistNameBuffer[0] = '\0';
                    // Copy the name for the confirmation popup
                    strncpy(_playlistNameBuffer, playlistToDelete.c_str(), sizeof(_playlistNameBuffer) - 1);
                }
            }
        }

        ImGui::Spacing();
        ImGui::Separator();
        if (ImGui::Button("Close", ImVec2(120, 0)))
        {
            _showManagePlaylistsPopup = false;
            ImGui::CloseCurrentPopup();
        }

        // Confirmation popup for deletion
        if (ImGui::BeginPopupModal("Confirm Delete", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Delete playlist \"%s\"?", _playlistNameBuffer);
            ImGui::TextUnformatted("This cannot be undone.");

            ImGui::Spacing();

            if (ImGui::Button("Delete", ImVec2(120, 0)))
            {
                _playlistManager.Delete(_playlistNameBuffer);
                _notificationCenter.postNotification(
                    new DisplayToastNotification(std::string("Deleted playlist: ") + _playlistNameBuffer));
                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();

            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        ImGui::EndPopup();
    }
}
