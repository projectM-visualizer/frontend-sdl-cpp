#include "gui/MainMenu.h"

#include "AudioCapture.h"
#include "IconsFontAwesome7.h"
#include "ProjectMSDLApplication.h"
#include "ProjectMWrapper.h"

#include "gui/ProjectMGUI.h"
#include "gui/SystemBrowser.h"

#include "notifications/PlaybackControlNotification.h"
#include "notifications/QuitNotification.h"
#include "notifications/UpdateWindowTitleNotification.h"

#include "imgui.h"

#include <Poco/NotificationCenter.h>


MainMenu::MainMenu(ProjectMGUI& gui)
    : _notificationCenter(Poco::NotificationCenter::defaultCenter())
    , _gui(gui)
    , _projectMWrapper(Poco::Util::Application::instance().getSubsystem<ProjectMWrapper>())
    , _audioCapture(Poco::Util::Application::instance().getSubsystem<AudioCapture>())
{
    _presetChooser.AllowedExtensions({".milk"});
    _presetChooser.MultiSelect(false);
}

void MainMenu::Draw()
{
    if (ImGui::BeginMainMenuBar())
    {
        DrawFileMenu();
        DrawPlaybackMenu();
        DrawOptionsMenu();
        DrawHelpMenu();

        ImGui::EndMainMenuBar();
    }

    if (_presetChooser.Draw())
    {
        auto selectedFile = _presetChooser.SelectedFiles();
        if (!selectedFile.empty())
        {
            // Open preset editor
            _gui.ShowPresetEditor(selectedFile.at(0).path());
        }
    }
}

void MainMenu::DrawFileMenu()
{
    if (ImGui::BeginMenu("File"))
    {
        if (ImGui::BeginMenu(ICON_FA_PENCIL " Preset Editor"))
        {
            if (ImGui::MenuItem(ICON_FA_PENCIL " Edit Current Preset", "Ctrl+e"))
            {
                auto currentPreset = _projectMWrapper.CurrentPresetFileName();
                if (currentPreset.empty())
                {
                    Poco::NotificationCenter::defaultCenter().postNotification(new DisplayToastNotification("No preset is currently loaded which can be edited."));
                }
                else
                {
                    _gui.ShowPresetEditor(currentPreset);
                }
            }
            if (ImGui::MenuItem(ICON_FA_FOLDER_OPEN " Select Preset From Disk...", "Ctrl+l"))
            {
                _presetChooser.Title("Select a Preset for Editing");
                _presetChooser.Show();
            }
            if (ImGui::MenuItem(ICON_FA_SQUARE_PLUS " Create New Preset", "Ctrl+Shift+n"))
            {
                _gui.ShowPresetEditor({});
            }

            ImGui::EndMenu();
        }

        ImGui::Separator();

        if (ImGui::MenuItem(ICON_FA_GEAR " Settings...", "Ctrl+s"))
        {
            _gui.ShowSettingsWindow();
        }

        ImGui::Separator();

        if (ImGui::MenuItem(ICON_FA_DOOR_OPEN " Quit projectM", "Ctrl+q"))
        {
            _notificationCenter.postNotification(new QuitNotification);
        }

        ImGui::EndMenu();
    }
}

void MainMenu::DrawPlaybackMenu()
{
    if (ImGui::BeginMenu("Playback"))
    {
        auto& app = ProjectMSDLApplication::instance();

        if (ImGui::MenuItem(ICON_FA_FORWARD_STEP " Play Next Preset", "n"))
        {
            _notificationCenter.postNotification(new PlaybackControlNotification(PlaybackControlNotification::Action::LastPreset));
        }
        if (ImGui::MenuItem(ICON_FA_BACKWARD_STEP " Play Previous Preset", "p"))
        {
            _notificationCenter.postNotification(new PlaybackControlNotification(PlaybackControlNotification::Action::PreviousPreset));
        }
        if (ImGui::MenuItem(ICON_FA_ROTATE_LEFT " Go Back One Preset", "Backspace"))
        {
            _notificationCenter.postNotification(new PlaybackControlNotification(PlaybackControlNotification::Action::LastPreset));
        }
        if (ImGui::MenuItem(ICON_FA_WAND_MAGIC_SPARKLES " Random Preset", "r"))
        {
            _notificationCenter.postNotification(new PlaybackControlNotification(PlaybackControlNotification::Action::RandomPreset));
        }

        ImGui::Separator();

        if (ImGui::MenuItem(ICON_FA_LOCK " Lock Preset", "Spacebar", app.config().getBool("projectM.presetLocked", false)))
        {
            _notificationCenter.postNotification(new PlaybackControlNotification(PlaybackControlNotification::Action::TogglePresetLocked));
        }
        if (ImGui::MenuItem(ICON_FA_SHUFFLE " Enable Shuffle", "y", app.config().getBool("projectM.shuffleEnabled", true)))
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
}

void MainMenu::DrawOptionsMenu()
{
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
}

void MainMenu::DrawHelpMenu()
{
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

        if (ImGui::MenuItem(ICON_FA_ARROW_UP_RIGHT_FROM_SQUARE " Visit the projectM Wiki on GitHub"))
        {
            SystemBrowser::OpenURL("https://github.com/projectM-visualizer/projectm/wiki");
        }
        if (ImGui::MenuItem(ICON_FA_ARROW_UP_RIGHT_FROM_SQUARE " Report a Bug or Request a Feature"))
        {
            SystemBrowser::OpenURL("https://github.com/projectM-visualizer/projectm/issues/new/choose");
        }
        if (ImGui::MenuItem(ICON_FA_ARROW_UP_RIGHT_FROM_SQUARE " Sponsor projectM on OpenCollective"))
        {
            SystemBrowser::OpenURL("https://opencollective.com/projectm");
        }

        ImGui::EndMenu();
    }
}
