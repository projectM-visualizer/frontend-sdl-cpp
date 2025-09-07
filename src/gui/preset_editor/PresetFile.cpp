#include "PresetFile.h"

#include <algorithm>
#include <cstdint>
#include <forward_list>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>

namespace Editor {

PresetFile PresetFile::EmptyPreset()
{
    PresetFile emptyPreset;

    if (emptyPreset.ReadData(emptyPreset.ExportPreset().str()))
    {
        return emptyPreset;
    }

    return {};
}

auto PresetFile::ReadFile(const std::string& presetFile) -> bool
{
    std::ifstream presetStream(presetFile.c_str(), std::ios_base::in | std::ios_base::binary);
    return ReadStream(presetStream);
}

auto PresetFile::ReadData(const std::string& presetData) -> bool
{
    std::istringstream presetStream(presetData);
    return ReadStream(presetStream);
}

auto PresetFile::ReadStream(std::istream& presetStream) -> bool
{
    if (!presetStream.good())
    {
        return false;
    }

    presetStream.seekg(0, std::istream::end);
    auto fileSize = presetStream.tellg();
    presetStream.seekg(0, std::istream::beg);

    if (static_cast<size_t>(fileSize) > maxFileSize)
    {
        return false;
    }

    std::vector<char> presetFileContents(fileSize);
    presetStream.read(presetFileContents.data(), fileSize);

    if (presetStream.fail() || presetStream.bad())
    {
        return false;
    }

    _presetValues.clear();

    size_t startPos{0}; // Starting position of current line
    size_t pos{0}; // Current read position

    auto parseLineIfDataAvailable = [this, &pos, &startPos, &presetFileContents]() {
        if (pos > startPos)
        {
            auto beg = presetFileContents.begin();
            std::string line(beg + startPos, beg + pos);
            ParseLine(line);
        }
    };

    while (pos < presetFileContents.size())
    {
        switch (presetFileContents[pos])
        {
            case '\r':
            case '\n':
                // EOL, skip over CR/LF
                parseLineIfDataAvailable();
                startPos = pos + 1;
                break;

            case '\0':
                // Null char is not expected. Could be a random binary file.
                return false;
        }

        ++pos;
    }

    parseLineIfDataAvailable();

    return !_presetValues.empty();
}

auto PresetFile::Write(const std::string& presetFile) const -> bool
{
    std::ofstream presetStream(presetFile.c_str(), std::ios_base::out | std::ios_base::binary);
    return Write(presetStream);
}

auto PresetFile::Write(std::ostream& presetStream) const -> bool
{
    if (!presetStream.good())
    {
        return false;
    }

    auto presetData = ExportPreset();
    presetStream << presetData.rdbuf();
    presetStream.flush();

    return true;
}

auto PresetFile::AsString() const -> std::string
{
    return ExportPreset().str();
}

auto PresetFile::GetCode(const std::string& keyPrefix) const -> std::string
{
    auto lowerKey = ToLower(keyPrefix);

    std::stringstream code; //!< The parsed code
    std::string key(lowerKey.length() + 5, '\0'); // Allocate a string that can hold up to 5 digits.

    key.replace(0, lowerKey.length(), lowerKey);

    for (int index{1}; index <= 99999; ++index)
    {
        key.replace(lowerKey.length(), 5, std::to_string(index));
        if (_presetValues.find(key) == _presetValues.end())
        {
            break;
        }

        auto line = _presetValues.at(key);

        // Remove backtick char in shader code
        if (!line.empty() && line.at(0) == '`')
        {
            line.erase(0, 1);
        }
        code << line << std::endl;
    }

    auto codeStr = code.str();

    return codeStr;
}

void PresetFile::SetCode(const std::string& keyPrefix, const std::string& code)
{
    auto lowerKeyPrefix = ToLower(keyPrefix);

    bool shaderPrefix = false;

    if (lowerKeyPrefix == "warp_" || lowerKeyPrefix == "comp_")
    {
        shaderPrefix = true;
    }

    // Remove all previous lines for this code block prefix
    for (auto it = begin(_presetValues); it != end(_presetValues);)
    {
        if (it->first.substr(0, lowerKeyPrefix.length()) == lowerKeyPrefix)
        {
            // Check if line prefix is only followed by numbers
            std::string remainder = it->first.substr(lowerKeyPrefix.length());
            bool onlyDigits = true;
            for (const auto ch : remainder)
            {
                if (ch < '0' || ch > '9')
                {
                    onlyDigits = false;
                    break;
                }
            }
            if (onlyDigits)
            {
                it = _presetValues.erase(it);
                continue;
            }
        }
        ++it;
    }

    if (code.empty())
    {
        return;
    }

    uint32_t lineNumber = 1;

    std::string key(lowerKeyPrefix.length() + 5, '\0'); // Allocate a string that can hold up to 5 digits.
    key.replace(0, lowerKeyPrefix.length(), lowerKeyPrefix);

    std::stringstream codeStream(code);
    std::string codeLine;
    while (std::getline(codeStream, codeLine, '\n'))
    {
        if (shaderPrefix)
        {
            codeLine = std::string("`").append(codeLine);
        }

        key.replace(lowerKeyPrefix.length(), 5, std::to_string(lineNumber));
        _presetValues[key] = codeLine;

        lineNumber++;

        // Milkdrop doesn't enforce this on export, but stops reading at 99999 lines.
        // So just stop writing more lines than necessary if someone tries.
        if (lineNumber > 99999)
        {
            break;
        }
    }
}

auto PresetFile::GetInt(const std::string& key, int defaultValue) const -> int
{
    auto lowerKey = ToLower(key);
    if (_presetValues.find(lowerKey) != _presetValues.end())
    {
        try
        {
            return std::stoi(_presetValues.at(lowerKey));
        }
        catch (std::logic_error&)
        {
        }
    }

    return defaultValue;
}

void PresetFile::SetInt(const std::string& key, int value)
{
    auto lowerKey = ToLower(key);
    _presetValues[lowerKey] = std::to_string(value);
}

auto PresetFile::GetFloat(const std::string& key, float defaultValue) const -> float
{
    auto lowerKey = ToLower(key);
    if (_presetValues.find(lowerKey) != _presetValues.end())
    {
        try
        {
            return std::stof(_presetValues.at(lowerKey));
        }
        catch (std::logic_error&)
        {
        }
    }

    return defaultValue;
}

void PresetFile::SetFloat(const std::string& key, float value)
{
    auto lowerKey = ToLower(key);
    _presetValues[lowerKey] = std::to_string(value);
}

auto PresetFile::GetBool(const std::string& key, bool defaultValue) const -> bool
{
    return GetInt(key, static_cast<int>(defaultValue)) > 0;
}

void PresetFile::SetBool(const std::string& key, bool value)
{
    auto lowerKey = ToLower(key);
    _presetValues[lowerKey] = value ? "1" : "0";
}

auto PresetFile::GetString(const std::string& key, const std::string& defaultValue) const -> std::string
{
    auto lowerKey = ToLower(key);
    if (_presetValues.find(lowerKey) != _presetValues.end())
    {
        return _presetValues.at(lowerKey);
    }

    return defaultValue;
}

void PresetFile::SetString(const std::string& key, const std::string& value)
{
    auto lowerKey = ToLower(key);
    _presetValues[lowerKey] = value;
}

const std::map<std::string, std::string>& PresetFile::PresetValues() const
{
    return _presetValues;
}

void PresetFile::ParseLine(const std::string& line)
{
    // Search for first delimiter, either space or equal
    auto varNameDelimiterPos = line.find_first_of(" =");

    if (varNameDelimiterPos == std::string::npos || varNameDelimiterPos == 0)
    {
        // Empty line, delimiter at start of line or no delimiter found, skip.
        return;
    }

    // Convert key to lower case, as INI functions are not case-sensitive.
    std::string varName(ToLower(std::string(line.begin(), line.begin() + varNameDelimiterPos)));
    std::string value(line.begin() + varNameDelimiterPos + 1, line.end());

    // Only add first occurrence to mimic Milkdrop behaviour
    if (!varName.empty() && _presetValues.find(varName) == _presetValues.end())
    {
        _presetValues.emplace(std::move(varName), std::move(value));
    }
}

auto PresetFile::ToLower(std::string str) -> std::string
{
    std::transform(str.begin(), str.end(), str.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    return str;
}

auto PresetFile::ExportPreset() const -> std::stringstream
{
    std::stringstream presetFileContents;

    int milkdropPresetVersion = GetInt("MILKDROP_PRESET_VERSION", 100);

    // Introduced in Milkdrop 2.0, but can be set to <200.
    presetFileContents << "MILKDROP_PRESET_VERSION=" << milkdropPresetVersion << std::endl;

    // Milkdrop 2 Pixel Shader version information.
    if (milkdropPresetVersion == 200)
    {
        presetFileContents << "PSVERSION=" << GetInt("psversion", 0) << std::endl;
    }
    else if (milkdropPresetVersion > 200)
    {
        presetFileContents << "PSVERSION_WARP=" << GetInt("psversion_warp", 0) << std::endl;
        presetFileContents << "PSVERSION_COMP=" << GetInt("psversion_comp", 0) << std::endl;
    }

    presetFileContents << "[preset00]" << std::endl;

    // (Unused) rating value and classic post-processing filter parameters
    presetFileContents << std::setprecision(3) << std::fixed;
    presetFileContents << "fRating=" << GetFloat("fRating", 3.0f) << std::endl;
    presetFileContents << "fGammaAdj=" << GetFloat("fGammaAdj", 2.0f) << std::endl;
    presetFileContents << "fDecay=" << GetFloat("fDecay", 0.98f) << std::endl;
    presetFileContents << "fVideoEchoZoom=" << GetFloat("fVideoEchoZoom", 2.0f) << std::endl;
    presetFileContents << "fVideoEchoAlpha=" << GetFloat("fVideoEchoAlpha", 0.0f) << std::endl;
    presetFileContents << "nVideoEchoOrientation=" << GetInt("nVideoEchoOrientation", 0) << std::endl;

    // Flags for classic filters and default waveform parameters
    presetFileContents << "nWaveMode=" << GetInt("nWaveMode", 0) << std::endl;
    presetFileContents << "bAdditiveWaves=" << GetInt("bAdditiveWaves", 0) << std::endl;
    presetFileContents << "bWaveDots=" << GetInt("bWaveDots", 0) << std::endl;
    presetFileContents << "bWaveThick=" << GetInt("bWaveThick", 0) << std::endl;
    presetFileContents << "bModWaveAlphaByVolume=" << GetInt("bModWaveAlphaByVolume", 0) << std::endl;
    presetFileContents << "bMaximizeWaveColor=" << GetInt("bMaximizeWaveColor", 1) << std::endl;
    presetFileContents << "bTexWrap=" << GetInt("bTexWrap", 1) << std::endl;
    presetFileContents << "bDarkenCenter=" << GetInt("bDarkenCenter", 0) << std::endl;
    presetFileContents << "bRedBlueStereo=" << GetInt("bRedBlueStereo", 0) << std::endl;
    presetFileContents << "bBrighten=" << GetInt("bBrighten", 0) << std::endl;
    presetFileContents << "bDarken=" << GetInt("bDarken", 0) << std::endl;
    presetFileContents << "bSolarize=" << GetInt("bSolarize", 0) << std::endl;
    presetFileContents << "bInvert=" << GetInt("bInvert", 0) << std::endl;

    // Default waveform and warp animation
    presetFileContents << "fWaveAlpha=" << GetFloat("fWaveAlpha", 0.8f) << std::endl;
    presetFileContents << "fWaveScale=" << GetFloat("fWaveScale", 1.0f) << std::endl;
    presetFileContents << "fWaveSmoothing=" << GetFloat("fWaveSmoothing", 0.75f) << std::endl;
    presetFileContents << "fWaveParam=" << GetFloat("fWaveParam", 0.0f) << std::endl;
    presetFileContents << "fModWaveAlphaStart=" << GetFloat("fModWaveAlphaStart", 0.75f) << std::endl;
    presetFileContents << "fModWaveAlphaEnd=" << GetFloat("fModWaveAlphaEnd", 0.95f) << std::endl;
    presetFileContents << "fWarpAnimSpeed=" << GetFloat("fWarpAnimSpeed", 1.0f) << std::endl;
    presetFileContents << "fWarpScale=" << GetFloat("fWarpScale", 1.0f) << std::endl;
    presetFileContents << std::setprecision(5);
    presetFileContents << "fZoomExponent=" << GetFloat("fZoomExponent", 1.0f) << std::endl;
    presetFileContents << std::setprecision(3);
    presetFileContents << "fShader=" << GetFloat("fShader", 0.0f) << std::endl;

    // Warp parameters and default waveform color/location
    presetFileContents << std::setprecision(5);
    presetFileContents << "zoom=" << GetFloat("zoom", 1.0f) << std::endl;
    presetFileContents << "rot=" << GetFloat("rot", 0.0f) << std::endl;
    presetFileContents << std::setprecision(3);
    presetFileContents << "cx=" << GetFloat("cx", 0.5f) << std::endl;
    presetFileContents << "cy=" << GetFloat("cy", 0.5f) << std::endl;
    presetFileContents << std::setprecision(5);
    presetFileContents << "dx=" << GetFloat("dx", 0.0f) << std::endl;
    presetFileContents << "dy=" << GetFloat("dy", 0.0f) << std::endl;
    presetFileContents << "warp=" << GetFloat("warp", 1.0f) << std::endl;
    presetFileContents << "sx=" << GetFloat("sx", 1.0f) << std::endl;
    presetFileContents << "sy=" << GetFloat("sy", 1.0f) << std::endl;
    presetFileContents << std::setprecision(3);
    presetFileContents << "wave_r=" << GetFloat("wave_r", 1.0f) << std::endl;
    presetFileContents << "wave_g=" << GetFloat("wave_g", 1.0f) << std::endl;
    presetFileContents << "wave_b=" << GetFloat("wave_b", 1.0f) << std::endl;
    presetFileContents << "wave_x=" << GetFloat("wave_x", 0.5f) << std::endl;
    presetFileContents << "wave_y=" << GetFloat("wave_y", 0.5f) << std::endl;

    // Borders and motion vectors
    presetFileContents << "ob_size=" << GetFloat("ob_size", 0.01f) << std::endl;
    presetFileContents << "ob_r=" << GetFloat("ob_r", 0.0f) << std::endl;
    presetFileContents << "ob_g=" << GetFloat("ob_g", 0.0f) << std::endl;
    presetFileContents << "ob_b=" << GetFloat("ob_b", 0.0f) << std::endl;
    presetFileContents << "ob_a=" << GetFloat("ob_a", 0.0f) << std::endl;
    presetFileContents << "ib_size=" << GetFloat("ib_size", 0.01f) << std::endl;
    presetFileContents << "ib_r=" << GetFloat("ib_r", 0.25f) << std::endl;
    presetFileContents << "ib_g=" << GetFloat("ib_g", 0.25f) << std::endl;
    presetFileContents << "ib_b=" << GetFloat("ib_b", 0.25f) << std::endl;
    presetFileContents << "ib_a=" << GetFloat("ib_a", 0.0f) << std::endl;
    presetFileContents << "nMotionVectorsX=" << GetFloat("nMotionVectorsX", 12.0f) << std::endl;
    presetFileContents << "nMotionVectorsY=" << GetFloat("nMotionVectorsY", 9.0f) << std::endl;
    presetFileContents << "mv_dx=" << GetFloat("mv_dx", 0.0f) << std::endl;
    presetFileContents << "mv_dy=" << GetFloat("mv_dy", 0.0f) << std::endl;
    presetFileContents << "mv_l=" << GetFloat("mv_l", 0.9f) << std::endl;
    presetFileContents << "mv_r=" << GetFloat("mv_r", 1.0f) << std::endl;
    presetFileContents << "mv_g=" << GetFloat("mv_g", 1.0f) << std::endl;
    presetFileContents << "mv_b=" << GetFloat("mv_b", 1.0f) << std::endl;
    presetFileContents << "mv_a=" << GetFloat("mv_a", 1.0f) << std::endl;
    presetFileContents << "b1n=" << GetFloat("b1n", 0.0f) << std::endl;
    presetFileContents << "b2n=" << GetFloat("b2n", 0.0f) << std::endl;
    presetFileContents << "b3n=" << GetFloat("b3n", 0.0f) << std::endl;
    presetFileContents << "b1x=" << GetFloat("b1x", 1.0f) << std::endl;
    presetFileContents << "b2x=" << GetFloat("b2x", 1.0f) << std::endl;
    presetFileContents << "b3x=" << GetFloat("b3x", 1.0f) << std::endl;
    presetFileContents << "b1ed=" << GetFloat("b1ed", 0.25f) << std::endl;

    // Custom waves
    for (int index = 0; index < 4; index++)
    {
        ExportWave(index, presetFileContents);
    }

    // Custom shapes
    for (int index = 0; index < 4; index++)
    {
        ExportShape(index, presetFileContents);
    }

    // Per-frame code
    ExportCodeBlock("per_frame_init_", presetFileContents);
    ExportCodeBlock("per_frame_", presetFileContents);
    ExportCodeBlock("per_pixel_", presetFileContents);

    // Shaders
    if (GetInt("psversion_warp", 0) >= 2)
    {
        ExportCodeBlock("warp_", presetFileContents);
    }
    if (GetInt("psversion_comp", 0) >= 2)
    {
        ExportCodeBlock("comp_", presetFileContents);
    }

    return presetFileContents;
}

void PresetFile::ExportWave(int index, std::stringstream& outputStream) const
{
    auto exportIntLine = [&](const std::string& key, int defaultValue) {
        std::string linePrefix = std::string("wavecode_")
                                     .append(std::to_string(index))
                                     .append("_")
                                     .append(key);

        outputStream << linePrefix << "=" << GetInt(linePrefix, defaultValue) << std::endl;
    };

    auto exportFloatLine = [&](const std::string& key, int precision, float defaultValue) {
        std::string linePrefix = std::string("wavecode_")
                                     .append(std::to_string(index))
                                     .append("_")
                                     .append(key);

        outputStream << std::setprecision(precision) << std::fixed
                     << linePrefix << "=" << GetFloat(linePrefix, defaultValue) << std::endl;
    };

    exportIntLine("enabled", 0);
    exportIntLine("samples", 512);
    exportIntLine("sep", 0);
    exportIntLine("bSpectrum", 0);
    exportIntLine("bUseDots", 0);
    exportIntLine("bDrawThick", 0);
    exportIntLine("bAdditive", 0);
    exportFloatLine("scaling", 5, 1.0f);
    exportFloatLine("smoothing", 5, 0.5f);
    exportFloatLine("r", 3, 1.0f);
    exportFloatLine("g", 3, 1.0f);
    exportFloatLine("b", 3, 1.0f);
    exportFloatLine("a", 3, 1.0f);

    std::string codeBlockPrefix = std::string("wave_")
                                      .append(std::to_string(index))
                                      .append("_");

    ExportCodeBlock(codeBlockPrefix + "init", outputStream);
    ExportCodeBlock(codeBlockPrefix + "per_frame", outputStream);
    ExportCodeBlock(codeBlockPrefix + "per_point", outputStream);
}

void PresetFile::ExportShape(int index, std::stringstream& outputStream) const
{
    auto exportIntLine = [&](const std::string& key, int defaultValue) {
        std::string linePrefix = std::string("shapecode_")
                                     .append(std::to_string(index))
                                     .append("_")
                                     .append(key);

        outputStream << linePrefix << "=" << GetInt(linePrefix, defaultValue) << std::endl;
    };

    auto exportFloatLine = [&](const std::string& key, int precision, float defaultValue) {
        std::string linePrefix = std::string("shapecode_")
                                     .append(std::to_string(index))
                                     .append("_")
                                     .append(key);

        outputStream << std::setprecision(precision) << std::fixed
                     << linePrefix << "=" << GetFloat(linePrefix, defaultValue) << std::endl;
    };

    exportIntLine("enabled", 0);
    exportIntLine("sides", 4);
    exportIntLine("additive", 0);
    exportIntLine("thickOutline", 0);
    exportIntLine("textured", 0);
    exportIntLine("num_inst", 1);
    exportFloatLine("x", 3, 0.5f);
    exportFloatLine("y", 3, 0.5f);
    exportFloatLine("rad", 5, 0.1f);
    exportFloatLine("ang", 5, 0.0f);
    exportFloatLine("tex_ang", 5, 0.0f);
    exportFloatLine("tex_zoom", 5, 1.0f);
    exportFloatLine("r", 3, 1.0f);
    exportFloatLine("g", 3, 0.0f);
    exportFloatLine("b", 3, 0.0f);
    exportFloatLine("a", 3, 1.0f);
    exportFloatLine("r2", 3, 1.0f);
    exportFloatLine("g2", 3, 0.0f);
    exportFloatLine("b2", 3, 0.0f);
    exportFloatLine("a2", 3, 0.0f);
    exportFloatLine("border_r", 3, 1.0f);
    exportFloatLine("border_g", 3, 1.0f);
    exportFloatLine("border_b", 3, 1.0f);
    exportFloatLine("border_a", 3, 0.1f);

    std::string codeBlockPrefix = std::string("shape_")
                                      .append(std::to_string(index))
                                      .append("_");

    ExportCodeBlock(codeBlockPrefix + "init", outputStream);
    ExportCodeBlock(codeBlockPrefix + "per_frame", outputStream);
}

void PresetFile::ExportCodeBlock(const std::string& keyPrefix, std::stringstream& outputStream) const
{
    std::string key(keyPrefix.length() + 5, '\0'); //!< Allocate a string that can hold up to 5 digits.
    key.replace(0, keyPrefix.length(), keyPrefix);

    for (int index{1}; index <= 99999; ++index)
    {
        key.replace(keyPrefix.length(), 5, std::to_string(index));
        if (_presetValues.find(key) == _presetValues.end())
        {
            break;
        }

        auto line = _presetValues.at(key);

        outputStream << key << "=" << line << std::endl;
    }
}

} // namespace Editor

