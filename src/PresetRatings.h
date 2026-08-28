#pragma once

#include <Poco/JSON/Object.h>

#include <map>
#include <string>

/**
 * @brief Manages preset ratings stored in a JSON sidecar file.
 *
 * Ratings are 1-5 stars. 0 means unrated.
 * File: ~/.config/projectM/ratings.json
 */
class PresetRatings
{
public:
    /**
     * @brief Loads ratings from the ratings file.
     */
    void Load();

    /**
     * @brief Saves ratings to the ratings file.
     */
    void Save() const;

    /**
     * @brief Returns the rating for a preset (0-5, 0 = unrated).
     */
    int GetRating(const std::string& presetPath) const;

    /**
     * @brief Sets the rating for a preset (0 removes the rating).
     */
    void SetRating(const std::string& presetPath, int rating);

    /**
     * @brief Returns all ratings (path -> 1-5).
     */
    const std::map<std::string, int>& Ratings() const { return _ratings; }

private:
    std::string RatingsFilePath() const;

    std::map<std::string, int> _ratings; //!< Preset path -> rating (1-5)
};
