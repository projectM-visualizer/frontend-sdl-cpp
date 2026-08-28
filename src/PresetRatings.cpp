#include "PresetRatings.h"

#include <Poco/File.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Path.h>

#include <fstream>

std::string PresetRatings::RatingsFilePath() const
{
    Poco::Path path = Poco::Path::configHome();
    path.makeDirectory().append("projectM/ratings.json");
    return path.toString();
}

void PresetRatings::Load()
{
    _ratings.clear();

    try
    {
        std::string filePath = RatingsFilePath();
        Poco::File file(filePath);
        if (!file.exists())
        {
            return;
        }

        std::ifstream in(filePath);
        if (!in.is_open())
        {
            return;
        }

        std::string jsonStr((std::istreambuf_iterator<char>(in)),
                             std::istreambuf_iterator<char>());
        in.close();

        if (jsonStr.empty())
        {
            return;
        }

        Poco::JSON::Parser parser;
        auto result = parser.parse(jsonStr);
        auto obj = result.extract<Poco::JSON::Object::Ptr>();

        for (const auto& [key, value] : *obj)
        {
            int rating = 0;
            if (!value.isEmpty())
            {
                rating = value.convert<int>();
            }
            if (rating >= 1 && rating <= 5)
            {
                _ratings[key] = rating;
            }
        }
    }
    catch (...)
    {
        // Ratings file may be corrupted — start fresh
    }
}

void PresetRatings::Save() const
{
    try
    {
        std::string filePath = RatingsFilePath();

        Poco::File dir(Poco::Path(filePath).makeParent().toString());
        if (!dir.exists())
        {
            dir.createDirectories();
        }

        Poco::JSON::Object obj;
        for (const auto& [path, rating] : _ratings)
        {
            obj.set(path, rating);
        }

        std::ofstream out(filePath, std::ios::out | std::ios::trunc);
        if (out.is_open())
        {
            Poco::JSON::Stringifier::stringify(obj, out);
        }
    }
    catch (...)
    {
        // Best effort
    }
}

int PresetRatings::GetRating(const std::string& presetPath) const
{
    auto it = _ratings.find(presetPath);
    if (it != _ratings.end())
    {
        return it->second;
    }
    return 0;
}

void PresetRatings::SetRating(const std::string& presetPath, int rating)
{
    if (rating < 1 || rating > 5)
    {
        _ratings.erase(presetPath);
    }
    else
    {
        _ratings[presetPath] = rating;
    }
    Save();
}
