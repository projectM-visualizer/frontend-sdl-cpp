#pragma once

#include <utility>

#include "PresetPlaylist.h"

class ProjectMGUI;

/**
 * @class PresetChooser
 * @brief Displays a simple search window to quickly filter and select a preset from the current playlist.
 *
 * This UI window will be displayed if the menu bar isn't open and allows
 * to quickly filter and search the current playlist.
 */
class PresetChooser
{
public:
    PresetChooser() = delete;

    explicit PresetChooser(PresetPlaylist::ItemList currentPlaylistItems,
                           PresetPlaylist::ItemList::size_type initialSelectionIndex);

    virtual ~PresetChooser() = default;

    /**
     * @brief Draws the preset chooser window.
     */
    bool Draw();

private:
    struct ItemWithIndex {
        explicit ItemWithIndex(PresetPlaylist::Item item, const PresetPlaylist::ItemList::size_type itemIndex)
            : item(std::move(item))
            , itemIndex(itemIndex)
        {
        }
        PresetPlaylist::Item item;
        PresetPlaylist::ItemList::size_type itemIndex{};
    };

    void NavigateAllItems(int itemsPerPage);

    void NavigateFilteredItems(int itemsPerPage);

    bool UpdateFilteredList();

    void DrawAllItems(float& listItemHeight, float& listHeight);

    void DrawFilteredItems(float& listItemHeight, float& listHeight);

    void PlayPresetIndex(PresetPlaylist::ItemList::size_type index);

    std::string _searchPhrase; //!< The currently entered search phrase.
    std::string _filterPhrase; //!< The search phrase used for the previous filtering.

    PresetPlaylist::ItemList _playlistItems; //!< Cached playlist items
    std::vector<ItemWithIndex> _playlistItemsFiltered; //!< Filtered playlist items, mapped to their original playlist index

    PresetPlaylist::ItemList::size_type _selectedIndex{};
    PresetPlaylist::ItemList::size_type _selectedFilteredIndex{};
    bool _selectionChanged{false};

    bool _isOpen{false}; //!< Becomes true if the popup has been opened.
    bool _firstDraw{true}; //!< true if the chooser hasn't been fully drawn yet.
};
