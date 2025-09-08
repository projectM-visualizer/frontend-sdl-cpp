#pragma once

namespace Editor {

/**
 * @brief An enum with the different expression code contexts used in Milkdrop.
 */
enum class ExpressionCodeTypes : int
{
    PerFrameInit,
    PerFrame,
    PerVertex,
    CustomWaveInit,
    CustomWavePerFrame,
    CustomWavePerPoint,
    CustomShapeInit,
    CustomShapePerFrame,
    WarpShader,
    CompositeShader
};

} // namespace Editor
