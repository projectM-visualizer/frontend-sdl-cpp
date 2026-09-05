#pragma once

#include "FileChooser.h"

#ifdef USE_SDL3
# include <SDL3/SDL.h>
#else
# include <SDL2/SDL.h>
#endif

#include <Poco/Logger.h>

class PresetSelection
{
public:

    void Show();

    void Draw();

protected:
    FileChooser _fileChooser; //!< File chooser dialog.

    bool _visible{ true }; //!< Flag for settings window visibility.
    float _displayDuration{ 0.0f }; //!< Preset display time
    uint32_t _playlistPosition{ 0 }; //!< Playlist position
    uint32_t _playlistSize{ 0 }; //!< Playlist size

    Poco::Logger& _logger{ Poco::Logger::get("PresetSelection") }; //!< The class logger.

};
