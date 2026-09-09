#pragma once

#include <Poco/Exception.h>
#include <Poco/Logger.h>

#include <string>
#include <vector>

POCO_DECLARE_EXCEPTION(, PlaylistEmptyException, Poco::Exception);

struct projectm;
struct projectm_playlist;

/**
 * @brief Class representing a single preset playlist in the application.
 *
 * Keeps preset filenames (full path and parsed base name), last playlist position
 * and other useful data.
 *
 * @note For now, this class just mirrors the contents of a projectM-managed playlist.
 *       Future versions will use this class to fully control preset playback as well,
 *       replacing the projectM playlist library.
 */
class PresetPlaylist
{
public:
    /**
     * Playlist sorting predicate (field)
     */
    enum class SortPredicate : uint8_t
    {
        FullPath, //!< Sorts by full preset path.
        PresetName //!< Sorts using the preset name only (e.g. file name).
    };

    /**
     * Playlist sort order.
     */
    enum class SortOrder : uint8_t
    {
        Ascending, //!< Lexical ascending order.
        Descending, //!< Lexical descending order.
        Random //!< Random order.
    };

    /**
     * @brief A single playlist item.
     * Stored information about a single playlist item, such as the full path
     * and the human-readable preset name.
     */
    class Item
    {
    public:
        Item() = delete;

        /**
         * Creates a new playlist item.
         * @param path The full path to the preset file.
         */
        explicit Item(std::string path);

        /**
         * Returns the full filesystem path of the playlist item.
         * @return The full path to the preset file.
         */
        [[nodiscard]] const std::string& Path() const
        {
            return _path;
        }

        /**
         * Sets a custom display name for the playlist item.
         * @param name The new item name to be displayed.
         */
        void PresetName(const std::string& name)
        {
            _presetName = name;
        }

        /**
         * Returns a human-readable name of the playlist item.
         * @return The human-readable name of the preset.
         */
        [[nodiscard]] const std::string& PresetName() const
        {
            return _presetName;
        }

    private:
        std::string _path; //!< The full filesystem path to the preset.
        std::string _presetName; //!< The display name for the preset.
    };

    using ItemList = std::vector<Item>; //!< A list of preset items

    /**
     * Creates an empty playlist.
     */
    PresetPlaylist();

    /**
     * Destructor.
     */
    virtual ~PresetPlaylist();

    /**
     * Connects the playlist with a projectM instance.
     * @param projectMHandle The projectM instance handle to connect the playlist to.
     */
    void Connect(projectm* projectMHandle);

    /**
     * Disconnects the playlist from the projectM instance.
     */
    void Disconnect();

    /**
     * Returns a reference to the stored list of presets.
     * @return A reference to the preset list.
     */
    [[nodiscard]] const ItemList& Items() const
    {
        return _items;
    }

    /**
     * Returns whether the current playlist is empty or not.
     * @return true if the playlist is empty, false if it contains at least one element.
     */
    [[nodiscard]] bool Empty() const
    {
        return _items.empty();
    }

    /**
     * Returns the number of presets in thist playlist instance.
     * @return The number of elements in the current playlist.
     */
    [[nodiscard]] ItemList::size_type Size() const
    {
        return _items.size();
    }

    /**
     * Returns a reference to the currently active playlist item.
     * @throws PlaylistEmptyException Thrown if the playlist is empty and there can't be a current item.
     * @throws std::out_of_range Thrown if the index is outside the valid range.
     * @return A reference to the current playlist item.
     */
    [[nodiscard]] const Item& CurrentItem() const
    {
        if (!_items.empty())
        {
            return _items.at(_currentIndex);
        }

        throw PlaylistEmptyException();
    }

    /**
     * Sets a new playlist index as the currently active preset.
     * @throws PlaylistEmptyException Thrown if the playlist is empty and there can't be a current item.
     * @param index The new playlist index to set.
     */
    void CurrentIndex(ItemList::size_type index, bool hardcut);

    /**
     * Returns the index of the currently active playlist item.
     * @throws PlaylistEmptyException Thrown if the playlist is empty and there can't be a current item.
     * @return The current playlist index.
     */
    [[nodiscard]] ItemList::size_type CurrentIndex() const
    {
        if (_items.empty())
        {
            throw PlaylistEmptyException();
        }

        return _currentIndex;
    }

    /**
     * Sets the shuffle mode.
     * @param enabled The new value of the shuffle flag
     */
    void ShuffleEnabled(bool enabled) const;

    /**
     * Returns whether shuffle mode is enabled or not.
     * @return true if shuffle mode is enabled, false if not.
     */
    [[nodiscard]] bool ShuffleEnabled() const;

    /**
     * Plays the next item in the playlist, regardless of shuffle.
     * @param hardCut If true, do not use a smooth transition.
     */
    void Next(bool hardCut);

    /**
     * Plays the previous item in the playlist, regardless of shuffle.
     * @param hardCut If true, do not use a smooth transition.
     */
    void Previous(bool hardCut);

    /**
     * Plays the last item in the playlist's playback history.
     * @param hardCut If true, do not use a smooth transition.
     */
    void LastInHistory(bool hardCut);

    /**
     * Plays a random item in the playlist, regardless of shuffle.
     * @param hardCut If true, do not use a smooth transition.
     */
    void Random(bool hardCut);

    /**
     * @brief Enters batch edit mode, prevents sending change notifications.
     * When using more than a single insert/remove call, always start batch mode and end
     * it with EndBatchEdit() when done.
     */
    void BeginBatchEdit();

    /**
     * @brief Exits batch edit mode and sends a change notification.
     * @note The change notification is always sent, even if no changes were made when batch mode was active.
     */
    void EndBatchEdit();

    /**
     * Adds a new item to the playlist.
     * @param item The playlist item to add.
     * @param allowDuplicates
     * @return true if the item was added, false if it was a duplicate.
     */
    bool AddItem(const Item& item, bool allowDuplicates);

    /**
     * Adds preset files from a given path to the end of the playlist.
     * @param path The path to scan for presets.
     * @param recurseSubdirs true to recursively search subdirectories, fals to only use the current directory.
     * @param allowDuplicates true to allow duplicate files (same physical path).
     * @return The number of added presets.
     */
    ItemList::size_type AddPath(const std::string& path, bool recurseSubdirs, bool allowDuplicates);

    /**
     * Adds a new item to the playlist at the given index.
     * @param item The playlist item to add.
     * @param index The index to insert the item at.
     * @param allowDuplicates true to allow duplicate files (same physical path).
     * @return true if the item was added, false if it was a duplicate.
     */
    bool InsertItem(const Item& item, ItemList::size_type index, bool allowDuplicates);

    /**
     * Adds preset files from a given path at the given index position in the playlist.
     * @param path The path to scan for presets.
     * @param index The index to insert the new items at.
     * @param recurseSubdirs true to recursively search subdirectories, fals to only use the current directory.
     * @param allowDuplicates true to allow duplicate files (same physical path).
     * @return The number of added presets.
     */
    ItemList::size_type InsertPath(const std::string& path, ItemList::size_type index, bool recurseSubdirs, bool allowDuplicates);

    /**
     * Removes a single item from the playlist.
     * @param index The item index to remove.
     */
    bool RemoveItem(ItemList::size_type index);

    /**
     * Removes @a count number of items beginning at @a index.
     * @param index The start index to remove items.
     * @param count The number of items to remove.
     */
    bool RemoveItems(ItemList::size_type index, ItemList::size_type count);

    /**
     * Removes all items from the playlist.
     */
    void Clear();

    /**
     * Sorts the whole playlist.
     * @param predicate The predicate/field to sort by.
     * @param order The sort order.
     */
    void Sort(SortPredicate predicate, SortOrder order);

    /**
     * Sorts a range of items in the current playlist.
     * @param startIndex The playlist index to begin sorting at.
     * @param count The nubmer of entries to sort beginning at @a index
     * @param predicate The predicate/field to sort by.
     * @param order The sort order.
     */
    void SortRange(ItemList::size_type startIndex, ItemList::size_type count, SortPredicate predicate, SortOrder order);

private:
    /**
     * @brief projectM callback. Called whenever a preset is switched in the current playlist.
     * @param isHardCut True if the switch was a hard cut.
     * @param index New preset playlist index.
     * @param context Callback context, e.g. "this" pointer.
     */
    static void PresetSwitchedEvent(bool isHardCut, unsigned int index, void* context);

    /**
     * Sends a notification that the playlist has changed.
     */
    void SendPlaylistChangedNotification() const;

    projectm_playlist* _projectMPlaylist{nullptr}; //!< The projectM-managed playlist (temporary).
    ItemList::size_type _currentIndex{}; //!< The currently active playlist index.
    ItemList _items; //!< All presets in the current playlist

    bool _shuffle{false}; //!< Shuffle enabled
    bool _isBatchEditing{false}; //!< Flag determining if the playlist is being batch-edited.

    Poco::Logger& _logger{Poco::Logger::get("PresetPlaylist")}; //!< The class logger.
};
