#include "gui/PresetBrowser.h"

#include "ProjectMWrapper.h"

#include "imgui.h"

#include <Poco/File.h>
#include <Poco/Path.h>
#include <Poco/NotificationCenter.h>

#include "notifications/DisplayToastNotification.h"

void PresetBrowser::Show()
{
    _visible = true;
}

const std::string& PresetBrowser::SelectedFolderPath() const
{
    return _selectedFolderPath;
}

void PresetBrowser::Draw(ProjectMWrapper& projectMWrapper)
{
    if (!_visible)
    {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(900, 700), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Preset Browser", &_visible))
    {
        // Folder Selection Section
        ImGui::Text("Step 1: Select a Preset Folder");
        ImGui::Separator();

        ImGui::BeginGroup();
        ImGui::Text("Selected Folder: %s", _selectedFolderPath.empty() ? "(none)" : _selectedFolderPath.c_str());
        ImGui::SameLine();
        if (ImGui::Button("Select Folder...##folder"))
        {
            _browseMode = BrowseMode::Folder;
            _folderChooser.Title("Browse Preset Folders");
            _folderChooser.CurrentDirectory(std::getenv("HOME") ? std::getenv("HOME") : "/");
            _folderChooser.Show();
        }
        ImGui::EndGroup();

        // Draw folder chooser in Folder mode
        if (_browseMode == BrowseMode::Folder && _folderChooser.Draw())
        {
            auto selectedFiles = _folderChooser.SelectedFiles();
            if (!selectedFiles.empty() && selectedFiles[0].isDirectory())
            {
                _selectedFolderPath = selectedFiles[0].path();
                poco_information_f1(_logger, "Selected preset folder: %s", _selectedFolderPath);
                _browseMode = BrowseMode::Files; // Auto-switch to file browsing mode
            }
        }

        ImGui::Spacing();
        ImGui::Separator();

        // File Selection Section (only show if a folder is selected)
        if (!_selectedFolderPath.empty())
        {
            ImGui::Text("Step 2: Select Individual Presets (optional)");
            ImGui::Separator();

            ImGui::Text("Browse individual preset files in the selected folder:");
            ImGui::SameLine();
            if (ImGui::Button("Browse Files...##files"))
            {
                _browseMode = BrowseMode::Files;
                _fileChooser.Title("Select Preset Files");
                _fileChooser.CurrentDirectory(_selectedFolderPath);
                _fileChooser.AllowedExtensions({"milk", "prjm"});
                _fileChooser.MultiSelect(true);
                _fileChooser.Show();
            }

            ImGui::Spacing();

            // Draw file chooser in Files mode
            if (_browseMode == BrowseMode::Files && _fileChooser.Draw())
            {
                auto selectedFiles = _fileChooser.SelectedFiles();
                if (!selectedFiles.empty())
                {
                    // Load the first selected file directly and display it
                    const auto& firstFile = selectedFiles[0];
                    if (!firstFile.isDirectory())
                    {
                        projectMWrapper.PlayPresetFile(firstFile.path());
                        std::string fileName = Poco::Path(firstFile.path()).getFileName();
                        Poco::NotificationCenter::defaultCenter().postNotification(
                            new DisplayToastNotification(Poco::format("Now playing: %s", fileName)));
                        poco_information_f1(_logger, "Playing preset file: %s", firstFile.path());
                    }
                }
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Text("Step 3: Load Presets");
            ImGui::Separator();

            ImGui::Spacing();
            if (ImGui::Button("Add Folder to Playlist", ImVec2(200, 0)))
            {
                projectMWrapper.AddPresetPath(_selectedFolderPath);
                Poco::NotificationCenter::defaultCenter().postNotification(
                    new DisplayToastNotification(Poco::format("Added preset folder: %s", _selectedFolderPath)));
                poco_information_f1(_logger, "Added folder to playlist: %s", _selectedFolderPath);
            }
            ImGui::SameLine();
            if (ImGui::Button("Load Folder as Primary", ImVec2(200, 0)))
            {
                projectMWrapper.LoadPresetPath(_selectedFolderPath);
                Poco::NotificationCenter::defaultCenter().postNotification(
                    new DisplayToastNotification(Poco::format("Loaded preset folder: %s", _selectedFolderPath)));
                poco_information_f1(_logger, "Loaded folder as primary: %s", _selectedFolderPath);
            }
        }
        else
        {
            ImGui::TextDisabled("(Select a folder first to enable file and loading options)");
        }
    }
    ImGui::End();
}
