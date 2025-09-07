#include "EditorPreset.h"

namespace Editor {

void EditorPreset::FromParsedFile(const PresetFile& parsedFile)
{
    // General:
    decay = parsedFile.GetFloat("fDecay", decay);
    gammaAdj = parsedFile.GetFloat("fGammaAdj", gammaAdj);
    videoEchoZoom = parsedFile.GetFloat("fVideoEchoZoom", videoEchoZoom);
    videoEchoAlpha = parsedFile.GetFloat("fVideoEchoAlpha", videoEchoAlpha);
    videoEchoOrientation = parsedFile.GetInt("nVideoEchoOrientation", videoEchoOrientation);
    redBlueStereo = parsedFile.GetBool("bRedBlueStereo", redBlueStereo);
    brighten = parsedFile.GetBool("bBrighten", brighten);
    darken = parsedFile.GetBool("bDarken", darken);
    solarize = parsedFile.GetBool("bSolarize", solarize);
    invert = parsedFile.GetBool("bInvert", invert);
    shader = parsedFile.GetFloat("fShader", shader);
    blur1Min = parsedFile.GetFloat("b1n", blur1Min);
    blur2Min = parsedFile.GetFloat("b2n", blur2Min);
    blur3Min = parsedFile.GetFloat("b3n", blur3Min);
    blur1Max = parsedFile.GetFloat("b1x", blur1Max);
    blur2Max = parsedFile.GetFloat("b2x", blur2Max);
    blur3Max = parsedFile.GetFloat("b3x", blur3Max);
    blur1EdgeDarken = parsedFile.GetFloat("b1ed", blur1EdgeDarken);

    // Wave:
    waveMode = parsedFile.GetInt("nWaveMode", waveMode);
    additiveWaves = parsedFile.GetBool("bAdditiveWaves", additiveWaves);
    waveDots = parsedFile.GetBool("bWaveDots", waveDots);
    waveThick = parsedFile.GetBool("bWaveThick", waveThick);
    modWaveAlphaByVolume = parsedFile.GetBool("bModWaveAlphaByVolume", modWaveAlphaByVolume);
    maximizeWaveColor = parsedFile.GetBool("bMaximizeWaveColor", maximizeWaveColor);
    waveScale = parsedFile.GetFloat("fWaveScale", waveScale);
    waveSmoothing = parsedFile.GetFloat("fWaveSmoothing", waveSmoothing);
    waveParam = parsedFile.GetFloat("fWaveParam", waveParam);
    modWaveAlphaStart = parsedFile.GetFloat("fModWaveAlphaStart", modWaveAlphaStart);
    modWaveAlphaEnd = parsedFile.GetFloat("fModWaveAlphaEnd", modWaveAlphaEnd);
    waveColor.red = parsedFile.GetFloat("wave_r", waveColor.red);
    waveColor.green = parsedFile.GetFloat("wave_g", waveColor.green);
    waveColor.blue = parsedFile.GetFloat("wave_b", waveColor.blue);
    waveColor.alpha = parsedFile.GetFloat("fWaveAlpha", waveColor.alpha);
    waveX = parsedFile.GetFloat("wave_x", waveX);
    waveY = parsedFile.GetFloat("wave_y", waveY);
    mvX = parsedFile.GetFloat("nMotionVectorsX", mvX);
    mvY = parsedFile.GetFloat("nMotionVectorsY", mvY);
    mvDX = parsedFile.GetFloat("mv_dx", mvDX);
    mvDY = parsedFile.GetFloat("mv_dy", mvDY);
    mvL = parsedFile.GetFloat("mv_l", mvL);
    mvColor.red = parsedFile.GetFloat("mv_r", mvColor.red);
    mvColor.green = parsedFile.GetFloat("mv_g", mvColor.green);
    mvColor.blue = parsedFile.GetFloat("mv_b", mvColor.blue);
    mvColor.alpha = parsedFile.GetBool("bMotionVectorsOn", false) ? 1.0f : 0.0f; // for backwards compatibility
    mvColor.alpha = parsedFile.GetFloat("mv_a", mvColor.alpha);

    // Motion:
    zoom = parsedFile.GetFloat("zoom", zoom);
    rot = parsedFile.GetFloat("rot", rot);
    rotCX = parsedFile.GetFloat("cx", rotCX);
    rotCY = parsedFile.GetFloat("cy", rotCY);
    xPush = parsedFile.GetFloat("dx", xPush);
    yPush = parsedFile.GetFloat("dy", yPush);
    warpAmount = parsedFile.GetFloat("warp", warpAmount);
    stretchX = parsedFile.GetFloat("sx", stretchX);
    stretchY = parsedFile.GetFloat("sy", stretchY);
    texWrap = parsedFile.GetBool("bTexWrap", texWrap);
    darkenCenter = parsedFile.GetBool("bDarkenCenter", darkenCenter);
    warpAnimSpeed = parsedFile.GetFloat("fWarpAnimSpeed", warpAnimSpeed);
    warpScale = parsedFile.GetFloat("fWarpScale", warpScale);
    zoomExponent = parsedFile.GetFloat("fZoomExponent", zoomExponent);

    // Borders:
    outerBorderSize = parsedFile.GetFloat("ob_size", outerBorderSize);
    outerBorderColor.red = parsedFile.GetFloat("ob_r", outerBorderColor.red);
    outerBorderColor.green = parsedFile.GetFloat("ob_g", outerBorderColor.green);
    outerBorderColor.blue = parsedFile.GetFloat("ob_b", outerBorderColor.blue);
    outerBorderColor.alpha = parsedFile.GetFloat("ob_a", outerBorderColor.alpha);
    innerBorderSize = parsedFile.GetFloat("ib_size", innerBorderSize);
    innerBorderColor.red = parsedFile.GetFloat("ib_r", innerBorderColor.red);
    innerBorderColor.green = parsedFile.GetFloat("ib_g", innerBorderColor.green);
    innerBorderColor.blue = parsedFile.GetFloat("ib_b", innerBorderColor.blue);
    innerBorderColor.alpha = parsedFile.GetFloat("ib_a", innerBorderColor.alpha);

    // Versions:
    presetVersion = parsedFile.GetInt("MILKDROP_PRESET_VERSION", presetVersion);
    if (presetVersion < 200)
    {
        // Milkdrop 1.x did not use shaders.
        warpShaderVersion = 0;
        compositeShaderVersion = 0;
    }
    else if (presetVersion == 200)
    {
        // Milkdrop 2.0 only supported a single shader language level variable.
        warpShaderVersion = parsedFile.GetInt("PSVERSION", warpShaderVersion);
        compositeShaderVersion = parsedFile.GetInt("PSVERSION", compositeShaderVersion);
    }
    else
    {
        warpShaderVersion = parsedFile.GetInt("PSVERSION_WARP", warpShaderVersion);
        compositeShaderVersion = parsedFile.GetInt("PSVERSION_COMP", compositeShaderVersion);
    }

    // Code:
    perFrameInitCode = parsedFile.GetCode("per_frame_init_");
    perFrameCode = parsedFile.GetCode("per_frame_");
    perPixelCode = parsedFile.GetCode("per_pixel_");

    // Custom waveform code:
    for (int index = 0; index < 4; index++)
    {
        Wave& wave = waves[index];

        std::string const wavecodePrefix = "wavecode_" + std::to_string(index) + "_";

        wave.index = index;
        wave.enabled = parsedFile.GetInt(wavecodePrefix + "enabled", wave.enabled);
        wave.samples = parsedFile.GetInt(wavecodePrefix + "samples", wave.samples);
        wave.sep = parsedFile.GetInt(wavecodePrefix + "sep", wave.sep);
        wave.spectrum = parsedFile.GetBool(wavecodePrefix + "bSpectrum", wave.spectrum);
        wave.useDots = parsedFile.GetBool(wavecodePrefix + "bUseDots", wave.useDots);
        wave.drawThick = parsedFile.GetBool(wavecodePrefix + "bDrawThick", wave.drawThick);
        wave.additive = parsedFile.GetBool(wavecodePrefix + "bAdditive", wave.additive);
        wave.scaling = parsedFile.GetFloat(wavecodePrefix + "scaling", wave.scaling);
        wave.smoothing = parsedFile.GetFloat(wavecodePrefix + "smoothing", wave.smoothing);
        wave.color.red = parsedFile.GetFloat(wavecodePrefix + "r", wave.color.red);
        wave.color.green = parsedFile.GetFloat(wavecodePrefix + "g", wave.color.green);
        wave.color.blue = parsedFile.GetFloat(wavecodePrefix + "b", wave.color.blue);
        wave.color.alpha = parsedFile.GetFloat(wavecodePrefix + "a", wave.color.alpha);

        std::string const wavePrefix = "wave_" + std::to_string(index) + "_";

        wave.initCode = parsedFile.GetCode(wavePrefix + "init");
        wave.perFrameCode = parsedFile.GetCode(wavePrefix + "per_frame");
        wave.perPointCode = parsedFile.GetCode(wavePrefix + "per_point");
    }

    // Custom shapes
    for (int index = 0; index < 4; index++)
    {
        Shape& shape = shapes[index];

        std::string const shapecodePrefix = "shapecode_" + std::to_string(index) + "_";

        shape.index = index;
        shape.enabled = parsedFile.GetBool(shapecodePrefix + "enabled", shape.enabled);
        shape.sides = parsedFile.GetInt(shapecodePrefix + "sides", shape.sides);
        shape.additive = parsedFile.GetBool(shapecodePrefix + "additive", shape.additive);
        shape.thickOutline = parsedFile.GetBool(shapecodePrefix + "thickOutline", shape.thickOutline);
        shape.textured = parsedFile.GetBool(shapecodePrefix + "textured", shape.textured);
        shape.instances = parsedFile.GetInt(shapecodePrefix + "nushape.inst", shape.instances);
        shape.x = parsedFile.GetFloat(shapecodePrefix + "x", shape.x);
        shape.y = parsedFile.GetFloat(shapecodePrefix + "y", shape.y);
        shape.radius = parsedFile.GetFloat(shapecodePrefix + "rad", shape.radius);
        shape.angle = parsedFile.GetFloat(shapecodePrefix + "ang", shape.angle);
        shape.tex_ang = parsedFile.GetFloat(shapecodePrefix + "tex_ang", shape.tex_ang);
        shape.tex_zoom = parsedFile.GetFloat(shapecodePrefix + "tex_zoom", shape.tex_zoom);
        shape.color.red = parsedFile.GetFloat(shapecodePrefix + "r", shape.color.red);
        shape.color.green = parsedFile.GetFloat(shapecodePrefix + "g", shape.color.green);
        shape.color.blue = parsedFile.GetFloat(shapecodePrefix + "b", shape.color.blue);
        shape.color.alpha = parsedFile.GetFloat(shapecodePrefix + "a", shape.color.alpha);
        shape.color2.red = parsedFile.GetFloat(shapecodePrefix + "r2", shape.color2.red);
        shape.color2.green = parsedFile.GetFloat(shapecodePrefix + "g2", shape.color2.green);
        shape.color2.blue = parsedFile.GetFloat(shapecodePrefix + "b2", shape.color2.blue);
        shape.color2.alpha = parsedFile.GetFloat(shapecodePrefix + "a2", shape.color2.alpha);
        shape.borderColor.red = parsedFile.GetFloat(shapecodePrefix + "border_r", shape.borderColor.red);
        shape.borderColor.green = parsedFile.GetFloat(shapecodePrefix + "border_g", shape.borderColor.green);
        shape.borderColor.blue = parsedFile.GetFloat(shapecodePrefix + "border_b", shape.borderColor.blue);
        shape.borderColor.alpha = parsedFile.GetFloat(shapecodePrefix + "border_a", shape.borderColor.alpha);

        std::string const shapePrefix = "shape_" + std::to_string(index) + "_";

        shape.initCode = parsedFile.GetCode(shapePrefix + "init");
        shape.perFrameCode = parsedFile.GetCode(shapePrefix + "per_frame");
    }

    // Shader code:
    warpShader = parsedFile.GetCode("warp_");
    compositeShader = parsedFile.GetCode("comp_");
}

void EditorPreset::ToParsedFile(PresetFile& parsedFile)
{
    // General:
    parsedFile.SetFloat("fDecay", decay);
    parsedFile.SetFloat("fGammaAdj", gammaAdj);
    parsedFile.SetFloat("fVideoEchoZoom", videoEchoZoom);
    parsedFile.SetFloat("fVideoEchoAlpha", videoEchoAlpha);
    parsedFile.SetInt("nVideoEchoOrientation", videoEchoOrientation);
    parsedFile.SetBool("bRedBlueStereo", redBlueStereo);
    parsedFile.SetBool("bBrighten", brighten);
    parsedFile.SetBool("bDarken", darken);
    parsedFile.SetBool("bSolarize", solarize);
    parsedFile.SetBool("bInvert", invert);
    parsedFile.SetFloat("fShader", shader);
    parsedFile.SetFloat("b1n", blur1Min);
    parsedFile.SetFloat("b2n", blur2Min);
    parsedFile.SetFloat("b3n", blur3Min);
    parsedFile.SetFloat("b1x", blur1Max);
    parsedFile.SetFloat("b2x", blur2Max);
    parsedFile.SetFloat("b3x", blur3Max);
    parsedFile.SetFloat("b1ed", blur1EdgeDarken);

    // Wave:
    parsedFile.SetInt("nWaveMode", waveMode);
    parsedFile.SetBool("bAdditiveWaves", additiveWaves);
    parsedFile.SetBool("bWaveDots", waveDots);
    parsedFile.SetBool("bWaveThick", waveThick);
    parsedFile.SetBool("bModWaveAlphaByVolume", modWaveAlphaByVolume);
    parsedFile.SetBool("bMaximizeWaveColor", maximizeWaveColor);
    parsedFile.SetFloat("fWaveAlpha", waveColor.alpha);
    parsedFile.SetFloat("fWaveScale", waveScale);
    parsedFile.SetFloat("fWaveSmoothing", waveSmoothing);
    parsedFile.SetFloat("fWaveParam", waveParam);
    parsedFile.SetFloat("fModWaveAlphaStart", modWaveAlphaStart);
    parsedFile.SetFloat("fModWaveAlphaEnd", modWaveAlphaEnd);
    parsedFile.SetFloat("wave_r", waveColor.red);
    parsedFile.SetFloat("wave_g", waveColor.green);
    parsedFile.SetFloat("wave_b", waveColor.blue);
    parsedFile.SetFloat("wave_x", waveX);
    parsedFile.SetFloat("wave_y", waveY);
    parsedFile.SetFloat("nMotionVectorsX", mvX);
    parsedFile.SetFloat("nMotionVectorsY", mvY);
    parsedFile.SetFloat("mv_dx", mvDX);
    parsedFile.SetFloat("mv_dy", mvDY);
    parsedFile.SetFloat("mv_l", mvL);
    parsedFile.SetFloat("mv_r", mvColor.red);
    parsedFile.SetFloat("mv_g", mvColor.green);
    parsedFile.SetFloat("mv_b", mvColor.blue);
    parsedFile.SetFloat("mv_a", mvColor.alpha);

    // Motion:
    parsedFile.SetFloat("zoom", zoom);
    parsedFile.SetFloat("rot", rot);
    parsedFile.SetFloat("cx", rotCX);
    parsedFile.SetFloat("cy", rotCY);
    parsedFile.SetFloat("dx", xPush);
    parsedFile.SetFloat("dy", yPush);
    parsedFile.SetFloat("warp", warpAmount);
    parsedFile.SetFloat("sx", stretchX);
    parsedFile.SetFloat("sy", stretchY);
    parsedFile.SetBool("bTexWrap", texWrap);
    parsedFile.SetBool("bDarkenCenter", darkenCenter);
    parsedFile.SetFloat("fWarpAnimSpeed", warpAnimSpeed);
    parsedFile.SetFloat("fWarpScale", warpScale);
    parsedFile.SetFloat("fZoomExponent", zoomExponent);

    // Borders:
    parsedFile.SetFloat("ob_size", outerBorderSize);
    parsedFile.SetFloat("ob_r", outerBorderColor.red);
    parsedFile.SetFloat("ob_g", outerBorderColor.green);
    parsedFile.SetFloat("ob_b", outerBorderColor.blue);
    parsedFile.SetFloat("ob_a", outerBorderColor.alpha);
    parsedFile.SetFloat("ib_size", innerBorderSize);
    parsedFile.SetFloat("ib_r", innerBorderColor.red);
    parsedFile.SetFloat("ib_g", innerBorderColor.green);
    parsedFile.SetFloat("ib_b", innerBorderColor.blue);
    parsedFile.SetFloat("ib_a", innerBorderColor.alpha);

    // Versions:
    parsedFile.SetInt("MILKDROP_PRESET_VERSION", presetVersion);
    if (presetVersion < 200)
    {
        // Milkdrop 1.x did not use shaders.
        parsedFile.SetInt("PSVERSION", 0);
        parsedFile.SetInt("PSVERSION_WARP", 0);
        parsedFile.SetInt("PSVERSION_COMP", 0);
    }
    else
    {
        parsedFile.SetInt("PSVERSION", warpShaderVersion);
        parsedFile.SetInt("PSVERSION_WARP", warpShaderVersion);
        parsedFile.SetInt("PSVERSION_COMP", compositeShaderVersion);
    }

    // Code:
    parsedFile.SetCode("per_frame_init_", perFrameInitCode);
    parsedFile.SetCode("per_frame_", perFrameCode);
    parsedFile.SetCode("per_pixel_", perPixelCode);

    // Custom waveform code:
    for (int index = 0; index < 4; index++)
    {
        Wave& wave = waves[index];

        std::string const wavecodePrefix = "wavecode_" + std::to_string(index) + "_";

        parsedFile.SetInt(wavecodePrefix + "enabled", wave.enabled);
        parsedFile.SetInt(wavecodePrefix + "samples", wave.samples);
        parsedFile.SetInt(wavecodePrefix + "sep", wave.sep);
        parsedFile.SetBool(wavecodePrefix + "bSpectrum", wave.spectrum);
        parsedFile.SetBool(wavecodePrefix + "bUseDots", wave.useDots);
        parsedFile.SetBool(wavecodePrefix + "bDrawThick", wave.drawThick);
        parsedFile.SetBool(wavecodePrefix + "bAdditive", wave.additive);
        parsedFile.SetFloat(wavecodePrefix + "scaling", wave.scaling);
        parsedFile.SetFloat(wavecodePrefix + "smoothing", wave.smoothing);
        parsedFile.SetFloat(wavecodePrefix + "r", wave.color.red);
        parsedFile.SetFloat(wavecodePrefix + "g", wave.color.green);
        parsedFile.SetFloat(wavecodePrefix + "b", wave.color.blue);
        parsedFile.SetFloat(wavecodePrefix + "a", wave.color.alpha);

        std::string const wavePrefix = "wave_" + std::to_string(index) + "_";

        parsedFile.SetCode(wavePrefix + "init", wave.initCode);
        parsedFile.SetCode(wavePrefix + "per_frame", wave.perFrameCode);
        parsedFile.SetCode(wavePrefix + "per_point", wave.perPointCode);
    }

    // Custom shapes
    for (int index = 0; index < 4; index++)
    {
        Shape& shape = shapes[index];

        std::string const shapecodePrefix = "shapecode_" + std::to_string(index) + "_";

        parsedFile.SetBool(shapecodePrefix + "enabled", shape.enabled);
        parsedFile.SetInt(shapecodePrefix + "sides", shape.sides);
        parsedFile.SetBool(shapecodePrefix + "additive", shape.additive);
        parsedFile.SetBool(shapecodePrefix + "thickOutline", shape.thickOutline);
        parsedFile.SetBool(shapecodePrefix + "textured", shape.textured);
        parsedFile.SetInt(shapecodePrefix + "nushape.inst", shape.instances);
        parsedFile.SetFloat(shapecodePrefix + "x", shape.x);
        parsedFile.SetFloat(shapecodePrefix + "y", shape.y);
        parsedFile.SetFloat(shapecodePrefix + "rad", shape.radius);
        parsedFile.SetFloat(shapecodePrefix + "ang", shape.angle);
        parsedFile.SetFloat(shapecodePrefix + "tex_ang", shape.tex_ang);
        parsedFile.SetFloat(shapecodePrefix + "tex_zoom", shape.tex_zoom);
        parsedFile.SetFloat(shapecodePrefix + "r", shape.color.red);
        parsedFile.SetFloat(shapecodePrefix + "g", shape.color.green);
        parsedFile.SetFloat(shapecodePrefix + "b", shape.color.blue);
        parsedFile.SetFloat(shapecodePrefix + "a", shape.color.alpha);
        parsedFile.SetFloat(shapecodePrefix + "r2", shape.color2.red);
        parsedFile.SetFloat(shapecodePrefix + "g2", shape.color2.green);
        parsedFile.SetFloat(shapecodePrefix + "b2", shape.color2.blue);
        parsedFile.SetFloat(shapecodePrefix + "a2", shape.color2.alpha);
        parsedFile.SetFloat(shapecodePrefix + "border_r", shape.borderColor.red);
        parsedFile.SetFloat(shapecodePrefix + "border_g", shape.borderColor.green);
        parsedFile.SetFloat(shapecodePrefix + "border_b", shape.borderColor.blue);
        parsedFile.SetFloat(shapecodePrefix + "border_a", shape.borderColor.alpha);

        std::string const shapePrefix = "shape_" + std::to_string(index) + "_";

        parsedFile.SetCode(shapePrefix + "init", shape.initCode);
        parsedFile.SetCode(shapePrefix + "per_frame", shape.perFrameCode);
    }

    // Shader code:
    parsedFile.SetCode("warp_", warpShader);
    parsedFile.SetCode("comp_", compositeShader);
}

} // namespace Editor
