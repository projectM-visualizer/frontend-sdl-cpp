/**
 * @file PresetPlaylist.cpp
 * @brief
 *
 **/

#include "PresetPlaylist.h"

#include "notifications/PlaylistChanged.h"
#include "notifications/UpdateWindowTitle.h"

#include <projectM-4/playlist.h>

#include <Poco/NotificationCenter.h>
#include <Poco/Path.h>

POCO_IMPLEMENT_EXCEPTION(PlaylistEmptyException, Poco::Exception, "PlaylistEmptyException")

PresetPlaylist::Item::Item(std::string path)
    : _path(std::move(path))
{
    poco_assert_dbg(!_path.empty());

    if (!_path.empty())
    {
        _presetName = Poco::Path(_path).getBaseName();
    }
}

PresetPlaylist::PresetPlaylist()
    : _projectMPlaylist(projectm_playlist_create(nullptr))
{
    if (!_projectMPlaylist)
    {
        poco_error(_logger, "Failed to create the projectM preset playlist manager instance.");
        throw std::runtime_error("projectM Playlist Manager initialization failed");
    }

    projectm_playlist_set_preset_switched_event_callback(_projectMPlaylist, &PresetPlaylist::PresetSwitchedEvent, this);
}
PresetPlaylist::~PresetPlaylist()
{
    poco_assert(_projectMPlaylist);
    projectm_playlist_set_preset_switched_event_callback(_projectMPlaylist, nullptr, nullptr);
    projectm_playlist_destroy(_projectMPlaylist);
    _projectMPlaylist = nullptr;
}

void PresetPlaylist::Connect(projectm* projectMHandle)
{
    projectm_playlist_connect(_projectMPlaylist, projectMHandle);
}

void PresetPlaylist::Disconnect()
{
    projectm_playlist_connect(_projectMPlaylist, nullptr);
}

void PresetPlaylist::CurrentIndex(ItemList::size_type index, bool hardcut)
{
    if (_items.empty())
    {
        throw PlaylistEmptyException();
    }

    if (index >= _items.size())
    {
        throw std::out_of_range("Playlist index out of range");
    }

    _currentIndex = index;

    poco_assert(_projectMPlaylist);
    projectm_playlist_set_position(_projectMPlaylist, _currentIndex, hardcut);
}

void PresetPlaylist::ShuffleEnabled(bool enabled) const
{
    poco_assert(_projectMPlaylist);
    projectm_playlist_set_shuffle(_projectMPlaylist, enabled);
}

bool PresetPlaylist::ShuffleEnabled() const
{
    poco_assert(_projectMPlaylist);
    return projectm_playlist_get_shuffle(_projectMPlaylist);
}

void PresetPlaylist::Next(bool hardCut)
{
    poco_assert(_projectMPlaylist);
    projectm_playlist_set_shuffle(_projectMPlaylist, false);
    projectm_playlist_play_next(_projectMPlaylist, hardCut);
    projectm_playlist_set_shuffle(_projectMPlaylist, _shuffle);
}

void PresetPlaylist::Previous(bool hardCut)
{
    poco_assert(_projectMPlaylist);
    projectm_playlist_set_shuffle(_projectMPlaylist, false);
    projectm_playlist_play_previous(_projectMPlaylist, hardCut);
    projectm_playlist_set_shuffle(_projectMPlaylist, _shuffle);
}

void PresetPlaylist::LastInHistory(bool hardCut)
{
    poco_assert(_projectMPlaylist);
    projectm_playlist_play_last(_projectMPlaylist, hardCut);
}

void PresetPlaylist::Random(bool hardCut)
{
    poco_assert(_projectMPlaylist);
    projectm_playlist_set_shuffle(_projectMPlaylist, true);
    projectm_playlist_play_next(_projectMPlaylist, hardCut);
    projectm_playlist_set_shuffle(_projectMPlaylist, _shuffle);
}

void PresetPlaylist::BeginBatchEdit()
{
    _isBatchEditing = true;
}

void PresetPlaylist::EndBatchEdit()
{
    if (_isBatchEditing)
    {
        _isBatchEditing = false;
        SendPlaylistChangedNotification();
    }
}

bool PresetPlaylist::AddItem(const Item& item, bool allowDuplicates)
{
    poco_assert(_projectMPlaylist);

    if (projectm_playlist_add_preset(_projectMPlaylist, item.Path().c_str(), allowDuplicates))
    {
        _items.push_back(item);

        if (!_isBatchEditing)
        {
            SendPlaylistChangedNotification();
        }

        return true;
    }

    return false;
}

PresetPlaylist::ItemList::size_type PresetPlaylist::AddPath(const std::string& path,
                                                            const bool recurseSubdirs,
                                                            const bool allowDuplicates)
{
    poco_assert(_projectMPlaylist);

    const uint32_t oldPlaylistSize = projectm_playlist_size(_projectMPlaylist);
    const uint32_t addedPresets = projectm_playlist_add_path(_projectMPlaylist, path.c_str(), recurseSubdirs, allowDuplicates);

    if (addedPresets > 0)
    {
        // For now, we gather the new items from the playlist manager and add them here.
        const auto newItems = projectm_playlist_items(_projectMPlaylist, oldPlaylistSize, addedPresets);
        auto curItem = newItems;
        while (*curItem)
        {
            if (**curItem != '\0')
            {
                _items.emplace_back(std::string(*curItem));
            }
            ++curItem;
        }
        projectm_playlist_free_string_array(newItems);

        if (!_isBatchEditing)
        {
            SendPlaylistChangedNotification();
        }
    }

    return addedPresets;
}

bool PresetPlaylist::InsertItem(const Item& item, ItemList::size_type index, bool allowDuplicates)
{
    poco_assert(_projectMPlaylist);

    if (projectm_playlist_insert_preset(_projectMPlaylist, item.Path().c_str(), index, allowDuplicates))
    {
        _items.insert(_items.begin() + static_cast<ItemList::difference_type>(index), item);

        if (!_isBatchEditing)
        {
            SendPlaylistChangedNotification();
        }

        return true;
    }

    return false;
}

PresetPlaylist::ItemList::size_type PresetPlaylist::InsertPath(const std::string& path,
                                                               const ItemList::size_type index,
                                                               const bool recurseSubdirs,
                                                               const bool allowDuplicates)
{
    poco_assert(_projectMPlaylist);

    const uint32_t addedPresets = projectm_playlist_insert_path(_projectMPlaylist, path.c_str(), index, recurseSubdirs, allowDuplicates);

    if (addedPresets > 0)
    {
        // For now, we gather the new items from the playlist manager and add them here.
        const auto newItems = projectm_playlist_items(_projectMPlaylist, index, addedPresets);
        auto curItem = newItems;
        while (*curItem)
        {
            if (**curItem != '\0')
            {
                _items.emplace_back(std::string(*curItem));
            }
            ++curItem;
        }
        projectm_playlist_free_string_array(newItems);

        if (!_isBatchEditing)
        {
            SendPlaylistChangedNotification();
        }
    }

    return addedPresets;
}

bool PresetPlaylist::RemoveItem(const ItemList::size_type index)
{
    if (index >= _items.size())
    {
        return false;
    }

    poco_assert(_projectMPlaylist);

    if (!projectm_playlist_remove_preset(_projectMPlaylist, index))
    {
        return false;
    }

    _items.erase(_items.begin() + static_cast<ItemList::difference_type>(index));

    if (!_isBatchEditing)
    {
        SendPlaylistChangedNotification();
    }

    return true;
}

bool PresetPlaylist::RemoveItems(const ItemList::size_type index, ItemList::size_type count)
{
    if (index >= _items.size())
    {
        return false;
    }

    if (index + count >= _items.size())
    {
        count = _items.size() - index;
    }

    poco_assert(_projectMPlaylist);

    if (!projectm_playlist_remove_presets(_projectMPlaylist, index, count))
    {
        return false;
    }

    _items.erase(_items.begin() + static_cast<ItemList::difference_type>(index),
                 _items.begin() + static_cast<ItemList::difference_type>(index) + static_cast<ItemList::difference_type>(count));

    if (!_isBatchEditing)
    {
        SendPlaylistChangedNotification();
    }

    return true;
}

void PresetPlaylist::Clear()
{
    poco_assert(_projectMPlaylist);

    if (_items.empty())
    {
        return;
    }

    _items.clear();
    projectm_playlist_clear(_projectMPlaylist);

    if (!_isBatchEditing)
    {
        SendPlaylistChangedNotification();
    }
}

void PresetPlaylist::Sort(SortPredicate predicate,
                          SortOrder order)
{
    SortRange(0, _items.size(), predicate, order);
}

void PresetPlaylist::SortRange(ItemList::size_type startIndex,
                               ItemList::size_type count,
                               SortPredicate predicate,
                               SortOrder order)
{
    poco_assert(_projectMPlaylist);

    if (_items.empty())
    {
        return;
    }

    projectm_playlist_sort_predicate prjmPredicate;
    switch (predicate)
    {
        case SortPredicate::FullPath:
        default:
            prjmPredicate = SORT_PREDICATE_FULL_PATH;

            break;
        case SortPredicate::PresetName:
            prjmPredicate = SORT_PREDICATE_FILENAME_ONLY;
            break;
    }

    projectm_playlist_sort_order prjmOrder;
    switch (order)
    {
        case SortOrder::Ascending:
        default:
            prjmOrder = SORT_ORDER_ASCENDING;
            break;
        case SortOrder::Descending:
            prjmOrder = SORT_ORDER_DESCENDING;
            break;
        case SortOrder::Random:
            // ToDo: Implement.
            throw std::runtime_error("Sort order Random is not supported yet.");
            break;
    }

    projectm_playlist_sort(_projectMPlaylist, startIndex, count, prjmPredicate, prjmOrder);

    // We just resync all items from the projectM playlist for now.
    auto playlistSize = projectm_playlist_size(_projectMPlaylist);
    _items.clear();
    _items.reserve(playlistSize);
    const auto newItems = projectm_playlist_items(_projectMPlaylist, 0, playlistSize);
    auto curItem = newItems;
    while (*curItem)
    {
        if (**curItem != '\0')
        {
            _items.emplace_back(std::string(*curItem));
        }
        ++curItem;
    }
    projectm_playlist_free_string_array(newItems);

    if (!_isBatchEditing)
    {
        SendPlaylistChangedNotification();
    }
}

void PresetPlaylist::PresetSwitchedEvent(bool /* isHardCut */, unsigned int index, void* context)
{
    auto that = static_cast<PresetPlaylist*>(context);

    if (!that->Empty())
    {
        that->_currentIndex = index;
        poco_information_f1(that->_logger, "Displaying preset: %s", that->CurrentItem().PresetName());
    }

    Poco::NotificationCenter::defaultCenter().postNotification(new Notification::UpdateWindowTitle);
}

void PresetPlaylist::SendPlaylistChangedNotification() const
{
    Poco::NotificationCenter::defaultCenter().postNotification(new Notification::PlaylistChanged(*this));
}
