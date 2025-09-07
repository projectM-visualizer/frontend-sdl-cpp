#pragma once

#include <map>
#include <string>

namespace Editor {

/**
 * @brief Milkdrop preset file parser/writer
 *
 * Extended version of the parser class in libprojectM, with added support for exporting presets.
 *
 * Reads in the file as key/value pairs, where the key is either separated from the value by an equal sign or a space.
 * Lines not matching this pattern are simply ignored, e.g. the [preset00] INI section.
 *
 * Values and code blocks can easily be accessed via the helper functions. It is also possible to access the parsed
 * map contents directly if required.
 *
 * When exporting, missing keys are added using Milkdrop's default values. Exporting an empty/unparsed preset
 * will return a default file, which is identical to passing an empty preset to Milkdrop/projectM.
 */
class PresetFile
{
public:
    using ValueMap = std::map<std::string, std::string>; //!< A map with key/value pairs, each representing one line in the preset file.

    static constexpr size_t maxFileSize = 0x100000; //!< Maximum size of a preset file. Used for sanity checks.

    /**
     * @brief Returns a PresetFile containing a parsed, empty preset with proper default values already set.
     * @return An empty preset with Milkdrop's default value.
     */
    static PresetFile EmptyPreset();

    /**
     * @brief Reads the preset file into an internal map to prepare for parsing.
     * @param presetFile The file name to read from.
     * @return True if the file was parsed successfully, false if an error occurred or no line could be parsed.
     */
    [[nodiscard]] auto ReadFile(const std::string& presetFile) -> bool;

    /**
     * @brief Reads the preset data into an internal map to prepare for parsing.
     * @param presetData The data to read from.
     * @return True if the data was parsed successfully, false if an error occurred or no line could be parsed.
     */
    [[nodiscard]] auto ReadData(const std::string& presetData) -> bool;

    /**
     * @brief Reads the data stream into an internal map to prepare for parsing.
     * @param presetStream The stream to read preset data form.
     * @return True if the stream was parsed successfully, false if an error occurred or no line could be parsed.
     */
    [[nodiscard]] auto ReadStream(std::istream& presetStream) -> bool;

    /**
     * @brief Saves the preset file in Milkdrop's write order.
     * @param presetFile The preset file name to write into.
     */
    [[nodiscard]] auto Write(const std::string& presetFile) const -> bool;

    /**
     * @brief Saves the preset file in Milkdrop's write order.
     * @param presetStream The preset data stream to write into.
     */
    [[nodiscard]] auto Write(std::ostream& presetStream) const -> bool;

    /**
     * @brief Returns the preset file as a string in Milkdrop's write order.
     * @return A string with the same data which would be saved to a preset file.
     */
    [[nodiscard]] auto AsString() const -> std::string;

    /**
     * @brief Returns a block of code, ready for parsing or use in shader compilation.
     *
     * Shaders have a "`" prepended on each line. If a line starts with this character, it's stripped and a newline
     * character is added at the end of each line. Equations are returned as a single, long line.
     *
     * The function appends numbers to the prefix, starting with 1, and stops when a key is missing. This is following
     * Milkdrop's behaviour, so any gap in numbers will essentially cut off all code after the gap.
     *
     * Comments starting with // or \\\\ will be stripped until the end of each line in both equations and shader code.
     *
     * @param keyPrefix The key prefix for the code block to be returned.
     * @return The code that was parsed from the given prefix. Empty if no code was found.
     */
    [[nodiscard]] auto GetCode(const std::string& keyPrefix) const -> std::string;

    /**
     * @brief Replaces/sets a block of code with the given prefix to the new string.
     *
     * Calling this method will remove all existing map keys starting with the prefix and generate
     * new entries, numbered from 1 to the number of lines in "code".
     *
     * If keyPrefix is either "comp_" or "warp_", each line will automatically be prefixed with the
     * "`" character.
     *
     * @param keyPrefix The key prefix for the code block to be set.
     * @param code The new code block to save.
     */
    void SetCode(const std::string& keyPrefix, const std::string& code);

    /**
     * @brief Returns the given key value as an integer.
     *
     * Returns the default value if no value can be parsed or the key doesn't exist.
     *
     * Any additional text after the number, e.g. a comment, is ignored.
     *
     * @param key The key to retrieve the value from.
     * @param defaultValue The default value to return if key is not found.
     * @return The converted value or the default value.
     */
    [[nodiscard]] auto GetInt(const std::string& key, int defaultValue) const -> int;

    /**
     * @brief Sets the given key to a new integer value.
     * @param key The key to set the value for.
     * @param value The new value to set.
     */
    void SetInt(const std::string& key, int value);

    /**
     * @brief Returns the given key value as a floating-point value.
     *
     * Returns the default value if no value can be parsed or the key doesn't exist.
     *
     * Any additional text after the number, e.g. a comment, is ignored.
     *
     * @param key The key to retrieve the value from.
     * @param defaultValue The default value to return if key is not found.
     * @return The converted value or the default value.
     */
    [[nodiscard]] auto GetFloat(const std::string& key, float defaultValue) const -> float;

    /**
     * @brief Sets the given key to a new float value.
     * @param key The key to set the value for.
     * @param value The new value to set.
     */
    void SetFloat(const std::string& key, float value);

    /**
     * @brief Returns the given key value as a boolean.
     *
     * Returns the default value if no value can be parsed or the key doesn't exist.
     *
     * Any additional text after the number, e.g. a comment, is ignored.
     *
     * @param key The key to retrieve the value from.
     * @param defaultValue The default value to return if key is not found.
     * @return True if the value is non-zero, false otherwise.
     */
    [[nodiscard]] auto GetBool(const std::string& key, bool defaultValue) const -> bool;

    /**
     * @brief Sets the given key to a new boolean (0/1) value.
     * @param key The key to set the value for.
     * @param value The new value to set.
     */
    void SetBool(const std::string& key, bool value);

    /**
     * @brief Returns the given key value as a string.
     *
     * Returns the default value if no value can be parsed or the key doesn't exist.
     *
     * @param key The key to retrieve the value from.
     * @param defaultValue The default value to return if key is not found.
     * @return the string content of the key, or the default value.
     */
    [[nodiscard]] auto GetString(const std::string& key, const std::string& defaultValue) const -> std::string;

    /**
     * @brief Sets the given key to a new string value.
     * @param key The key to set the value for.
     * @param value The new value to set.
     */
    void SetString(const std::string& key, const std::string& value);

    /**
     * @brief Returns a reference to the internal value map.
     * @return A reference to the internal value map.
     */
    [[nodiscard]] auto PresetValues() const -> const ValueMap&;

protected:
    /**
     * @brief Parses a single line and stores the result in the value map.
     *
     * The function doesn't really care about invalid lines with random text or comments. The first "word"
     * is added as key to the map, but will not be used afterward.
     *
     * @param line The line to parse.
     */
    void ParseLine(const std::string& line);

private:
    /**
     * @brief Converts the string to lower-case.
     * Only letters A-Z are converted to a-z by default.
     * @param str The original string.
     * @return The lower-case string.
     */
    static auto ToLower(std::string str) -> std::string;

    /**
     * @brief Exports the currently stored preset data as a Milkdrop-compatible INI file.
     *
     * All contents are written in the exact same order and formatting as Milkdrop does.
     * This means parsing and exporting an existing preset file will return identical contents.
     *
     * @return A stringstream containing the preset file data.
     */
    [[nodiscard]] auto ExportPreset() const -> std::stringstream;

    /**
     * @brief Exports a block for a single custom waveform.
     * @param index The index of the custom waveform (0-3).
     * @param outputStream The stream to write the exported data into.
     */
    void ExportWave(int index, std::stringstream& outputStream) const;

    /**
     * @brief Exports a block for a single custom shape.
     * @param index The index of the custom shape (0-3).
     * @param outputStream The stream to write the exported data into.
     */
    void ExportShape(int index, std::stringstream& outputStream) const;

    /**
     * @brief Exports a block of code, with one 1-based numbered key per line.
     *
     * This method will not add the "`" character for shader code. Each code line
     * is written as-is.
     *
     * @param keyPrefix The code block prefix, without line numbers.
     * @param outputStream The stream to write the exported data into.
     */
    void ExportCodeBlock(const std::string& keyPrefix, std::stringstream& outputStream) const;

    ValueMap _presetValues; //!< Map with preset keys and their value.
};

} // namespace Editor
