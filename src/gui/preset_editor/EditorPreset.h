#pragma once

#include "PresetFile.h"

#include <array>
#include <string>

namespace Editor {

/**
 * @class EditorPreset
 * @brief Holds all parsed preset values and code and syncs them with the file.
 *
 * Since ImGUI needs to retrieve data on every rendered frame, using the PresetFile getter/setter methods
 * is too slow. For this reason, we keep the values easily accessible in this class as public members.
 *
 * The class provides methods to sync the data in both directions with a PresetFile.
 */
class EditorPreset
{
public:
    /**
     * Used like an array of floats in ImGui::ColorEditor4
     */
    struct ColorRGBA {
        float red{};
        float green{};
        float blue{};
        float alpha{};
    };

    class Wave
    {
    public:
        int index{0}; //!< Custom waveform index in the preset.
        bool enabled{false}; //!< Render waveform if true.
        int samples{512}; //!< Number of samples/vertices in the waveform.
        int sep{0}; //!< Separation distance of dual waveforms.

        float scaling{1.0f}; //!< Scale factor of waveform.
        float smoothing{0.5f}; //!< Smooth factor of waveform.
        float x{0.5f};
        float y{0.5f};

        ColorRGBA color{1.0f, 1.0f, 1.0f, 1.0f}; //!< Wave color

        bool spectrum{false}; //!< Spectrum data or PCM data.
        bool useDots{false}; //!< If non-zero, draw wave as dots instead of lines.
        bool drawThick{false}; //!< Draw thicker lines.
        bool additive{false}; //!< Add color values together.

        std::string initCode; //!< Custom wave init code, run once on load.
        std::string perFrameCode; //!< Custom wave per-frame code, run once after the per-frame code.
        std::string perPointCode; //!< Custom wave per-point code, run once per waveform vertex.
    };

    class Shape
    {
    public:
        int index{0}; //!< The custom shape index in the preset.
        bool enabled{false}; //!< If false, the shape isn't drawn.
        int sides{4}; //!< Number of sides (vertices)
        bool additive{false}; //!< Flag that specifies whether the shape should be drawn additive.
        bool thickOutline{false}; //!< If true, the shape is rendered with a thick line, otherwise a single-pixel line.
        bool textured{false}; //!< If true, the shape will be rendered with the given texture.
        int instances{1}; //!< Number of shape instances to render

        float x{0.5f}; //!< The shape x position.
        float y{0.5f}; //!< The shape y position.
        float radius{0.1f}; //!< The shape radius (1.0 fills the whole screen).
        float angle{0.0f}; //!< The shape rotation.

        ColorRGBA color{1.0f, 0.0f, 0.0f, 1.0f}; //!< First color
        ColorRGBA color2{0.0f, 1.0f, 0.0f, 0.0f}; //!< Second color
        ColorRGBA borderColor{0.0f, 1.0f, 0.0f, 0.0f}; //!< Border color

        float tex_ang{0.0f}; //!< Texture rotation angle.
        float tex_zoom{1.0f}; //!< Texture zoom value.

        std::string initCode; //!< Custom shape init code, run once on load.
        std::string perFrameCode; //!< Custom shape per-frame code, run once per shape instance.
    };

    void FromParsedFile(const PresetFile& parsedFile);

    void ToParsedFile(PresetFile& parsedFile);

    float gammaAdj{2.0f};
    float videoEchoZoom{2.0f};
    float videoEchoAlpha{0.0f};
    float videoEchoAlphaOld{0.0f};
    int videoEchoOrientation{0};
    int videoEchoOrientationOld{0};

    float decay{0.98f};

    int waveMode{0};
    int oldWaveMode{-1};
    bool additiveWaves{false};
    float waveScale{1.0f};
    float waveSmoothing{0.75f};
    bool waveDots{false};
    bool waveThick{false};
    float waveParam{0.0f};
    bool modWaveAlphaByVolume{false};
    float modWaveAlphaStart{0.75f};
    float modWaveAlphaEnd{0.95f};
    float warpAnimSpeed{1.0f};
    float warpScale{1.0f};
    float zoomExponent{1.0f};
    float shader{0.0f};
    bool maximizeWaveColor{true};
    bool texWrap{true};
    bool darkenCenter{false};
    bool redBlueStereo{false};
    bool brighten{false};
    bool darken{false};
    bool solarize{false};
    bool invert{false};

    float zoom{1.0f};
    float rot{0.0f};
    float rotCX{0.5f};
    float rotCY{0.5f};
    float xPush{0.0f};
    float yPush{0.0f};
    float warpAmount{1.0f};
    float stretchX{1.0f};
    float stretchY{1.0f};
    ColorRGBA waveColor{1.0f, 1.0f, 1.0f, 0.8f};
    float waveX{0.5f};
    float waveY{0.5f};
    float outerBorderSize{0.01f};
    ColorRGBA outerBorderColor{0.0f, 0.0f, 0.0f, 0.0f};
    float innerBorderSize{0.01f};
    ColorRGBA innerBorderColor{0.25f, 0.25f, 0.25f, 0.0f};
    float mvX{12.0f};
    float mvY{9.0f};
    float mvDX{0.0f};
    float mvDY{0.0f};
    float mvL{0.9f};
    ColorRGBA mvColor{1.0f, 1.0f, 1.0f, 1.0f};
    float blur1Min{0.0f};
    float blur2Min{0.0f};
    float blur3Min{0.0f};
    float blur1Max{1.0f};
    float blur2Max{1.0f};
    float blur3Max{1.0f};
    float blur1EdgeDarken{0.25f};

    int presetVersion{100}; //!< Value of MILKDROP_PRESET_VERSION in preset files.
    int warpShaderVersion{2}; //!< PSVERSION or PSVERSION_WARP.
    int compositeShaderVersion{2}; //!< PSVERSION or PSVERSION_COMP.

    std::string perFrameInitCode; //!< Preset init code, run once on load.
    std::string perFrameCode; //!< Preset per-frame code, run once at the start of each frame.
    std::string perPixelCode; //!< Preset per-pixel/per-vertex code, run once per warp mesh vertex.

    std::array<Wave, 4> waves;
    std::array<Shape, 4> shapes;

    std::string warpShader; //!< Warp shader code.
    std::string compositeShader; //!< Composite shader code.
};

} // namespace Editor
