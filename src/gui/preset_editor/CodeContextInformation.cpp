#include "CodeContextInformation.h"

#include "IconsFontAwesome7.h"

#include <map>

namespace Editor {

std::string CodeContextInformation::GetContextName(ExpressionCodeTypes type, int index)
{
    switch (type)
    {
        case ExpressionCodeTypes::PerFrameInit:
            return "Per-Frame Init";
        case ExpressionCodeTypes::PerFrame:
            return "Per-Frame";
        case ExpressionCodeTypes::PerVertex:
            return "Per-Vertex";
        case ExpressionCodeTypes::CustomWaveInit:
            return "Wave " + std::to_string(index) + " Init";
        case ExpressionCodeTypes::CustomWavePerFrame:
            return "Wave " + std::to_string(index) + " Per-Frame";
        case ExpressionCodeTypes::CustomWavePerPoint:
            return "Wave " + std::to_string(index) + " Per-Point";
        case ExpressionCodeTypes::CustomShapeInit:
            return "Shape " + std::to_string(index) + " Init";
        case ExpressionCodeTypes::CustomShapePerFrame:
            return "Shape " + std::to_string(index) + " Per-Frame";
        case ExpressionCodeTypes::WarpShader:
            return "Warp Shader";
        case ExpressionCodeTypes::CompositeShader:
            return "Composite Shader";
    }

    throw std::runtime_error(std::string("Unknown value for argument ExpressionCodeTypes type in CodeContextInformation::GetContextName()"));
}

std::vector<std::pair<const char*, const char*>> CodeContextInformation::GetIdentifierList(ExpressionCodeTypes type)
{
    static std::map<ExpressionCodeTypes, std::vector<std::pair<const char*, const char*>>> identifierList{
        {ExpressionCodeTypes::PerFrameInit, {{"zoom", "Controls inward/outward motion.\nWritable."}, //
                                             {"zoomexp", "Controls the curvature of the zoom.\nWritable."}, //
                                             {"rot", "Controls the amount of rotation.\nWritable."}, //
                                             {"warp", "Controls the magnitude of the warping.\nWritable."}, //
                                             {"cx", "Controls where the center of rotation and stretching is, horizontally.\nWritable."}, //
                                             {"cy", "Controls where the center of rotation and stretching is, vertically.\nWritable."}, //
                                             {"dx", "Controls amount of constant horizontal motion.\nWritable."}, //
                                             {"dy", "Controls amount of constant vertical motion.\nWritable."}, //
                                             {"sx", "Controls amount of constant horizontal stretching.\nWritable."}, //
                                             {"sy", "Controls amount of constant vertical stretching.\nWritable."}, //
                                             {"wave_mode", "Controls which of the 8/16 types of waveform is drawn.\nWritable."}, //
                                             {"wave_x", "Horizontal position of the waveform (0..1).\nWritable."}, //
                                             {"wave_y", "Vertical position of the waveform (0..1).\nWritable."}, //
                                             {"wave_r", "Amount of red color in the wave (0..1).\nWritable."}, //
                                             {"wave_g", "Amount of green color in the wave (0..1).\nWritable."}, //
                                             {"wave_b", "Amount of blue color in the wave (0..1).\nWritable."}, //
                                             {"wave_a", "Opacity of the wave (0..1).\nWritable."}, //
                                             {"wave_mystery", "This value does different things for each waveform (-1..1).\nWritable."}, //
                                             {"wave_usedots", "If 1, the waveform is drawn as dots instead of lines (0/1).\nWritable."}, //
                                             {"wave_thick", "If 1, the waveform's lines (or dots) are drawn with double thickness (0/1).\nWritable."}, //
                                             {"wave_additive", "If 1, the wave is drawn additively, saturating the image at white (0/1).\nWritable."}, //
                                             {"wave_brighten", "If 1, all 3 r/g/b colors will be scaled up until at least one reaches 1.0 (0/1)\nWritable."}, //
                                             {"ob_size", "Thickness of the outer border drawn at the edges of the screen every frame (0..0.5).\nWritable."}, //
                                             {"ob_r", "Amount of red color in the outer border (0..1).\nWritable."}, //
                                             {"ob_g", "Amount of green color in the outer border (0..1).\nWritable."}, //
                                             {"ob_b", "Amount of blue color in the outer border (0..1).\nWritable."}, //
                                             {"ob_a", "Opacity of the outer border (0=transparent, 1=opaque).\nWritable."}, //
                                             {"ib_size", "Thickness of the inner border drawn at the edges of the screen every frame (0..0.5).\nWritable."}, //
                                             {"ib_r", "Amount of red color in the inner border (0..1).\nWritable."}, //
                                             {"ib_g", "Amount of green color in the inner border (0..1).\nWritable."}, //
                                             {"ib_b", "Amount of blue color in the inner border (0..1).\nWritable."}, //
                                             {"ib_a", "Opacity of the inner border (0=transparent, 1=opaque).\nWritable."}, //
                                             {"mv_r", "Amount of red color in the motion vectors (0..1).\nWritable."}, //
                                             {"mv_g", "Amount of green color in the motion vectors (0..1).\nWritable."}, //
                                             {"mv_b", "Amount of blue color in the motion vectors (0..1).\nWritable."}, //
                                             {"mv_a", "Opacity of the motion vectors (0=transparent, 1=opaque).\nWritable."}, //
                                             {"mv_x", "The number of motion vectors in the X direction (0..64).\nWritable."}, //
                                             {"mv_y", "The number of motion vectors in the Y direction (0..48).\nWritable."}, //
                                             {"mv_l", "The length of the motion vectors (0=no trail, 1=normal, 2=double...).\nWritable."}, //
                                             {"mv_dx", "Horizontal placement offset of the motion vectors (-1..1).\nWritable."}, //
                                             {"mv_dy", "Vertical placement offset of the motion vectors (-1..1).\nWritable."}, //
                                             {"decay", "Controls the eventual fade to black (1=no fade, 0.9=strong fade, 0.98=recommended).\nWritable."}, //
                                             {"gamma", "Controls display brightness  (1=normal, 2=double, 3=triple, etc.).\nWritable."}, //
                                             {"echo_zoom", "Controls the size of the second graphics layer (>0).\nWritable."}, //
                                             {"echo_alpha", "Controls the opacity of the second graphics layer (0=transparent/off, 0.5=half-mix, 1=opaque).\nWritable."}, //
                                             {"echo_orient", "Selects an orientation for the second graphics layer (0=normal, 1=flip on x, 2=flip on y, 3=flip on both).\nWritable."}, //
                                             {"darken_center", "If 1, help keeps the image from getting too bright by continually dimming the center point (0/1).\nWritable."}, //
                                             {"wrap", "Sets whether or not screen elements can drift off of one side and onto the other (0/1).\nWritable."}, //
                                             {"invert", "Inverts the colors in the image (0/1).\nWritable."}, //
                                             {"brighten", "Brightens the darker parts of the image (0/1).\nWritable."}, //
                                             {"darken", "Darkens the brighter parts of the image (0/1).\nWritable."}, //
                                             {"solarize", "Emphasizes mid-range colors (0/1).\nWritable."}, //
                                             {"monitor", "Value for debugging preset code. Unsupported in projectM.\nWritable."}, //
                                             {"time", "Retrieves the current time, in seconds, since program start.\nREAD-ONLY."}, //
                                             {"fps", "Retrieves the current framerate, in frames per second.\nREAD-ONLY."}, //
                                             {"frame", "Retrieves the number of frames of animation elapsed since the program started.\nREAD-ONLY."}, //
                                             {"progress", "Progress through the current preset.\nREAD-ONLY."}, //
                                             {"bass", "Retrieves the current amount of bass (1=normal, <~0.7=quiet, >1.3=loud).\nREAD-ONLY."}, //
                                             {"mid", "Retrieves the current amount of middles (1=normal, <~0.7=quiet, >1.3=loud).\nREAD-ONLY."}, //
                                             {"treb", "Retrieves the current amount of treble (1=normal, <~0.7=quiet, >1.3=loud).\nREAD-ONLY."}, //
                                             {"bass_att", "Retrieves the attenuated amount of bass (1=normal, <~0.7=quiet, >1.3=loud).\nREAD-ONLY."}, //
                                             {"mid_att", "Retrieves the attenuated amount of middles (1=normal, <~0.7=quiet, >1.3=loud).\nREAD-ONLY."}, //
                                             {"treb_att", "Retrieves the attenuated amount of treble (1=normal, <~0.7=quiet, >1.3=loud).\nREAD-ONLY."}, //
                                             {"meshx", "Tells you the user's per-vertex mesh size in the X direction. Always an integer value.\nREAD-ONLY."}, //
                                             {"meshy", "Tells you the user's per-vertex mesh size in the Y direction. Always an integer value.\nREAD-ONLY."}, //
                                             {"pixelsx", "Width of the rendering canvas, in pixels.\nREAD-ONLY."}, //
                                             {"pixelsy", "Height of the rendering canvas, in pixels.\nREAD-ONLY."}, //
                                             {"aspectx", "Multiply an X coordinate by this to make the preset look the same at any aspect (>0).\nREAD-ONLY."}, //
                                             {"aspecty", "Multiply a Y coordinate by this to make the preset look the same at any aspect (>0).\nREAD-ONLY."}, //
                                             {"blur1_min", "Blur level 1 range minimum (0..1).\nWritable."}, //
                                             {"blur1_max", "Blur level 1 range maximum (0..1).\nWritable."}, //
                                             {"blur2_min", "Blur level 2 range minimum (0..1).\nWritable."}, //
                                             {"blur2_max", "Blur level 2 range maximum (0..1).\nWritable."}, //
                                             {"blur3_min", "Blur level 3 range minimum (0..1).\nWritable."}, //
                                             {"blur3_max", "Blur level 3 range maximum (0..1).\nWritable."}, //
                                             {"blur1_edge_darken", "Amount of edge darkening at blur level 1 (0..1).\nWritable."}, //
                                             {"q1", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                             {"q2", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                             {"q3", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                             {"q4", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                             {"q5", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                             {"q6", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                             {"q7", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                             {"q8", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                             {"q9", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                             {"q10", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                             {"q11", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                             {"q12", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                             {"q13", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                             {"q14", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                             {"q15", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                             {"q16", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                             {"q17", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                             {"q18", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                             {"q19", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                             {"q20", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                             {"q21", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                             {"q22", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                             {"q23", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                             {"q24", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                             {"q25", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                             {"q26", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                             {"q27", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                             {"q28", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                             {"q29", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                             {"q30", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                             {"q31", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                             {"q32", "q1 to q32 values are carried along a chain to each code block.\nWritable."}}},

        {ExpressionCodeTypes::PerFrame, {{"zoom", "Controls inward/outward motion.\nWritable."}, //
                                         {"zoomexp", "Controls the curvature of the zoom.\nWritable."}, //
                                         {"rot", "Controls the amount of rotation.\nWritable."}, //
                                         {"warp", "Controls the magnitude of the warping.\nWritable."}, //
                                         {"cx", "Controls where the center of rotation and stretching is, horizontally.\nWritable."}, //
                                         {"cy", "Controls where the center of rotation and stretching is, vertically.\nWritable."}, //
                                         {"dx", "Controls amount of constant horizontal motion.\nWritable."}, //
                                         {"dy", "Controls amount of constant vertical motion.\nWritable."}, //
                                         {"sx", "Controls amount of constant horizontal stretching.\nWritable."}, //
                                         {"sy", "Controls amount of constant vertical stretching.\nWritable."}, //
                                         {"wave_mode", "Controls which of the 8/16 types of waveform is drawn.\nWritable."}, //
                                         {"wave_x", "Horizontal position of the waveform (0..1).\nWritable."}, //
                                         {"wave_y", "Vertical position of the waveform (0..1).\nWritable."}, //
                                         {"wave_r", "Amount of red color in the wave (0..1).\nWritable."}, //
                                         {"wave_g", "Amount of green color in the wave (0..1).\nWritable."}, //
                                         {"wave_b", "Amount of blue color in the wave (0..1).\nWritable."}, //
                                         {"wave_a", "Opacity of the wave (0..1).\nWritable."}, //
                                         {"wave_mystery", "This value does different things for each waveform (-1..1).\nWritable."}, //
                                         {"wave_usedots", "If 1, the waveform is drawn as dots instead of lines (0/1).\nWritable."}, //
                                         {"wave_thick", "If 1, the waveform's lines (or dots) are drawn with double thickness (0/1).\nWritable."}, //
                                         {"wave_additive", "If 1, the wave is drawn additively, saturating the image at white (0/1).\nWritable."}, //
                                         {"wave_brighten", "If 1, all 3 r/g/b colors will be scaled up until at least one reaches 1.0 (0/1)\nWritable."}, //
                                         {"ob_size", "Thickness of the outer border drawn at the edges of the screen every frame (0..0.5).\nWritable."}, //
                                         {"ob_r", "Amount of red color in the outer border (0..1).\nWritable."}, //
                                         {"ob_g", "Amount of green color in the outer border (0..1).\nWritable."}, //
                                         {"ob_b", "Amount of blue color in the outer border (0..1).\nWritable."}, //
                                         {"ob_a", "Opacity of the outer border (0=transparent, 1=opaque).\nWritable."}, //
                                         {"ib_size", "Thickness of the inner border drawn at the edges of the screen every frame (0..0.5).\nWritable."}, //
                                         {"ib_r", "Amount of red color in the inner border (0..1).\nWritable."}, //
                                         {"ib_g", "Amount of green color in the inner border (0..1).\nWritable."}, //
                                         {"ib_b", "Amount of blue color in the inner border (0..1).\nWritable."}, //
                                         {"ib_a", "Opacity of the inner border (0=transparent, 1=opaque).\nWritable."}, //
                                         {"mv_r", "Amount of red color in the motion vectors (0..1).\nWritable."}, //
                                         {"mv_g", "Amount of green color in the motion vectors (0..1).\nWritable."}, //
                                         {"mv_b", "Amount of blue color in the motion vectors (0..1).\nWritable."}, //
                                         {"mv_a", "Opacity of the motion vectors (0=transparent, 1=opaque).\nWritable."}, //
                                         {"mv_x", "The number of motion vectors in the X direction (0..64).\nWritable."}, //
                                         {"mv_y", "The number of motion vectors in the Y direction (0..48).\nWritable."}, //
                                         {"mv_l", "The length of the motion vectors (0=no trail, 1=normal, 2=double...).\nWritable."}, //
                                         {"mv_dx", "Horizontal placement offset of the motion vectors (-1..1).\nWritable."}, //
                                         {"mv_dy", "Vertical placement offset of the motion vectors (-1..1).\nWritable."}, //
                                         {"decay", "Controls the eventual fade to black (1=no fade, 0.9=strong fade, 0.98=recommended).\nWritable."}, //
                                         {"gamma", "Controls display brightness  (1=normal, 2=double, 3=triple, etc.).\nWritable."}, //
                                         {"echo_zoom", "Controls the size of the second graphics layer (>0).\nWritable."}, //
                                         {"echo_alpha", "Controls the opacity of the second graphics layer (0=transparent/off, 0.5=half-mix, 1=opaque).\nWritable."}, //
                                         {"echo_orient", "Selects an orientation for the second graphics layer (0=normal, 1=flip on x, 2=flip on y, 3=flip on both).\nWritable."}, //
                                         {"darken_center", "If 1, help keeps the image from getting too bright by continually dimming the center point (0/1).\nWritable."}, //
                                         {"wrap", "Sets whether or not screen elements can drift off of one side and onto the other (0/1).\nWritable."}, //
                                         {"invert", "Inverts the colors in the image (0/1).\nWritable."}, //
                                         {"brighten", "Brightens the darker parts of the image (0/1).\nWritable."}, //
                                         {"darken", "Darkens the brighter parts of the image (0/1).\nWritable."}, //
                                         {"solarize", "Emphasizes mid-range colors (0/1).\nWritable."}, //
                                         {"monitor", "Value for debugging preset code. Unsupported in projectM.\nWritable."}, //
                                         {"time", "Retrieves the current time, in seconds, since program start.\nREAD-ONLY."}, //
                                         {"fps", "Retrieves the current framerate, in frames per second.\nREAD-ONLY."}, //
                                         {"frame", "Retrieves the number of frames of animation elapsed since the program started.\nREAD-ONLY."}, //
                                         {"progress", "Progress through the current preset.\nREAD-ONLY."}, //
                                         {"bass", "Retrieves the current amount of bass (1=normal, <~0.7=quiet, >1.3=loud).\nREAD-ONLY."}, //
                                         {"mid", "Retrieves the current amount of middles (1=normal, <~0.7=quiet, >1.3=loud).\nREAD-ONLY."}, //
                                         {"treb", "Retrieves the current amount of treble (1=normal, <~0.7=quiet, >1.3=loud).\nREAD-ONLY."}, //
                                         {"bass_att", "Retrieves the attenuated amount of bass (1=normal, <~0.7=quiet, >1.3=loud).\nREAD-ONLY."}, //
                                         {"mid_att", "Retrieves the attenuated amount of middles (1=normal, <~0.7=quiet, >1.3=loud).\nREAD-ONLY."}, //
                                         {"treb_att", "Retrieves the attenuated amount of treble (1=normal, <~0.7=quiet, >1.3=loud).\nREAD-ONLY."}, //
                                         {"meshx", "Tells you the user's per-vertex mesh size in the X direction. Always an integer value.\nREAD-ONLY."}, //
                                         {"meshy", "Tells you the user's per-vertex mesh size in the Y direction. Always an integer value.\nREAD-ONLY."}, //
                                         {"pixelsx", "Width of the rendering canvas, in pixels.\nREAD-ONLY."}, //
                                         {"pixelsy", "Height of the rendering canvas, in pixels.\nREAD-ONLY."}, //
                                         {"aspectx", "Multiply an X coordinate by this to make the preset look the same at any aspect (>0).\nREAD-ONLY."}, //
                                         {"aspecty", "Multiply a Y coordinate by this to make the preset look the same at any aspect (>0).\nREAD-ONLY."}, //
                                         {"blur1_min", "Blur level 1 range minimum (0..1).\nWritable."}, //
                                         {"blur1_max", "Blur level 1 range maximum (0..1).\nWritable."}, //
                                         {"blur2_min", "Blur level 2 range minimum (0..1).\nWritable."}, //
                                         {"blur2_max", "Blur level 2 range maximum (0..1).\nWritable."}, //
                                         {"blur3_min", "Blur level 3 range minimum (0..1).\nWritable."}, //
                                         {"blur3_max", "Blur level 3 range maximum (0..1).\nWritable."}, //
                                         {"blur1_edge_darken", "Amount of edge darkening at blur level 1 (0..1).\nWritable."}, //
                                         {"q1", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                         {"q2", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                         {"q3", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                         {"q4", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                         {"q5", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                         {"q6", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                         {"q7", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                         {"q8", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                         {"q9", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                         {"q10", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                         {"q11", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                         {"q12", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                         {"q13", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                         {"q14", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                         {"q15", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                         {"q16", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                         {"q17", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                         {"q18", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                         {"q19", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                         {"q20", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                         {"q21", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                         {"q22", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                         {"q23", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                         {"q24", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                         {"q25", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                         {"q26", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                         {"q27", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                         {"q28", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                         {"q29", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                         {"q30", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                         {"q31", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                         {"q32", "q1 to q32 values are carried along a chain to each code block.\nWritable."}}},

        {ExpressionCodeTypes::PerVertex, {{"x", "Retrieves the X position of the current pixel (0..1).\nREAD-ONLY."}, //
                                          {"y", "Retrieves the Y position of the current pixel (0..1).\nREAD-ONLY."}, //
                                          {"rad", "Retrieves the distance of the pixel from the center of the screen.\nAt the center of the screen this will be zero, and at the corners, 1.\nREAD-ONLY."}, //
                                          {"ang", "Retrieves the angle of the current pixel, with respect to the center of the screen.\nThis is equal to the arctangent of y over x.\nREAD-ONLY."}, //
                                          {"zoom", "Controls inward/outward motion.\nWritable."}, //
                                          {"zoomexp", "Controls the curvature of the zoom.\nWritable."}, //
                                          {"rot", "Controls the amount of rotation.\nWritable."}, //
                                          {"warp", "Controls the magnitude of the warping.\nWritable."}, //
                                          {"cx", "Controls where the center of rotation and stretching is, horizontally.\nWritable."}, //
                                          {"cy", "Controls where the center of rotation and stretching is, vertically.\nWritable."}, //
                                          {"dx", "Controls amount of constant horizontal motion.\nWritable."}, //
                                          {"dy", "Controls amount of constant vertical motion.\nWritable."}, //
                                          {"sx", "Controls amount of constant horizontal stretching.\nWritable."}, //
                                          {"sy", "Controls amount of constant vertical stretching.\nWritable."}, //
                                          {"time", "Retrieves the current time, in seconds, since program start.\nREAD-ONLY."}, //
                                          {"fps", "Retrieves the current framerate, in frames per second.\nREAD-ONLY."}, //
                                          {"frame", "Retrieves the number of frames of animation elapsed since the program started.\nREAD-ONLY."}, //
                                          {"progress", "Progress through the current preset.\nREAD-ONLY."}, //
                                          {"bass", "Retrieves the current amount of bass (1=normal, <~0.7=quiet, >1.3=loud).\nREAD-ONLY."}, //
                                          {"mid", "Retrieves the current amount of middles (1=normal, <~0.7=quiet, >1.3=loud).\nREAD-ONLY."}, //
                                          {"treb", "Retrieves the current amount of treble (1=normal, <~0.7=quiet, >1.3=loud).\nREAD-ONLY."}, //
                                          {"bass_att", "Retrieves the attenuated amount of bass (1=normal, <~0.7=quiet, >1.3=loud).\nREAD-ONLY."}, //
                                          {"mid_att", "Retrieves the attenuated amount of middles (1=normal, <~0.7=quiet, >1.3=loud).\nREAD-ONLY."}, //
                                          {"treb_att", "Retrieves the attenuated amount of treble (1=normal, <~0.7=quiet, >1.3=loud).\nREAD-ONLY."}, //
                                          {"meshx", "Tells you the user's per-vertex mesh size in the X direction. Always an integer value.\nREAD-ONLY."}, //
                                          {"meshy", "Tells you the user's per-vertex mesh size in the Y direction. Always an integer value.\nREAD-ONLY."}, //
                                          {"pixelsx", "Width of the rendering canvas, in pixels.\nREAD-ONLY."}, //
                                          {"pixelsy", "Height of the rendering canvas, in pixels.\nREAD-ONLY."}, //
                                          {"aspectx", "Multiply an X coordinate by this to make the preset look the same at any aspect (>0).\nREAD-ONLY."}, //
                                          {"aspecty", "Multiply a Y coordinate by this to make the preset look the same at any aspect (>0).\nREAD-ONLY."}, //
                                          {"q1", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                          {"q2", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                          {"q3", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                          {"q4", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                          {"q5", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                          {"q6", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                          {"q7", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                          {"q8", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                          {"q9", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                          {"q10", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                          {"q11", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                          {"q12", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                          {"q13", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                          {"q14", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                          {"q15", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                          {"q16", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                          {"q17", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                          {"q18", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                          {"q19", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                          {"q20", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                          {"q21", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                          {"q22", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                          {"q23", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                          {"q24", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                          {"q25", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                          {"q26", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                          {"q27", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                          {"q28", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                          {"q29", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                          {"q30", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                          {"q31", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                          {"q32", "q1 to q32 values are carried along a chain to each code block.\nWritable."}}},

        {ExpressionCodeTypes::CustomWaveInit, {{"r", "Base amount of red color in the wave (0..1).\nWritable."}, //
                                               {"g", "Base amount of green color in the wave (0..1).\nWritable."}, //
                                               {"b", "Base amount of blue color in the wave (0..1).\nWritable."}, //
                                               {"a", "Base opacity of the waveform (0=transparent, 1=opaque).\nWritable."}, //
                                               {"samples", "Read: retrieves the # of samples specified for this custom wave (from the menu).\nWrite: lets you dynamically change that #, frame to frame (0..512).\nWritable."}, //
                                               {"time", "Retrieves the current time, in seconds, since program start.\nREAD-ONLY."}, //
                                               {"fps", "Retrieves the current framerate, in frames per second.\nREAD-ONLY."}, //
                                               {"frame", "Retrieves the number of frames of animation elapsed since the program started.\nREAD-ONLY."}, //
                                               {"progress", "Progress through the current preset.\nREAD-ONLY."}, //
                                               {"bass", "Retrieves the current amount of bass (1=normal, <~0.7=quiet, >1.3=loud).\nREAD-ONLY."}, //
                                               {"mid", "Retrieves the current amount of middles (1=normal, <~0.7=quiet, >1.3=loud).\nREAD-ONLY."}, //
                                               {"treb", "Retrieves the current amount of treble (1=normal, <~0.7=quiet, >1.3=loud).\nREAD-ONLY."}, //
                                               {"bass_att", "Retrieves the attenuated amount of bass (1=normal, <~0.7=quiet, >1.3=loud).\nREAD-ONLY."}, //
                                               {"mid_att", "Retrieves the attenuated amount of middles (1=normal, <~0.7=quiet, >1.3=loud).\nREAD-ONLY."}, //
                                               {"treb_att", "Retrieves the attenuated amount of treble (1=normal, <~0.7=quiet, >1.3=loud).\nREAD-ONLY."}, //
                                               {"q1", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                               {"q2", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                               {"q3", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                               {"q4", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                               {"q5", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                               {"q6", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                               {"q7", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                               {"q8", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                               {"q9", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                               {"q10", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                               {"q11", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                               {"q12", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                               {"q13", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                               {"q14", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                               {"q15", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                               {"q16", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                               {"q17", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                               {"q18", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                               {"q19", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                               {"q20", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                               {"q21", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                               {"q22", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                               {"q23", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                               {"q24", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                               {"q25", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                               {"q26", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                               {"q27", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                               {"q28", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                               {"q29", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                               {"q30", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                               {"q31", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                               {"q32", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                               {"t1", "t1 to t8 values are carried from each wave init to per-frame to per-point.\nWritable."}, //
                                               {"t2", "t1 to t8 values are carried from each wave init to per-frame to per-point.\nWritable."}, //
                                               {"t3", "t1 to t8 values are carried from each wave init to per-frame to per-point.\nWritable."}, //
                                               {"t4", "t1 to t8 values are carried from each wave init to per-frame to per-point.\nWritable."}, //
                                               {"t5", "t1 to t8 values are carried from each wave init to per-frame to per-point.\nWritable."}, //
                                               {"t6", "t1 to t8 values are carried from each wave init to per-frame to per-point.\nWritable."}, //
                                               {"t7", "t1 to t8 values are carried from each wave init to per-frame to per-point.\nWritable."}, //
                                               {"t8", "t1 to t8 values are carried from each wave init to per-frame to per-point.\nWritable."}}},

        {ExpressionCodeTypes::CustomWavePerFrame, {{"r", "Base amount of red color in the wave (0..1).\nWritable."}, //
                                                   {"g", "Base amount of green color in the wave (0..1).\nWritable."}, //
                                                   {"b", "Base amount of blue color in the wave (0..1).\nWritable."}, //
                                                   {"a", "Base opacity of the waveform (0=transparent, 1=opaque).\nWritable."}, //
                                                   {"samples", "Read: retrieves the # of samples specified for this custom wave (from the menu).\nWrite: lets you dynamically change that #, frame to frame (0..512).\nWritable."}, //
                                                   {"time", "Retrieves the current time, in seconds, since program start.\nREAD-ONLY."}, //
                                                   {"fps", "Retrieves the current framerate, in frames per second.\nREAD-ONLY."}, //
                                                   {"frame", "Retrieves the number of frames of animation elapsed since the program started.\nREAD-ONLY."}, //
                                                   {"progress", "Progress through the current preset.\nREAD-ONLY."}, //
                                                   {"bass", "Retrieves the current amount of bass (1=normal, <~0.7=quiet, >1.3=loud).\nREAD-ONLY."}, //
                                                   {"mid", "Retrieves the current amount of middles (1=normal, <~0.7=quiet, >1.3=loud).\nREAD-ONLY."}, //
                                                   {"treb", "Retrieves the current amount of treble (1=normal, <~0.7=quiet, >1.3=loud).\nREAD-ONLY."}, //
                                                   {"bass_att", "Retrieves the attenuated amount of bass (1=normal, <~0.7=quiet, >1.3=loud).\nREAD-ONLY."}, //
                                                   {"mid_att", "Retrieves the attenuated amount of middles (1=normal, <~0.7=quiet, >1.3=loud).\nREAD-ONLY."}, //
                                                   {"treb_att", "Retrieves the attenuated amount of treble (1=normal, <~0.7=quiet, >1.3=loud).\nREAD-ONLY."}, //
                                                   {"q1", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q2", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q3", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q4", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q5", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q6", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q7", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q8", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q9", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q10", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q11", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q12", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q13", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q14", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q15", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q16", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q17", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q18", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q19", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q20", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q21", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q22", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q23", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q24", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q25", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q26", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q27", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q28", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q29", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q30", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q31", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q32", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"t1", "t1 to t8 values are carried from each wave init to per-frame to per-point.\nWritable."}, //
                                                   {"t2", "t1 to t8 values are carried from each wave init to per-frame to per-point.\nWritable."}, //
                                                   {"t3", "t1 to t8 values are carried from each wave init to per-frame to per-point.\nWritable."}, //
                                                   {"t4", "t1 to t8 values are carried from each wave init to per-frame to per-point.\nWritable."}, //
                                                   {"t5", "t1 to t8 values are carried from each wave init to per-frame to per-point.\nWritable."}, //
                                                   {"t6", "t1 to t8 values are carried from each wave init to per-frame to per-point.\nWritable."}, //
                                                   {"t7", "t1 to t8 values are carried from each wave init to per-frame to per-point.\nWritable."}, //
                                                   {"t8", "t1 to t8 values are carried from each wave init to per-frame to per-point.\nWritable."}}},

        {ExpressionCodeTypes::CustomWavePerPoint, {{"r", "Base amount of red color in the wave (0..1).\nWritable."}, //
                                                   {"g", "Base amount of green color in the wave (0..1).\nWritable."}, //
                                                   {"b", "Base amount of blue color in the wave (0..1).\nWritable."}, //
                                                   {"a", "Base opacity of the waveform (0=transparent, 1=opaque).\nWritable."}, //
                                                   {"x", "The X position of this point that makes up the wave (0=left, 1=right).\nWritable."}, //
                                                   {"y", "The Y position of this point that makes up the wave (0=left, 1=right).\nWritable."}, //
                                                   {"sample", "How far along we are, through the samples that make up the waveform (0=first sample, 0.5 = half-way through, 1=last sample).\nREAD-ONLY."}, //
                                                   {"value1", "The value of the left audio channel sample or spectrum at this point in the waveform.\nREAD-ONLY."}, //
                                                   {"value2", "The value of the right audio channel sample or spectrum at this point in the waveform.\nREAD-ONLY."}, //
                                                   {"time", "Retrieves the current time, in seconds, since program start.\nREAD-ONLY."}, //
                                                   {"fps", "Retrieves the current framerate, in frames per second.\nREAD-ONLY."}, //
                                                   {"frame", "Retrieves the number of frames of animation elapsed since the program started.\nREAD-ONLY."}, //
                                                   {"progress", "Progress through the current preset.\nREAD-ONLY."}, //
                                                   {"bass", "Retrieves the current amount of bass (1=normal, <~0.7=quiet, >1.3=loud).\nREAD-ONLY."}, //
                                                   {"mid", "Retrieves the current amount of middles (1=normal, <~0.7=quiet, >1.3=loud).\nREAD-ONLY."}, //
                                                   {"treb", "Retrieves the current amount of treble (1=normal, <~0.7=quiet, >1.3=loud).\nREAD-ONLY."}, //
                                                   {"bass_att", "Retrieves the attenuated amount of bass (1=normal, <~0.7=quiet, >1.3=loud).\nREAD-ONLY."}, //
                                                   {"mid_att", "Retrieves the attenuated amount of middles (1=normal, <~0.7=quiet, >1.3=loud).\nREAD-ONLY."}, //
                                                   {"treb_att", "Retrieves the attenuated amount of treble (1=normal, <~0.7=quiet, >1.3=loud).\nREAD-ONLY."}, //
                                                   {"q1", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q2", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q3", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q4", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q5", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q6", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q7", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q8", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q9", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q10", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q11", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q12", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q13", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q14", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q15", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q16", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q17", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q18", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q19", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q20", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q21", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q22", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q23", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q24", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q25", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q26", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q27", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q28", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q29", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q30", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q31", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"q32", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                   {"t1", "t1 to t8 values are carried from each wave init to per-frame to per-point.\nWritable."}, //
                                                   {"t2", "t1 to t8 values are carried from each wave init to per-frame to per-point.\nWritable."}, //
                                                   {"t3", "t1 to t8 values are carried from each wave init to per-frame to per-point.\nWritable."}, //
                                                   {"t4", "t1 to t8 values are carried from each wave init to per-frame to per-point.\nWritable."}, //
                                                   {"t5", "t1 to t8 values are carried from each wave init to per-frame to per-point.\nWritable."}, //
                                                   {"t6", "t1 to t8 values are carried from each wave init to per-frame to per-point.\nWritable."}, //
                                                   {"t7", "t1 to t8 values are carried from each wave init to per-frame to per-point.\nWritable."}, //
                                                   {"t8", "t1 to t8 values are carried from each wave init to per-frame to per-point.\nWritable."}}},

        {ExpressionCodeTypes::CustomShapeInit, {{"num_inst", "The total # of shape instances (1..1024).\nREAD-ONLY."}, //
                                                {"sides", "The default number of sides that make up the polygonal shape (3..100).\nWritable."}, //
                                                {"thick", "If non-zero, the border will be overdrawn 4 times to make it thicker, bolder, and more visible (0/1).\nWritable."}, //
                                                {"additive", "If non-zero, the shape will add color to saturate the image towards white; otherwise, it will replace what's there (0/1).\nWritable."}, //
                                                {"x", "Default X position of the shape (0..1; 0=left side, 1=right side).\nWritable."}, //
                                                {"y", "Default Y position of the shape (0..1; 0=left side, 1=right side).\nWritable."}, //
                                                {"rad", "Default radius of the shape (>=0).\nWritable."}, //
                                                {"ang", "Default rotation angle of the shape (0...2*pi).\nWritable."}, //
                                                {"textured", "If non-zero, the shape will be textured with the image from the previous frame (0/1).\nWritable."}, //
                                                {"tex_zoom", "The portion of the previous frame's image to use with the shape (>0).\nWritable."}, //
                                                {"tex_ang", "The angle at which to rotate the previous frame's image before applying it to the shape (0..2*PI).\nWritable."}, //
                                                {"r", "Default amount of red color toward the center of the shape (0..1).\nWritable."}, //
                                                {"g", "Default amount of green color toward the center of the shape (0..1).\nWritable."}, //
                                                {"b", "Default amount of blue color toward the center of the shape (0..1).\nWritable."}, //
                                                {"a", "Default opacity of the center of the shape (0=transparent, 1=opaque).\nWritable."}, //
                                                {"r2", "Default amount of red color toward the outer edge of the shape (0..1).\nWritable."}, //
                                                {"g2", "Default amount of green color toward the outer edge of the shape (0..1).\nWritable."}, //
                                                {"b2", "Default amount of blue color toward the outer edge of the shape (0..1).\nWritable."}, //
                                                {"a2", "Default opacity of the outer edge of the shape (0=transparent, 1=opaque).\nWritable."}, //
                                                {"border_r", "Default amount of red color in the shape's border (0..1).\nWritable."}, //
                                                {"border_g", "Default amount of green color in the shape's border (0..1).\nWritable."}, //
                                                {"border_b", "Default amount of blue color in the shape's border (0..1).\nWritable."}, //
                                                {"border_a", "Default opacity of the shape's border (0=transparent, 1=opaque).\nWritable."}, //
                                                {"time", "Retrieves the current time, in seconds, since program start.\nREAD-ONLY."}, //
                                                {"fps", "Retrieves the current framerate, in frames per second.\nREAD-ONLY."}, //
                                                {"frame", "Retrieves the number of frames of animation elapsed since the program started.\nREAD-ONLY."}, //
                                                {"progress", "Progress through the current preset.\nREAD-ONLY."}, //
                                                {"bass", "Retrieves the current amount of bass (1=normal, <~0.7=quiet, >1.3=loud).\nREAD-ONLY."}, //
                                                {"mid", "Retrieves the current amount of middles (1=normal, <~0.7=quiet, >1.3=loud).\nREAD-ONLY."}, //
                                                {"treb", "Retrieves the current amount of treble (1=normal, <~0.7=quiet, >1.3=loud).\nREAD-ONLY."}, //
                                                {"bass_att", "Retrieves the attenuated amount of bass (1=normal, <~0.7=quiet, >1.3=loud).\nREAD-ONLY."}, //
                                                {"mid_att", "Retrieves the attenuated amount of middles (1=normal, <~0.7=quiet, >1.3=loud).\nREAD-ONLY."}, //
                                                {"treb_att", "Retrieves the attenuated amount of treble (1=normal, <~0.7=quiet, >1.3=loud).\nREAD-ONLY."}, //
                                                {"q1", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                {"q2", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                {"q3", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                {"q4", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                {"q5", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                {"q6", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                {"q7", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                {"q8", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                {"q9", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                {"q10", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                {"q11", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                {"q12", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                {"q13", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                {"q14", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                {"q15", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                {"q16", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                {"q17", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                {"q18", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                {"q19", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                {"q20", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                {"q21", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                {"q22", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                {"q23", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                {"q24", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                {"q25", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                {"q26", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                {"q27", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                {"q28", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                {"q29", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                {"q30", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                {"q31", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                {"q32", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                {"t1", "t1 to t8 values are carried from each shape init to per-frame.\nWritable."}, //
                                                {"t2", "t1 to t8 values are carried from each shape init to per-frame.\nWritable."}, //
                                                {"t3", "t1 to t8 values are carried from each shape init to per-frame.\nWritable."}, //
                                                {"t4", "t1 to t8 values are carried from each shape init to per-frame.\nWritable."}, //
                                                {"t5", "t1 to t8 values are carried from each shape init to per-frame.\nWritable."}, //
                                                {"t6", "t1 to t8 values are carried from each shape init to per-frame.\nWritable."}, //
                                                {"t7", "t1 to t8 values are carried from each shape init to per-frame.\nWritable."}, //
                                                {"t8", "t1 to t8 values are carried from each shape init to per-frame.\nWritable."}}},

        {ExpressionCodeTypes::CustomShapePerFrame, {{"num_inst", "The total # of shape instances (1..1024).\nREAD-ONLY."}, //
                                                    {"instance", "The current instance number that this code is being executed for (0..num_inst-1).\nREAD-ONLY."}, //
                                                    {"sides", "The default number of sides that make up the polygonal shape (3..100).\nWritable."}, //
                                                    {"thick", "If non-zero, the border will be overdrawn 4 times to make it thicker, bolder, and more visible (0/1).\nWritable."}, //
                                                    {"additive", "If non-zero, the shape will add color to saturate the image towards white; otherwise, it will replace what's there (0/1).\nWritable."}, //
                                                    {"x", "Default X position of the shape (0..1; 0=left side, 1=right side).\nWritable."}, //
                                                    {"y", "Default Y position of the shape (0..1; 0=left side, 1=right side).\nWritable."}, //
                                                    {"rad", "Default radius of the shape (>=0).\nWritable."}, //
                                                    {"ang", "Default rotation angle of the shape (0...2*pi).\nWritable."}, //
                                                    {"textured", "If non-zero, the shape will be textured with the image from the previous frame (0/1).\nWritable."}, //
                                                    {"tex_zoom", "The portion of the previous frame's image to use with the shape (>0).\nWritable."}, //
                                                    {"tex_ang", "The angle at which to rotate the previous frame's image before applying it to the shape (0..2*PI).\nWritable."}, //
                                                    {"r", "Default amount of red color toward the center of the shape (0..1).\nWritable."}, //
                                                    {"g", "Default amount of green color toward the center of the shape (0..1).\nWritable."}, //
                                                    {"b", "Default amount of blue color toward the center of the shape (0..1).\nWritable."}, //
                                                    {"a", "Default opacity of the center of the shape (0=transparent, 1=opaque).\nWritable."}, //
                                                    {"r2", "Default amount of red color toward the outer edge of the shape (0..1).\nWritable."}, //
                                                    {"g2", "Default amount of green color toward the outer edge of the shape (0..1).\nWritable."}, //
                                                    {"b2", "Default amount of blue color toward the outer edge of the shape (0..1).\nWritable."}, //
                                                    {"a2", "Default opacity of the outer edge of the shape (0=transparent, 1=opaque).\nWritable."}, //
                                                    {"border_r", "Default amount of red color in the shape's border (0..1).\nWritable."}, //
                                                    {"border_g", "Default amount of green color in the shape's border (0..1).\nWritable."}, //
                                                    {"border_b", "Default amount of blue color in the shape's border (0..1).\nWritable."}, //
                                                    {"border_a", "Default opacity of the shape's border (0=transparent, 1=opaque).\nWritable."}, //
                                                    {"time", "Retrieves the current time, in seconds, since program start.\nREAD-ONLY."}, //
                                                    {"fps", "Retrieves the current framerate, in frames per second.\nREAD-ONLY."}, //
                                                    {"frame", "Retrieves the number of frames of animation elapsed since the program started.\nREAD-ONLY."}, //
                                                    {"progress", "Progress through the current preset.\nREAD-ONLY."}, //
                                                    {"bass", "Retrieves the current amount of bass (1=normal, <~0.7=quiet, >1.3=loud).\nREAD-ONLY."}, //
                                                    {"mid", "Retrieves the current amount of middles (1=normal, <~0.7=quiet, >1.3=loud).\nREAD-ONLY."}, //
                                                    {"treb", "Retrieves the current amount of treble (1=normal, <~0.7=quiet, >1.3=loud).\nREAD-ONLY."}, //
                                                    {"bass_att", "Retrieves the attenuated amount of bass (1=normal, <~0.7=quiet, >1.3=loud).\nREAD-ONLY."}, //
                                                    {"mid_att", "Retrieves the attenuated amount of middles (1=normal, <~0.7=quiet, >1.3=loud).\nREAD-ONLY."}, //
                                                    {"treb_att", "Retrieves the attenuated amount of treble (1=normal, <~0.7=quiet, >1.3=loud).\nREAD-ONLY."}, //
                                                    {"q1", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                    {"q2", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                    {"q3", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                    {"q4", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                    {"q5", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                    {"q6", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                    {"q7", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                    {"q8", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                    {"q9", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                    {"q10", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                    {"q11", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                    {"q12", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                    {"q13", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                    {"q14", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                    {"q15", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                    {"q16", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                    {"q17", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                    {"q18", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                    {"q19", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                    {"q20", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                    {"q21", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                    {"q22", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                    {"q23", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                    {"q24", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                    {"q25", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                    {"q26", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                    {"q27", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                    {"q28", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                    {"q29", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                    {"q30", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                    {"q31", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                    {"q32", "q1 to q32 values are carried along a chain to each code block.\nWritable."}, //
                                                    {"t1", "t1 to t8 values are carried from each shape init to per-frame.\nWritable."}, //
                                                    {"t2", "t1 to t8 values are carried from each shape init to per-frame.\nWritable."}, //
                                                    {"t3", "t1 to t8 values are carried from each shape init to per-frame.\nWritable."}, //
                                                    {"t4", "t1 to t8 values are carried from each shape init to per-frame.\nWritable."}, //
                                                    {"t5", "t1 to t8 values are carried from each shape init to per-frame.\nWritable."}, //
                                                    {"t6", "t1 to t8 values are carried from each shape init to per-frame.\nWritable."}, //
                                                    {"t7", "t1 to t8 values are carried from each shape init to per-frame.\nWritable."}, //
                                                    {"t8", "t1 to t8 values are carried from each shape init to per-frame.\nWritable."}}},

        {ExpressionCodeTypes::WarpShader, {{"shader_body", ICON_FA_PAPER_PLANE " Shader entry point.\nWill be replaced with the appropriate function declaration at runtime."}, //
                                           {"ret", ICON_FA_CUBE " float3\n" ICON_FA_RIGHT_FROM_BRACKET " Shader output RGB color."}, //
                                           {"uv", ICON_FA_CUBE " float2\nWarped UV coordinates (approx. 0..1)."}, //
                                           {"uv_orig", ICON_FA_CUBE " float2\nOriginal, unwarped UV coordinates (0..1)."}, //
                                           {"rad", ICON_FA_CUBE " float\nRadius of the current pixel from center of screen (0..1)."}, //
                                           {"ang", ICON_FA_CUBE " float\nAngle of the current pixel from center of screen (0..2*PI)."}, //
                                           {"rand_preset", ICON_FA_CUBE " float4\n4 random floats [0..1], updated once per preset."}, //
                                           {"rand_frame", ICON_FA_CUBE " float4\n4 random floats [0..1], updated each frame."}, //
                                           {"time", ICON_FA_CUBE " float\nThe time, in seconds, starting at zero when the *preset* starts."}, //
                                           {"fps", ICON_FA_CUBE " float\nThe current framerate (frames per second)."}, //
                                           {"frame", ICON_FA_CUBE " float\nThe current frame #."}, //
                                           {"progress", ICON_FA_CUBE " float\nThe progress through the current preset (0..1)."}, //
                                           {"bass", ICON_FA_CUBE " float\nThe current amount of bass (1=normal, <~0.7=quiet, >1.3=loud)."}, //
                                           {"mid", ICON_FA_CUBE " float\nThe current amount of middles (1=normal, <~0.7=quiet, >1.3=loud)."}, //
                                           {"treb", ICON_FA_CUBE " float\nThe current amount of treble (1=normal, <~0.7=quiet, >1.3=loud)."}, //
                                           {"vol", ICON_FA_CUBE " float\nThe current volume of all frequencies (1=normal, <~0.7=quiet, >1.3=loud)."}, //
                                           {"bass_att", ICON_FA_CUBE " float\nThe attenuated amount of bass (1=normal, <~0.7=quiet, >1.3=loud)."}, //
                                           {"mid_att", ICON_FA_CUBE " float\nThe attenuated amount of middles (1=normal, <~0.7=quiet, >1.3=loud)."}, //
                                           {"treb_att", ICON_FA_CUBE " float\nThe attenuated amount of treble (1=normal, <~0.7=quiet, >1.3=loud)."}, //
                                           {"vol_att", ICON_FA_CUBE " float\nThe attenuated volume of all frequencies (1=normal, <~0.7=quiet, >1.3=loud)."}, //
                                           {"aspect", ICON_FA_CUBE " float4\n.xy: Multiplier to use on UV's to paste an image fullscreen, *aspect-aware*\n.zw: Inverse (1/x) values as in .xy."}, //
                                           {"texsize", ICON_FA_CUBE " float4\nInfo about the size of the drawing canvas, in pixels.\n.xy: (width,height)\n.zw: (1/(float)w, 1/(float)h)"}, //
                                           {"slow_roam_cos", ICON_FA_CUBE " float4\nFour values that slowly roam around in the [0..1] range at varying speeds.\n.xyzw: 0.5 + 0.5*cos(time * float4(~0.005, ~0.008, ~0.013, ~0.022))"}, //
                                           {"roam_cos", ICON_FA_CUBE " float4\nFour values that roam around in the [0..1] range.\n.xyzw: 0.5 + 0.5*cos(time * float4(~0.3, ~1.3, ~5, ~20))"}, //
                                           {"slow_roam_sin", ICON_FA_CUBE " float4\nFour values that slowly roam around in the [0..1] range at varying speeds.\n.xyzw: 0.5 + 0.5*sin(time * float4(~0.005, ~0.008, ~0.013, ~0.022))"}, //
                                           {"roam_sin", ICON_FA_CUBE " float4\nFour values that roam around in the [0..1] range at varying speeds.\n.xyzw: 0.5 + 0.5*sin(time * float4(~0.3, ~1.3, ~5, ~20))"}, //
                                           {"q1", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                           {"q2", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                           {"q3", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                           {"q4", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                           {"q5", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                           {"q6", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                           {"q7", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                           {"q8", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                           {"q9", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                           {"q10", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                           {"q11", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                           {"q12", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                           {"q13", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                           {"q14", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                           {"q15", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                           {"q16", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                           {"q17", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                           {"q18", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                           {"q19", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                           {"q20", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                           {"q21", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                           {"q22", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                           {"q23", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                           {"q24", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                           {"q25", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                           {"q26", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                           {"q27", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                           {"q28", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                           {"q29", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                           {"q30", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                           {"q31", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                           {"q32", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                           {"_qa", ICON_FA_CUBE " float4\nA 4-component vector of the q1-q4 variables. Can be swizzled."}, //
                                           {"_qb", ICON_FA_CUBE " float4\nA 4-component vector of the q5-q8 variables. Can be swizzled."}, //
                                           {"_qc", ICON_FA_CUBE " float4\nA 4-component vector of the q9-q12 variables. Can be swizzled."}, //
                                           {"_qd", ICON_FA_CUBE " float4\nA 4-component vector of the q13-q46 variables. Can be swizzled."}, //
                                           {"_qe", ICON_FA_CUBE " float4\nA 4-component vector of the q17-q20 variables. Can be swizzled."}, //
                                           {"_qf", ICON_FA_CUBE " float4\nA 4-component vector of the q21-q24 variables. Can be swizzled."}, //
                                           {"_qg", ICON_FA_CUBE " float4\nA 4-component vector of the q25-q28 variables. Can be swizzled."}, //
                                           {"_qh", ICON_FA_CUBE " float4\nA 4-component vector of the q29-q32 variables. Can be swizzled."}, //
                                           {"blur1_min", ICON_FA_CUBE " float\nBlur level 1 range minimum (0..1)."}, //
                                           {"blur1_max", ICON_FA_CUBE " float\nBlur level 1 range maximum (0..1)."}, //
                                           {"blur2_min", ICON_FA_CUBE " float\nBlur level 2 range minimum (0..1)."}, //
                                           {"blur2_max", ICON_FA_CUBE " float\nBlur level 2 range maximum (0..1)."}, //
                                           {"blur3_min", ICON_FA_CUBE " float\nBlur level 3 range minimum (0..1)."}, //
                                           {"blur3_max", ICON_FA_CUBE " float\nBlur level 3 range maximum (0..1)."}, //
                                           {"rot_s1", ICON_FA_CUBE " float4x3\nFour random, static rotations, randomized at preset load time.\nMinor translation component (<1)."}, //
                                           {"rot_s2", ICON_FA_CUBE " float4x3\nFour random, static rotations, randomized at preset load time.\nMinor translation component (<1)."}, //
                                           {"rot_s3", ICON_FA_CUBE " float4x3\nFour random, static rotations, randomized at preset load time.\nMinor translation component (<1)."}, //
                                           {"rot_s4", ICON_FA_CUBE " float4x3\nFour random, static rotations, randomized at preset load time.\nMinor translation component (<1)."}, //
                                           {"rot_d1", ICON_FA_CUBE " float4x3\nFour random, slowly changing rotations."}, //
                                           {"rot_d2", ICON_FA_CUBE " float4x3\nFour random, slowly changing rotations."}, //
                                           {"rot_d3", ICON_FA_CUBE " float4x3\nFour random, slowly changing rotations."}, //
                                           {"rot_d4", ICON_FA_CUBE " float4x3\nFour random, slowly changing rotations."}, //
                                           {"rot_f1", ICON_FA_CUBE " float4x3\nFour random, faster-changing rotations."}, //
                                           {"rot_f2", ICON_FA_CUBE " float4x3\nFour random, faster-changing rotations."}, //
                                           {"rot_f3", ICON_FA_CUBE " float4x3\nFour random, faster-changing rotations."}, //
                                           {"rot_f4", ICON_FA_CUBE " float4x3\nFour random, faster-changing rotations."}, //
                                           {"rot_vf1", ICON_FA_CUBE " float4x3\nFour random, very-fast-changing rotations."}, //
                                           {"rot_vf2", ICON_FA_CUBE " float4x3\nFour random, very-fast-changing rotations."}, //
                                           {"rot_vf3", ICON_FA_CUBE " float4x3\nFour random, very-fast-changing rotations."}, //
                                           {"rot_vf4", ICON_FA_CUBE " float4x3\nFour random, very-fast-changing rotations."}, //
                                           {"rot_uf1", ICON_FA_CUBE " float4x3\nFour random, ultra-fast-changing rotations."}, //
                                           {"rot_uf2", ICON_FA_CUBE " float4x3\nFour random, ultra-fast-changing rotations."}, //
                                           {"rot_uf3", ICON_FA_CUBE " float4x3\nFour random, ultra-fast-changing rotations."}, //
                                           {"rot_uf4", ICON_FA_CUBE " float4x3\nFour random, ultra-fast-changing rotations."}, //
                                           {"rot_rand1", ICON_FA_CUBE " float4x3\nRandom rotation on every frame."}, //
                                           {"rot_rand2", ICON_FA_CUBE " float4x3\nRandom rotation on every frame."}, //
                                           {"rot_rand3", ICON_FA_CUBE " float4x3\nRandom rotation on every frame."}, //
                                           {"rot_rand4", ICON_FA_CUBE " float4x3\nRandom rotation on every frame."}, //
                                           {"sampler_main", ICON_FA_CUBE " sampler\nMain preset image texture sampler.\nBilinear filtering, wraps around."}, //
                                           {"sampler_fw_main", ICON_FA_CUBE " sampler\nMain preset image texture sampler.\nBilinear filtering, wraps around."}, //
                                           {"sampler_fc_main", ICON_FA_CUBE " sampler\nMain preset image texture sampler.\nBilinear filtering, clamps to edge."}, //
                                           {"sampler_pw_main", ICON_FA_CUBE " sampler\nMain preset image texture sampler.\nPoint sampling, wraps around."}, //
                                           {"sampler_pc_main", ICON_FA_CUBE " sampler\nMain preset image texture sampler.\nPoint sampling, clamps to edge."}, //
                                           {"sampler_noise_lq", ICON_FA_CUBE " sampler\n256x256 2D 4-channel low-quality noise texture.\nBilinear filtering, wraps around."}, //
                                           {"sampler_fw_noise_lq", ICON_FA_CUBE " sampler\n256x256 2D 4-channel low-quality noise texture.\nBilinear filtering, wraps around."}, //
                                           {"sampler_fc_noise_lq", ICON_FA_CUBE " sampler\n256x256 2D 4-channel low-quality noise texture.\nBilinear filtering, clamps to edge."}, //
                                           {"sampler_pw_noise_lq", ICON_FA_CUBE " sampler\n256x256 2D 4-channel low-quality noise texture.\nPoint sampling, wraps around."}, //
                                           {"sampler_pc_noise_lq", ICON_FA_CUBE " sampler\n256x256 2D 4-channel low-quality noise texture.\nPoint sampling, clamps to edge."}, //
                                           {"sampler_noise_lq_lite", ICON_FA_CUBE " sampler\n32x32 2D 4-channel low-quality noise texture.\nBilinear filtering, wraps around."}, //
                                           {"sampler_fw_noise_lq_lite", ICON_FA_CUBE " sampler\n32x32 2D 4-channel low-quality noise texture.\nBilinear filtering, wraps around."}, //
                                           {"sampler_fc_noise_lq_lite", ICON_FA_CUBE " sampler\n32x32 2D 4-channel low-quality noise texture.\nBilinear filtering, clamps to edge."}, //
                                           {"sampler_pw_noise_lq_lite", ICON_FA_CUBE " sampler\n32x32 2D 4-channel low-quality noise texture.\nPoint sampling, wraps around."}, //
                                           {"sampler_pc_noise_lq_lite", ICON_FA_CUBE " sampler\n32x32 2D 4-channel low-quality noise texture.\nPoint sampling, clamps to edge."}, //
                                           {"sampler_noise_mq", ICON_FA_CUBE " sampler\n64x64 2D 4-channel medium-quality noise texture.\nBilinear filtering, wraps around."}, //
                                           {"sampler_fw_noise_mq", ICON_FA_CUBE " sampler\n64x64 2D 4-channel medium-quality noise texture.\nBilinear filtering, wraps around."}, //
                                           {"sampler_fc_noise_mq", ICON_FA_CUBE " sampler\n64x64 2D 4-channel medium-quality noise texture.\nBilinear filtering, clamps to edge."}, //
                                           {"sampler_pw_noise_mq", ICON_FA_CUBE " sampler\n64x64 2D 4-channel medium-quality noise texture.\nPoint sampling, wraps around."}, //
                                           {"sampler_pc_noise_mq", ICON_FA_CUBE " sampler\n64x64 2D 4-channel medium-quality noise texture.\nPoint sampling, clamps to edge."}, //
                                           {"sampler_noise_hq", ICON_FA_CUBE " sampler\n32x32 2D 4-channel high-quality noise texture.\nBilinear filtering, wraps around."}, //
                                           {"sampler_fw_noise_hq", ICON_FA_CUBE " sampler\n32x32 2D 4-channel high-quality noise texture.\nBilinear filtering, wraps around."}, //
                                           {"sampler_fc_noise_hq", ICON_FA_CUBE " sampler\n32x32 2D 4-channel high-quality noise texture.\nBilinear filtering, clamps to edge."}, //
                                           {"sampler_pw_noise_hq", ICON_FA_CUBE " sampler\n32x32 2D 4-channel high-quality noise texture.\nPoint sampling, wraps around."}, //
                                           {"sampler_pc_noise_hq", ICON_FA_CUBE " sampler\n32x32 2D 4-channel high-quality noise texture.\nPoint sampling, clamps to edge."}, //
                                           {"sampler_noisevol_lq", ICON_FA_CUBE " sampler\n32x32x32 3D 4-channel low-quality volumetric noise texture.\nBilinear filtering, wraps around."}, //
                                           {"sampler_fw_noisevol_lq", ICON_FA_CUBE " sampler\n32x32x32 3D 4-channel low-quality volumetric noise texture.\nBilinear filtering, wraps around."}, //
                                           {"sampler_fc_noisevol_lq", ICON_FA_CUBE " sampler\n32x32x32 3D 4-channel low-quality volumetric noise texture.\nBilinear filtering, clamps to edge."}, //
                                           {"sampler_pw_noisevol_lq", ICON_FA_CUBE " sampler\n32x32x32 3D 4-channel low-quality volumetric noise texture.\nPoint sampling, wraps around."}, //
                                           {"sampler_pc_noisevol_lq", ICON_FA_CUBE " sampler\n32x32x32 3D 4-channel low-quality volumetric noise texture.\nPoint sampling, clamps to edge."}, //
                                           {"sampler_noisevol_hq", ICON_FA_CUBE " sampler\n8x8x8 3D 4-channel high-quality volumetric noise texture.\nBilinear filtering, wraps around."}, //
                                           {"sampler_fw_noisevol_hq", ICON_FA_CUBE " sampler\n8x8x8 3D 4-channel high-quality volumetric noise texture.\nBilinear filtering, wraps around."}, //
                                           {"sampler_fc_noisevol_hq", ICON_FA_CUBE " sampler\n8x8x8 3D 4-channel high-quality volumetric noise texture.\nBilinear filtering, clamps to edge."}, //
                                           {"sampler_pw_noisevol_hq", ICON_FA_CUBE " sampler\n8x8x8 3D 4-channel high-quality volumetric noise texture.\nPoint sampling, wraps around."}, //
                                           {"sampler_pc_noisevol_hq", ICON_FA_CUBE " sampler\n8x8x8 3D 4-channel high-quality volumetric noise texture.\nPoint sampling, clamps to edge."}}},

        {ExpressionCodeTypes::CompositeShader, {{"shader_body", ICON_FA_PAPER_PLANE " Shader entry point.\nWill be replaced with the appropriate function declaration at runtime."}, //
                                                {"ret", ICON_FA_CUBE " float3\n" ICON_FA_RIGHT_FROM_BRACKET " Shader output RGB color."}, //
                                                {"uv", ICON_FA_CUBE " float2\nUnwarped UV coordinates (0..1)."}, //
                                                {"hue_shader", ICON_FA_CUBE " float3\nA color that varies across the screen (the old 'hue shader' effect from MilkDrop 1)."}, //
                                                {"rad", ICON_FA_CUBE " float\nRadius of the current pixel from center of screen (0..1)."}, //
                                                {"ang", ICON_FA_CUBE " float\nAngle of the current pixel from center of screen (0..2*PI)."}, //
                                                {"rand_preset", ICON_FA_CUBE " float4\n4 random floats [0..1], updated once per preset."}, //
                                                {"rand_frame", ICON_FA_CUBE " float4\n4 random floats [0..1], updated each frame."}, //
                                                {"time", ICON_FA_CUBE " float\nThe time, in seconds, starting at zero when the *preset* starts."}, //
                                                {"fps", ICON_FA_CUBE " float\nThe current framerate (frames per second)."}, //
                                                {"frame", ICON_FA_CUBE " float\nThe current frame #."}, //
                                                {"progress", ICON_FA_CUBE " float\nThe progress through the current preset (0..1)."}, //
                                                {"bass", ICON_FA_CUBE " float\nThe current amount of bass (1=normal, <~0.7=quiet, >1.3=loud)."}, //
                                                {"mid", ICON_FA_CUBE " float\nThe current amount of middles (1=normal, <~0.7=quiet, >1.3=loud)."}, //
                                                {"treb", ICON_FA_CUBE " float\nThe current amount of treble (1=normal, <~0.7=quiet, >1.3=loud)."}, //
                                                {"vol", ICON_FA_CUBE " float\nThe current volume of all frequencies (1=normal, <~0.7=quiet, >1.3=loud)."}, //
                                                {"bass_att", ICON_FA_CUBE " float\nThe attenuated amount of bass (1=normal, <~0.7=quiet, >1.3=loud)."}, //
                                                {"mid_att", ICON_FA_CUBE " float\nThe attenuated amount of middles (1=normal, <~0.7=quiet, >1.3=loud)."}, //
                                                {"treb_att", ICON_FA_CUBE " float\nThe attenuated amount of treble (1=normal, <~0.7=quiet, >1.3=loud)."}, //
                                                {"vol_att", ICON_FA_CUBE " float\nThe attenuated volume of all frequencies (1=normal, <~0.7=quiet, >1.3=loud)."}, //
                                                {"aspect", ICON_FA_CUBE " float4\n.xy: Multiplier to use on UV's to paste an image fullscreen, *aspect-aware*\n.zw: Inverse (1/x) values as in .xy."}, //
                                                {"texsize", ICON_FA_CUBE " float4\nInfo about the size of the drawing canvas, in pixels.\n.xy: (width,height)\n.zw: (1/(float)w, 1/(float)h)"}, //
                                                {"slow_roam_cos", ICON_FA_CUBE " float4\nFour values that slowly roam around in the [0..1] range at varying speeds.\n.xyzw: 0.5 + 0.5*cos(time * float4(~0.005, ~0.008, ~0.013, ~0.022))"}, //
                                                {"roam_cos", ICON_FA_CUBE " float4\nFour values that roam around in the [0..1] range.\n.xyzw: 0.5 + 0.5*cos(time * float4(~0.3, ~1.3, ~5, ~20))"}, //
                                                {"slow_roam_sin", ICON_FA_CUBE " float4\nFour values that slowly roam around in the [0..1] range at varying speeds.\n.xyzw: 0.5 + 0.5*sin(time * float4(~0.005, ~0.008, ~0.013, ~0.022))"}, //
                                                {"roam_sin", ICON_FA_CUBE " float4\nFour values that roam around in the [0..1] range at varying speeds.\n.xyzw: 0.5 + 0.5*sin(time * float4(~0.3, ~1.3, ~5, ~20))"}, //
                                                {"q1", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                                {"q2", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                                {"q3", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                                {"q4", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                                {"q5", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                                {"q6", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                                {"q7", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                                {"q8", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                                {"q9", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                                {"q10", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                                {"q11", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                                {"q12", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                                {"q13", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                                {"q14", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                                {"q15", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                                {"q16", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                                {"q17", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                                {"q18", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                                {"q19", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                                {"q20", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                                {"q21", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                                {"q22", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                                {"q23", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                                {"q24", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                                {"q25", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                                {"q26", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                                {"q27", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                                {"q28", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                                {"q29", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                                {"q30", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                                {"q31", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                                {"q32", ICON_FA_CUBE " float\nq1 to q32 values are copied from the per-frame code into the shader."}, //
                                                {"_qa", ICON_FA_CUBE " float4\nA 4-component vector of the q1-q4 variables. Can be swizzled."}, //
                                                {"_qb", ICON_FA_CUBE " float4\nA 4-component vector of the q5-q8 variables. Can be swizzled."}, //
                                                {"_qc", ICON_FA_CUBE " float4\nA 4-component vector of the q9-q12 variables. Can be swizzled."}, //
                                                {"_qd", ICON_FA_CUBE " float4\nA 4-component vector of the q13-q46 variables. Can be swizzled."}, //
                                                {"_qe", ICON_FA_CUBE " float4\nA 4-component vector of the q17-q20 variables. Can be swizzled."}, //
                                                {"_qf", ICON_FA_CUBE " float4\nA 4-component vector of the q21-q24 variables. Can be swizzled."}, //
                                                {"_qg", ICON_FA_CUBE " float4\nA 4-component vector of the q25-q28 variables. Can be swizzled."}, //
                                                {"_qh", ICON_FA_CUBE " float4\nA 4-component vector of the q29-q32 variables. Can be swizzled."}, //
                                                {"blur1_min", ICON_FA_CUBE " float\nBlur level 1 range minimum (0..1)."}, //
                                                {"blur1_max", ICON_FA_CUBE " float\nBlur level 1 range maximum (0..1)."}, //
                                                {"blur2_min", ICON_FA_CUBE " float\nBlur level 2 range minimum (0..1)."}, //
                                                {"blur2_max", ICON_FA_CUBE " float\nBlur level 2 range maximum (0..1)."}, //
                                                {"blur3_min", ICON_FA_CUBE " float\nBlur level 3 range minimum (0..1)."}, //
                                                {"blur3_max", ICON_FA_CUBE " float\nBlur level 3 range maximum (0..1)."}, //
                                                {"rot_s1", ICON_FA_CUBE " float4x3\nFour random, static rotations, randomized at preset load time.\nMinor translation component (<1)."}, //
                                                {"rot_s2", ICON_FA_CUBE " float4x3\nFour random, static rotations, randomized at preset load time.\nMinor translation component (<1)."}, //
                                                {"rot_s3", ICON_FA_CUBE " float4x3\nFour random, static rotations, randomized at preset load time.\nMinor translation component (<1)."}, //
                                                {"rot_s4", ICON_FA_CUBE " float4x3\nFour random, static rotations, randomized at preset load time.\nMinor translation component (<1)."}, //
                                                {"rot_d1", ICON_FA_CUBE " float4x3\nFour random, slowly changing rotations."}, //
                                                {"rot_d2", ICON_FA_CUBE " float4x3\nFour random, slowly changing rotations."}, //
                                                {"rot_d3", ICON_FA_CUBE " float4x3\nFour random, slowly changing rotations."}, //
                                                {"rot_d4", ICON_FA_CUBE " float4x3\nFour random, slowly changing rotations."}, //
                                                {"rot_f1", ICON_FA_CUBE " float4x3\nFour random, faster-changing rotations."}, //
                                                {"rot_f2", ICON_FA_CUBE " float4x3\nFour random, faster-changing rotations."}, //
                                                {"rot_f3", ICON_FA_CUBE " float4x3\nFour random, faster-changing rotations."}, //
                                                {"rot_f4", ICON_FA_CUBE " float4x3\nFour random, faster-changing rotations."}, //
                                                {"rot_vf1", ICON_FA_CUBE " float4x3\nFour random, very-fast-changing rotations."}, //
                                                {"rot_vf2", ICON_FA_CUBE " float4x3\nFour random, very-fast-changing rotations."}, //
                                                {"rot_vf3", ICON_FA_CUBE " float4x3\nFour random, very-fast-changing rotations."}, //
                                                {"rot_vf4", ICON_FA_CUBE " float4x3\nFour random, very-fast-changing rotations."}, //
                                                {"rot_uf1", ICON_FA_CUBE " float4x3\nFour random, ultra-fast-changing rotations."}, //
                                                {"rot_uf2", ICON_FA_CUBE " float4x3\nFour random, ultra-fast-changing rotations."}, //
                                                {"rot_uf3", ICON_FA_CUBE " float4x3\nFour random, ultra-fast-changing rotations."}, //
                                                {"rot_uf4", ICON_FA_CUBE " float4x3\nFour random, ultra-fast-changing rotations."}, //
                                                {"rot_rand1", ICON_FA_CUBE " float4x3\nRandom rotation on every frame."}, //
                                                {"rot_rand2", ICON_FA_CUBE " float4x3\nRandom rotation on every frame."}, //
                                                {"rot_rand3", ICON_FA_CUBE " float4x3\nRandom rotation on every frame."}, //
                                                {"rot_rand4", ICON_FA_CUBE " float4x3\nRandom rotation on every frame."}, //
                                                {"sampler_main", ICON_FA_CUBE " sampler\nMain preset image texture sampler.\nBilinear filtering, wraps around."}, //
                                                {"sampler_fw_main", ICON_FA_CUBE " sampler\nMain preset image texture sampler.\nBilinear filtering, wraps around."}, //
                                                {"sampler_fc_main", ICON_FA_CUBE " sampler\nMain preset image texture sampler.\nBilinear filtering, clamps to edge."}, //
                                                {"sampler_pw_main", ICON_FA_CUBE " sampler\nMain preset image texture sampler.\nPoint sampling, wraps around."}, //
                                                {"sampler_pc_main", ICON_FA_CUBE " sampler\nMain preset image texture sampler.\nPoint sampling, clamps to edge."}, //
                                                {"sampler_noise_lq", ICON_FA_CUBE " sampler\n256x256 2D 4-channel low-quality noise texture.\nBilinear filtering, wraps around."}, //
                                                {"sampler_fw_noise_lq", ICON_FA_CUBE " sampler\n256x256 2D 4-channel low-quality noise texture.\nBilinear filtering, wraps around."}, //
                                                {"sampler_fc_noise_lq", ICON_FA_CUBE " sampler\n256x256 2D 4-channel low-quality noise texture.\nBilinear filtering, clamps to edge."}, //
                                                {"sampler_pw_noise_lq", ICON_FA_CUBE " sampler\n256x256 2D 4-channel low-quality noise texture.\nPoint sampling, wraps around."}, //
                                                {"sampler_pc_noise_lq", ICON_FA_CUBE " sampler\n256x256 2D 4-channel low-quality noise texture.\nPoint sampling, clamps to edge."}, //
                                                {"sampler_noise_lq_lite", ICON_FA_CUBE " sampler\n32x32 2D 4-channel low-quality noise texture.\nBilinear filtering, wraps around."}, //
                                                {"sampler_fw_noise_lq_lite", ICON_FA_CUBE " sampler\n32x32 2D 4-channel low-quality noise texture.\nBilinear filtering, wraps around."}, //
                                                {"sampler_fc_noise_lq_lite", ICON_FA_CUBE " sampler\n32x32 2D 4-channel low-quality noise texture.\nBilinear filtering, clamps to edge."}, //
                                                {"sampler_pw_noise_lq_lite", ICON_FA_CUBE " sampler\n32x32 2D 4-channel low-quality noise texture.\nPoint sampling, wraps around."}, //
                                                {"sampler_pc_noise_lq_lite", ICON_FA_CUBE " sampler\n32x32 2D 4-channel low-quality noise texture.\nPoint sampling, clamps to edge."}, //
                                                {"sampler_noise_mq", ICON_FA_CUBE " sampler\n64x64 2D 4-channel medium-quality noise texture.\nBilinear filtering, wraps around."}, //
                                                {"sampler_fw_noise_mq", ICON_FA_CUBE " sampler\n64x64 2D 4-channel medium-quality noise texture.\nBilinear filtering, wraps around."}, //
                                                {"sampler_fc_noise_mq", ICON_FA_CUBE " sampler\n64x64 2D 4-channel medium-quality noise texture.\nBilinear filtering, clamps to edge."}, //
                                                {"sampler_pw_noise_mq", ICON_FA_CUBE " sampler\n64x64 2D 4-channel medium-quality noise texture.\nPoint sampling, wraps around."}, //
                                                {"sampler_pc_noise_mq", ICON_FA_CUBE " sampler\n64x64 2D 4-channel medium-quality noise texture.\nPoint sampling, clamps to edge."}, //
                                                {"sampler_noise_hq", ICON_FA_CUBE " sampler\n32x32 2D 4-channel high-quality noise texture.\nBilinear filtering, wraps around."}, //
                                                {"sampler_fw_noise_hq", ICON_FA_CUBE " sampler\n32x32 2D 4-channel high-quality noise texture.\nBilinear filtering, wraps around."}, //
                                                {"sampler_fc_noise_hq", ICON_FA_CUBE " sampler\n32x32 2D 4-channel high-quality noise texture.\nBilinear filtering, clamps to edge."}, //
                                                {"sampler_pw_noise_hq", ICON_FA_CUBE " sampler\n32x32 2D 4-channel high-quality noise texture.\nPoint sampling, wraps around."}, //
                                                {"sampler_pc_noise_hq", ICON_FA_CUBE " sampler\n32x32 2D 4-channel high-quality noise texture.\nPoint sampling, clamps to edge."}, //
                                                {"sampler_noisevol_lq", ICON_FA_CUBE " sampler\n32x32x32 3D 4-channel low-quality volumetric noise texture.\nBilinear filtering, wraps around."}, //
                                                {"sampler_fw_noisevol_lq", ICON_FA_CUBE " sampler\n32x32x32 3D 4-channel low-quality volumetric noise texture.\nBilinear filtering, wraps around."}, //
                                                {"sampler_fc_noisevol_lq", ICON_FA_CUBE " sampler\n32x32x32 3D 4-channel low-quality volumetric noise texture.\nBilinear filtering, clamps to edge."}, //
                                                {"sampler_pw_noisevol_lq", ICON_FA_CUBE " sampler\n32x32x32 3D 4-channel low-quality volumetric noise texture.\nPoint sampling, wraps around."}, //
                                                {"sampler_pc_noisevol_lq", ICON_FA_CUBE " sampler\n32x32x32 3D 4-channel low-quality volumetric noise texture.\nPoint sampling, clamps to edge."}, //
                                                {"sampler_noisevol_hq", ICON_FA_CUBE " sampler\n8x8x8 3D 4-channel high-quality volumetric noise texture.\nBilinear filtering, wraps around."}, //
                                                {"sampler_fw_noisevol_hq", ICON_FA_CUBE " sampler\n8x8x8 3D 4-channel high-quality volumetric noise texture.\nBilinear filtering, wraps around."}, //
                                                {"sampler_fc_noisevol_hq", ICON_FA_CUBE " sampler\n8x8x8 3D 4-channel high-quality volumetric noise texture.\nBilinear filtering, clamps to edge."}, //
                                                {"sampler_pw_noisevol_hq", ICON_FA_CUBE " sampler\n8x8x8 3D 4-channel high-quality volumetric noise texture.\nPoint sampling, wraps around."}, //
                                                {"sampler_pc_noisevol_hq", ICON_FA_CUBE " sampler\n8x8x8 3D 4-channel high-quality volumetric noise texture.\nPoint sampling, clamps to edge."}}}};

    return identifierList.at(type);
}

const TextEditor::LanguageDefinition& CodeContextInformation::GetLanguageDefinition(ExpressionCodeTypes type)
{
    static std::map<ExpressionCodeTypes, TextEditor::LanguageDefinition> languageDefinitions{
        {ExpressionCodeTypes::PerFrameInit, PopulateLanguageDefinitionForType(ExpressionCodeTypes::PerFrameInit)},
        {ExpressionCodeTypes::PerFrame, PopulateLanguageDefinitionForType(ExpressionCodeTypes::PerFrame)},
        {ExpressionCodeTypes::PerVertex, PopulateLanguageDefinitionForType(ExpressionCodeTypes::PerVertex)},
        {ExpressionCodeTypes::CustomWaveInit, PopulateLanguageDefinitionForType(ExpressionCodeTypes::CustomWaveInit)},
        {ExpressionCodeTypes::CustomWavePerFrame, PopulateLanguageDefinitionForType(ExpressionCodeTypes::CustomWavePerFrame)},
        {ExpressionCodeTypes::CustomWavePerPoint, PopulateLanguageDefinitionForType(ExpressionCodeTypes::CustomWavePerPoint)},
        {ExpressionCodeTypes::CustomShapeInit, PopulateLanguageDefinitionForType(ExpressionCodeTypes::CustomShapeInit)},
        {ExpressionCodeTypes::CustomShapePerFrame, PopulateLanguageDefinitionForType(ExpressionCodeTypes::CustomShapePerFrame)},
        {ExpressionCodeTypes::WarpShader, PopulateLanguageDefinitionForType(ExpressionCodeTypes::WarpShader)},
        {ExpressionCodeTypes::CompositeShader, PopulateLanguageDefinitionForType(ExpressionCodeTypes::CompositeShader)}};

    return languageDefinitions.at(type);
}

TextEditor::LanguageDefinition CodeContextInformation::PopulateLanguageDefinitionForType(ExpressionCodeTypes type)
{
    auto definition = TextEditor::LanguageDefinition::MilkdropExpression();

    if (type == ExpressionCodeTypes::WarpShader || type == ExpressionCodeTypes::CompositeShader)
    {
        definition = TextEditor::LanguageDefinition::HLSL();
    }

    for (const auto& sourceIdentifier : GetIdentifierList(type))
    {
        TextEditor::Identifier id;
        id.mDeclaration = sourceIdentifier.second;
        definition.mIdentifiers.insert(std::make_pair(sourceIdentifier.first, id));
    }

    return definition;
}

} // namespace Editor
