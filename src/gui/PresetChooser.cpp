#include "PresetChooser.h"

#include "ProjectMWrapper.h"

#include <imgui.h>
#include <imgui_stdlib.h>

#include <Poco/String.h>
#include <Poco/StringTokenizer.h>

#include <Poco/Util/Application.h>

#include <utility>

PresetChooser::PresetChooser(PresetPlaylist::ItemList currentPlaylistItems,
                             const PresetPlaylist::ItemList::size_type initialSelectionIndex)
    : _playlistItems(std::move(currentPlaylistItems))
    , _selectedIndex(initialSelectionIndex)
{
}

bool PresetChooser::Draw()
{
    std::string windowId = "Preset Quick Search###PresetChooser";

    if (!_isOpen)
    {
        _isOpen = true;
        // Immediately scroll to the current playlist index
        _selectionChanged = true;

        // Skip this frame so ImGui doesn't add the hotkey to the text field input buffer
        return true;
    }

    if (_firstDraw)
    {
        ImGui::OpenPopup(windowId.c_str());
    }

    ImGui::SetNextWindowSize(ImVec2(1050, 0), ImGuiCond_FirstUseEver);
    if (ImGui::BeginPopupModal(windowId.c_str(), &_isOpen, ImGuiWindowFlags_NoCollapse))
    {
        ImGui::PushStyleColor(ImGuiCol_Text, 0x80808080);
        ImGui::Text("Use Up/Down/PgUp/PgDown/Home/End to navigate, ENTER to select and ESC to exit");
        ImGui::PopStyleColor();

        ImGui::SetNextItemWidth(-1);
        if (_firstDraw)
        {
            ImGui::SetKeyboardFocusHere(0);
        }
        ImGui::InputText("##FilterText", &_searchPhrase, ImGuiInputTextFlags_None);

        ImGui::Separator();

        float listItemHeight{};
        float listHeight{};

        bool listIsFiltered = UpdateFilteredList();
        if (listIsFiltered)
        {
            DrawFilteredItems(listItemHeight, listHeight);
        }
        else
        {
            DrawAllItems(listItemHeight, listHeight);
        }

        int itemsPerPage{};
        if (listItemHeight > 0 && listHeight > 0)
        {
            itemsPerPage = static_cast<int>(listHeight / listItemHeight);
        }

        // Navigation
        if (listIsFiltered)
        {
            NavigateFilteredItems(itemsPerPage);
        }
        else
        {
            NavigateAllItems(itemsPerPage);
        }

        if (ImGui::IsKeyPressed(ImGuiKey_Enter))
        {
            PlayPresetIndex(_selectedIndex);
            ImGui::CloseCurrentPopup();
            _isOpen = false;
        }

        if (ImGui::IsKeyPressed(ImGuiKey_Escape))
        {
            ImGui::CloseCurrentPopup();
            _isOpen = false;
        }

        ImGui::EndPopup();
    }
    else
    {
        ImGui::CloseCurrentPopup();
        _isOpen = false;
    }

    _firstDraw = false;

    return _isOpen;
}

void PresetChooser::NavigateAllItems(const int itemsPerPage)
{
    if (_playlistItems.empty())
    {
        return;
    }

    if (ImGui::IsKeyPressed(ImGuiKey_UpArrow))
    {
        if (_selectedIndex > 0)
        {
            --_selectedIndex;
            _selectionChanged = true;
        }
    }

    if (ImGui::IsKeyPressed(ImGuiKey_DownArrow))
    {
        if (_selectedIndex < _playlistItems.size() - 1)
        {
            ++_selectedIndex;
            _selectionChanged = true;
        }
    }

    if (ImGui::IsKeyPressed(ImGuiKey_PageUp))
    {
        if (_selectedIndex > 0)
        {
            if (_selectedIndex > itemsPerPage)
            {
                _selectedIndex -= itemsPerPage;
            }
            else
            {
                _selectedIndex = 0;
            }
            _selectionChanged = true;
        }
    }

    if (ImGui::IsKeyPressed(ImGuiKey_PageDown))
    {
        if (_selectedIndex < _playlistItems.size() - 1)
        {
            if (_selectedIndex + itemsPerPage < _playlistItems.size() - 1)
            {
                _selectedIndex += itemsPerPage;
            }
            else
            {
                _selectedIndex = _playlistItems.size() - 1;
            }
            _selectionChanged = true;
        }
    }

    if (ImGui::IsKeyPressed(ImGuiKey_Home))
    {
        _selectedIndex = 0;
        _selectionChanged = true;
    }

    if (ImGui::IsKeyPressed(ImGuiKey_End))
    {
        _selectedIndex = _playlistItems.size() - 1;
        _selectionChanged = true;
    }
}

void PresetChooser::NavigateFilteredItems(int itemsPerPage)
{
    if (_playlistItemsFiltered.empty())
    {
        return;
    }

    if (ImGui::IsKeyPressed(ImGuiKey_UpArrow))
    {
        if (_selectedFilteredIndex > 0)
        {
            --_selectedFilteredIndex;
            _selectedIndex = _playlistItemsFiltered.at(_selectedFilteredIndex).itemIndex;
            _selectionChanged = true;
        }
    }

    if (ImGui::IsKeyPressed(ImGuiKey_DownArrow))
    {
        if (_selectedFilteredIndex < _playlistItemsFiltered.size() - 1)
        {
            ++_selectedFilteredIndex;
            _selectedIndex = _playlistItemsFiltered.at(_selectedFilteredIndex).itemIndex;
            _selectionChanged = true;
        }
    }

    if (ImGui::IsKeyPressed(ImGuiKey_PageUp))
    {
        if (_selectedFilteredIndex > 0)
        {
            if (_selectedFilteredIndex > itemsPerPage)
            {
                _selectedFilteredIndex -= itemsPerPage;
            }
            else
            {
                _selectedFilteredIndex = 0;
            }
            _selectedIndex = _playlistItemsFiltered.at(_selectedFilteredIndex).itemIndex;
            _selectionChanged = true;
        }
    }

    if (ImGui::IsKeyPressed(ImGuiKey_PageDown))
    {
        if (_selectedFilteredIndex < _playlistItemsFiltered.size() - 1)
        {
            if (_selectedFilteredIndex + itemsPerPage < _playlistItemsFiltered.size() - 1)
            {
                _selectedFilteredIndex += itemsPerPage;
            }
            else
            {
                _selectedFilteredIndex = _playlistItemsFiltered.size() - 1;
            }
            _selectedIndex = _playlistItemsFiltered.at(_selectedFilteredIndex).itemIndex;
            _selectionChanged = true;
        }
    }

    if (ImGui::IsKeyPressed(ImGuiKey_Home))
    {
        _selectedFilteredIndex = 0;
        _selectedIndex = _playlistItemsFiltered.at(_selectedFilteredIndex).itemIndex;
        _selectionChanged = true;
    }

    if (ImGui::IsKeyPressed(ImGuiKey_End))
    {
        _selectedFilteredIndex = _playlistItemsFiltered.size() - 1;
        _selectedIndex = _playlistItemsFiltered.at(_selectedFilteredIndex).itemIndex;
        _selectionChanged = true;
    }
}

bool PresetChooser::UpdateFilteredList()
{
    if (_searchPhrase == _filterPhrase)
    {
        return !_filterPhrase.empty();
    }

    // Filter items word by word and case-insensitively to allow more flexible searches
    Poco::StringTokenizer tok(Poco::toLower(_searchPhrase), " ", Poco::StringTokenizer::TOK_IGNORE_EMPTY);

    _playlistItemsFiltered.clear();
    if (tok.count() == 0)
    {
        _filterPhrase = "";
        return false;
    }

    _filterPhrase = _searchPhrase;

    bool selectedIndexInFilteredList{false};
    size_t index{};
    for (const auto& item : _playlistItems)
    {
        std::string lowerCaseName = Poco::toLower(item.PresetName());
        if (std::all_of(tok.begin(), tok.end(),
                        [&](const std::string& word) {
                            return lowerCaseName.find(word) != std::string::npos;
                        }))
        {
            if (index == _selectedIndex)
            {
                _selectedFilteredIndex = _playlistItemsFiltered.size();
                selectedIndexInFilteredList = true;
            }
            _playlistItemsFiltered.emplace_back(item, index);
        }
        ++index;
    }

    if (!selectedIndexInFilteredList)
    {
        _selectedFilteredIndex = 0;
        if (!_playlistItemsFiltered.empty())
        {
            _selectedIndex = _playlistItemsFiltered.at(0).itemIndex;
        }
        else
        {
            _selectedIndex = 0;
        }
    }

    _selectionChanged = true;

    return true;
}

void PresetChooser::DrawAllItems(float& listItemHeight, float& listHeight)
{
    if (ImGui::BeginListBox("##FilteredPresets", ImVec2(-1, -ImGui::GetTextLineHeight() - ImGui::GetStyle().FramePadding.y * 2 - 4)))
    {
        ImGuiListClipper clipper;
        clipper.Begin(_playlistItems.size());
        while (clipper.Step())
        {
            for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
            {
                std::string label = _playlistItems.at(i).PresetName() + "##" + std::to_string(i);
                bool popColor = false;
                if (i == _selectedIndex)
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, 0xFF00FF80);
                    popColor = true;
                }
                if (ImGui::Selectable(label.c_str(), i == _selectedIndex, ImGuiSelectableFlags_AllowDoubleClick))
                {
                    if (ImGui::IsMouseDoubleClicked(0))
                    {
                        PlayPresetIndex(i);
                        ImGui::CloseCurrentPopup();
                        _isOpen = false;
                    }
                    else
                    {
                        _selectedIndex = i;
                    }
                }
                if (popColor)
                {
                    ImGui::PopStyleColor();
                }
            }
        }

        listItemHeight = clipper.ItemsHeight;

        if (_selectionChanged)
        {
            float itemPosY = clipper.StartPosY + listItemHeight * _selectedIndex;
            ImGui::SetScrollFromPosY(itemPosY - ImGui::GetWindowPos().y);
            _selectionChanged = false;
        }

        ImGui::EndListBox();
    }

    listHeight = ImGui::GetItemRectSize().y;
}

void PresetChooser::DrawFilteredItems(float& listItemHeight, float& listHeight)
{
    // _selectedIndex always points to the original playlist index, so we can keep the selection
    // while changing the filter.
    if (ImGui::BeginListBox("##FilteredPresets", ImVec2(-1, -ImGui::GetTextLineHeight() - ImGui::GetStyle().FramePadding.y * 2 - 4)))
    {
        ImGuiListClipper clipper;
        clipper.Begin(static_cast<int>(_playlistItemsFiltered.size()));
        while (clipper.Step())
        {
            for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
            {
                std::string label = _playlistItemsFiltered.at(i).item.PresetName() + "##" + std::to_string(i);
                bool popColor = false;
                if (i == _selectedFilteredIndex)
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, 0xFF00FF80);
                    popColor = true;
                }
                if (ImGui::Selectable(label.c_str(), i == _selectedFilteredIndex, ImGuiSelectableFlags_AllowDoubleClick))
                {
                    if (ImGui::IsMouseDoubleClicked(0))
                    {
                        PlayPresetIndex(_playlistItemsFiltered.at(i).itemIndex);
                        ImGui::CloseCurrentPopup();
                        _isOpen = false;
                    }
                    else
                    {
                        _selectedFilteredIndex = i;
                        _selectedIndex = _playlistItemsFiltered.at(i).itemIndex;
                    }
                }
                if (popColor)
                {
                    ImGui::PopStyleColor();
                }
            }
        }

        listItemHeight = clipper.ItemsHeight;

        if (_selectionChanged)
        {
            float itemPosY = clipper.StartPosY + listItemHeight * _selectedFilteredIndex;
            ImGui::SetScrollFromPosY(itemPosY - ImGui::GetWindowPos().y);
            _selectionChanged = false;
        }

        ImGui::EndListBox();
    }

    listHeight = ImGui::GetItemRectSize().y;
}

void PresetChooser::PlayPresetIndex(PresetPlaylist::ItemList::size_type index)
{
    bool hardCut = !ImGui::GetIO().KeyShift;
    auto& projectMWrapper = Poco::Util::Application::instance().getSubsystem<ProjectMWrapper>();
    projectMWrapper.Playlist().CurrentIndex(index, hardCut);
}
