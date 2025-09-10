#include "PresetEditorGUI.h"

#include "CodeContextInformation.h"
#include "CodeEditorWindow.h"
#include "IconsFontAwesome7.h"

#include "ProjectMSDLApplication.h"
#include "ProjectMWrapper.h"

#include "notifications/DisplayToastNotification.h"
#include "notifications/UpdateWindowTitleNotification.h"

#include <imgui.h>

#include <Poco/NotificationCenter.h>
#include <Poco/Util/Application.h>

#include <sstream>

namespace Editor {

PresetEditorGUI::PresetEditorGUI(ProjectMGUI& gui)
    : _gui(gui)
    , _application(ProjectMSDLApplication::instance())
    , _projectMWrapper(Poco::Util::Application::instance().getSubsystem<ProjectMWrapper>())
    , _menu(*this)
{
}

PresetEditorGUI::~PresetEditorGUI()
{
}

void PresetEditorGUI::Show(const std::string& presetFile)
{
    if (presetFile.empty())
    {
        _presetFile = PresetFile::EmptyPreset();
    }
    else
    {
        _presetFile = PresetFile();
        if (!_presetFile.ReadFile(presetFile))
        {
            Poco::NotificationCenter::defaultCenter().postNotification(new DisplayToastNotification("The selected preset could not be loaded."));
            return;
        }
    }

    TakeProjectMControl();

    // Load the parsed preset as data
    std::string errorMessage;
    if (!_projectMWrapper.LoadPresetData(_presetFile.AsString(), errorMessage))
    {
        Poco::NotificationCenter::defaultCenter().postNotification(new DisplayToastNotification(errorMessage));
        ReleaseProjectMControl();
        return;
    }

    _editorPreset.FromParsedFile(_presetFile);
    _codeEditorWindow = std::make_unique<CodeEditorWindow>();

    _visible = true;
}

void PresetEditorGUI::Close()
{
    _wantClose = true;
}

bool PresetEditorGUI::Draw()
{
    if (!_visible)
    {
        return false;
    }

    HandleGlobalEditorKeys();

    _menu.Draw();

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, IM_COL32(0xd9, 0x1e, 0x18, 0xff));

    if (ImGui::Begin("Preset Editor", &_visible, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground))
    {
        DrawLeftSideBar();

        ImGui::SameLine();

        _codeEditorWindow->Draw();
    }
    ImGui::End();

    ImGui::PopStyleColor();

    if (!_visible || _wantClose)
    {
        // Check for unsaved data

        ReleaseProjectMControl();

        _visible = false;
        _wantClose = false;

        _codeEditorWindow.reset();
    }

    return _visible;
}

void PresetEditorGUI::UpdatePresetPreview()
{
    std::string errorMessage;

    _editorPreset.ToParsedFile(_presetFile);
    if (!_projectMWrapper.LoadPresetData(_presetFile.AsString(), errorMessage))
    {
        Poco::NotificationCenter::defaultCenter().postNotification(new DisplayToastNotification("Preset reload failed:\n" + errorMessage));
    }
}

void PresetEditorGUI::HandleGlobalEditorKeys()
{
    ImGuiIO& io = ImGui::GetIO();
    auto shift = io.KeyShift;
    auto ctrl = io.ConfigMacOSXBehaviors ? io.KeySuper : io.KeyCtrl;
    auto alt = io.ConfigMacOSXBehaviors ? io.KeyCtrl : io.KeyAlt;

    // Reload edited preset - Alt+Enter or F5
    if (ImGui::IsKeyPressed(ImGuiKey_F5, false) || alt && ImGui::IsKeyPressed(ImGuiKey_Enter, false))
    {
        UpdatePresetPreview();
    }

    // Save preset - Ctrl+S
    if (ctrl && ImGui::IsKeyPressed(ImGuiKey_S, false))
    {

    }

    // New preset - Ctrl+N
    if (ctrl && ImGui::IsKeyPressed(ImGuiKey_N, false))
    {
        Show("");
    }
}

void PresetEditorGUI::TakeProjectMControl()
{
    // Detach playlist, seize control over projectM
    _projectMWrapper.UnbindPlaylist();
    _projectMWrapper.EnablePlaybackControl(false);
    _projectMWrapper.HardLockPreset(true);

    Poco::NotificationCenter::defaultCenter().postNotification(new UpdateWindowTitleNotification("projectM Preset Editor"));
}

void PresetEditorGUI::ReleaseProjectMControl()
{
    // Reattach playlist and restore original settings
    _projectMWrapper.BindPlaylist();
    _projectMWrapper.EnablePlaybackControl(true);
    _projectMWrapper.HardLockPreset(false);

    Poco::NotificationCenter::defaultCenter().postNotification(new UpdateWindowTitleNotification());
}

void PresetEditorGUI::EditCode(ExpressionCodeTypes type, std::string& code, int index)
{
    _codeEditorWindow->OpenCodeInTab(type, code, index);
}

unsigned long PresetEditorGUI::GetLoC(const std::string& code)
{
    std::istringstream iss(code);
    unsigned long loc = 0;
    std::string line;
    while (std::getline(iss, line))
    {
        ++loc;
    }

    return loc;
}

void PresetEditorGUI::DrawLeftSideBar()
{
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
    ImGui::SetNextWindowBgAlpha(0.5f);
    ImGui::BeginChild("LeftSideBar", ImVec2(400, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX, window_flags);

    DrawPresetCompatibilitySettings();
    DrawGeneralParameters();
    DrawDefaultWaveformSettings();
    DrawMotionVectorSettings();
    DrawWarpMotionSettings();
    DrawMotionCodeSettings();
    DrawBorderSettings();

    if (ImGui::CollapsingHeader("Custom Waveforms"))
    {
        ImGui::Indent(16.0f);
        for (auto& wave : _editorPreset.waves)
        {
            DrawCustomWaveSettings(wave);
        }
        ImGui::Unindent(16.0f);
    }

    if (ImGui::CollapsingHeader("Custom Shapes"))
    {
        ImGui::Indent(16.0f);
        for (auto& shape : _editorPreset.shapes)
        {
            DrawCustomShapeSettings(shape);
        }
        ImGui::Unindent(16.0f);
    }

    DrawShaderCodeSettings();

    ImGui::EndChild();

    ImGui::PopStyleVar();
}

void PresetEditorGUI::DrawPresetCompatibilitySettings()
{
    if (ImGui::CollapsingHeader("Preset Compatibility"))
    {
        {
            static const char* milkdropVersions[] = {
                "140 - Milkdrop 1.4, No Shaders",
                "200 - Milkdrop 2.0, Same Warp/Comp PS Versions",
                "201 - Milkdrop 2.1+ Separate Warp/Comp PS Versions"};
            int selectionIndex = _editorPreset.presetVersion < 200    ? 0
                                 : _editorPreset.presetVersion == 200 ? 1
                                                                      : 2;
            const char* comboboxPreviewValue = milkdropVersions[selectionIndex];
            if (ImGui::BeginCombo("Preset Version", comboboxPreviewValue))
            {
                for (int index = 0; index < 3; index++)
                {
                    const bool isSelected = (selectionIndex == index);
                    if (ImGui::Selectable(milkdropVersions[index], isSelected))
                    {
                        switch (index)
                        {
                            case 0:
                            default:
                                _editorPreset.presetVersion = 140;
                                _editorPreset.warpShaderVersion = 0;
                                _editorPreset.compositeShaderVersion = 0;
                                break;
                            case 1:
                                _editorPreset.presetVersion = 200;
                                _editorPreset.compositeShaderVersion = _editorPreset.warpShaderVersion;
                                break;
                            case 2:
                                _editorPreset.presetVersion = 201;
                                break;
                        }
                    }

                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }

                ImGui::EndCombo();
            }
            DrawHelpTooltip("Determines the feature set of a preset regarding use of shaders.");
        }

        if (_editorPreset.presetVersion == 200)
        {
            ImGui::TextUnformatted("Pixel Shader Version");
            ImGui::Indent(16.0f);

            if (ImGui::SliderInt("PS Version", &_editorPreset.warpShaderVersion, 0, 4))
            {
                _editorPreset.compositeShaderVersion = _editorPreset.warpShaderVersion;
            }
            DrawHelpTooltip("Minimum required DirectX Pixel Shader version.\n0/1=No PS, 2=PS 2.0, 3=PS 2.x, 4=PS 3.0 (Ctrl-click to set higher value)");

            ImGui::Unindent(16.0f);
        }
        else if (_editorPreset.presetVersion > 200)
        {
            ImGui::TextUnformatted("Pixel Shader Versions");
            ImGui::Indent(16.0f);

            ImGui::SliderInt("Warp PS Version", &_editorPreset.warpShaderVersion, 0, 4);
            DrawHelpTooltip("Minimum required DirectX Pixel Shader version for the warp shader.\n0/1=No PS, 2=PS 2.0, 3=PS 2.x, 4=PS 3.0 (Ctrl-click to set higher value)");
            ImGui::SliderInt("Composite PS Version", &_editorPreset.compositeShaderVersion, 0, 4);
            DrawHelpTooltip("Minimum required DirectX Pixel Shader version for the composite shader.\n0/1=No PS, 2=PS 2.0, 3=PS 2.x, 4=PS 3.0 (Ctrl-click to set higher value)");

            ImGui::Unindent(16.0f);
        }
    }
}

void PresetEditorGUI::DrawGeneralParameters()
{
    if (ImGui::CollapsingHeader("General Parameters"))
    {
        bool usesCompositeShader = _editorPreset.presetVersion >= 200 && _editorPreset.compositeShaderVersion >= 2;

        ImGui::TextUnformatted("Post-Processing Filters");
        ImGui::Indent(16.0f);

        ImGui::SliderFloat("Decay##PerFrameDecay", &_editorPreset.decay, 0.00f, 1.0f);
        DrawHelpTooltip("Controls the eventual fade to black.\n1=no fade, 0.9=strong fade, 0.98=recommended");

        ImGui::BeginDisabled(usesCompositeShader);

        ImGui::SliderFloat("Gamma Adjustment##GammaAdjustment", &_editorPreset.gammaAdj, 0.00f, 10.0f);
        DrawHelpTooltip("Controls display brightness.\n1=normal, 2=double, 3=triple, etc.\nOnly applied if no composite shader is used!");

        ImGui::Checkbox("Brighten", &_editorPreset.brighten);
        DrawHelpTooltip("Brightens the darker parts of the image (nonlinear; square root filter)\nOnly applied if no composite shader is used!");

        ImGui::Checkbox("Darken", &_editorPreset.darken);
        DrawHelpTooltip("Darkens the brighter parts of the image (nonlinear; squaring filter)\nOnly applied if no composite shader is used!");

        ImGui::Checkbox("Solarize", &_editorPreset.solarize);
        DrawHelpTooltip("Emphasizes mid-range colors\nOnly applied if no composite shader is used!");

        ImGui::Checkbox("Invert", &_editorPreset.invert);
        DrawHelpTooltip("Inverts the colors in the image\nOnly applied if no composite shader is used!");

        ImGui::Checkbox("Darken Center", &_editorPreset.darkenCenter);
        DrawHelpTooltip("Darkens a diamond-shaped area in the center of the image.\nApplied after drawing shapes/waveforms, but before drawing borders.");

        ImGui::Unindent(16.0f);
        ImGui::Spacing();

        static const char* videoEchoOrientations[] = {
            "Normal",
            "Flip X",
            "Flip Y",
            "Flip X + Y"};

        ImGui::TextUnformatted("Video Echo");
        ImGui::Indent(16.0f);
        ImGui::SliderFloat("Zoom##VideoEchoZoom", &_editorPreset.videoEchoZoom, 0.01f, 10.0f);
        DrawHelpTooltip("Controls the size of the second graphics layer\nOnly applied if no composite shader is used!");

        ImGui::SliderFloat("Alpha##VideoEchoAlpha", &_editorPreset.videoEchoAlpha, 0.00f, 1.0f);
        DrawHelpTooltip("Controls the opacity of the second graphics layer.\n0=transparent (off), 0.5=half-mix, 1=opaque\nOnly applied if no composite shader is used!");

        {
            int selectionIndex = _editorPreset.videoEchoOrientation % 4;
            const char* comboboxPreviewValue = videoEchoOrientations[_editorPreset.videoEchoOrientation % 4];
            if (ImGui::BeginCombo("Orientation##VideoEchoOrientation", comboboxPreviewValue))
            {
                for (int index = 0; index < 4; index++)
                {
                    const bool isSelected = (selectionIndex == index);
                    if (ImGui::Selectable(videoEchoOrientations[index], isSelected))
                    {
                        _editorPreset.videoEchoOrientation = index;
                    }

                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }

                ImGui::EndCombo();
            }
            DrawHelpTooltip("Selects an orientation for the second graphics layer.\nOnly applied if no composite shader is used!");
        }

        ImGui::EndDisabled();

        ImGui::Unindent(16.0f);
        ImGui::Spacing();

        ImGui::TextUnformatted("Blur Texture Value Range");
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        DrawHelpTooltip("Normally these are set to 0 (min) and 1 (max) each.\n"
                        "You can clamp the values in the blur texture to a tighter range,though.\n"
                        "This will increase the precision in the blur textures, but you run the risk of clamping values to your min/max.\n"
                        "If you use the GetBlur1() .. GetBlur3() functions to sample the blur texture, they will automatically \"unpack\" the values for you in the end!");

        ImGui::Indent(16.0f);

        ImGui::DragFloatRange2("Blur 1 Range", &_editorPreset.blur1Min, &_editorPreset.blur1Max, 0.01, 0.0, 1.0);
        ImGui::DragFloatRange2("Blur 2 Range", &_editorPreset.blur2Min, &_editorPreset.blur2Max, 0.01, 0.0, 1.0);
        ImGui::DragFloatRange2("Blur 3 Range", &_editorPreset.blur3Min, &_editorPreset.blur3Max, 0.01, 0.0, 1.0);
        ImGui::Spacing();
        ImGui::SliderFloat("Blur 1 Edge Darken", &_editorPreset.blur1EdgeDarken, 0.00f, 1.0f);

        ImGui::Unindent(16.0f);

        ImGui::Spacing();
    }
}

void PresetEditorGUI::DrawDefaultWaveformSettings()
{
    if (ImGui::CollapsingHeader("Default Waveform Effect"))
    {
        DrawWaveformModeSelection();

        ImGui::TextUnformatted("Drawing Settings");

        ImGui::Indent(16.0f);

        ImGui::Checkbox("Additive Waves##WaveformAdditive", &_editorPreset.additiveWaves);
        DrawHelpTooltip("The wave is drawn additively, saturating the image at white");

        ImGui::Checkbox("Dots##WaveformDrawDots", &_editorPreset.waveDots);
        DrawHelpTooltip("The waveform is drawn as dots (instead of lines)");

        ImGui::Checkbox("Thick##WaveformDrawThick", &_editorPreset.waveThick);
        DrawHelpTooltip("The waveform's lines (or dots) are drawn with double thickness");

        ImGui::SliderFloat("Scale##DefaultWaveformScale", &_editorPreset.waveScale, 0.00f, 5.0f);
        DrawHelpTooltip("Scaling factor of the waveform.\n1 = original size, 2 = twice the size, 0.5  = half the size");

        ImGui::SliderFloat("Smoothing##DefaultWaveformSmoothing", &_editorPreset.waveSmoothing, 0.00f, 1.0f);
        DrawHelpTooltip("Smoothing of the waveform.\n0 = no smoothing, 0.75 = heavy smoothing");

        ImGui::SliderFloat("Mystery Param##DefaultWaveformParam", &_editorPreset.waveParam, -1.00f, 1.0f);
        DrawHelpTooltip("This value does different things depending on the mode.\nFor example, it could control angle at which the waveform was drawn.");

        ImGui::Unindent(16.0f);
        ImGui::Spacing();

        ImGui::TextUnformatted("Position");

        ImGui::Indent(16.0f);

        ImGui::SliderFloat("X##DefaultWaveformX", &_editorPreset.waveX, 0.00f, 1.0f);
        DrawHelpTooltip("Position of the waveform.\n0 = far left edge of screen, 0.5 = center, 1 = far right");

        ImGui::SliderFloat("Y##DefaultWaveformY", &_editorPreset.waveY, 0.00f, 1.0f);
        DrawHelpTooltip("Position of the waveform.\n0 = very bottom of screen, 0.5 = center, 1 = top");

        ImGui::Unindent(16.0f);
        ImGui::Spacing();

        ImGui::TextUnformatted("Color");

        ImGui::Indent(16.0f);

        ImGui::ColorEdit4("Color##DefaultWaveformColor", &_editorPreset.waveColor.red, ImGuiColorEditFlags_Float);
        DrawHelpTooltip("The color of the waveform");

        ImGui::Checkbox("Maximize Wave Color", &_editorPreset.maximizeWaveColor);
        DrawHelpTooltip("All 3 R/G/B colors will be scaled up until at least one reaches 1.0");

        ImGui::Checkbox("Modulate Alpha by Volume", &_editorPreset.modWaveAlphaByVolume);
        DrawHelpTooltip("Modulate waveform alpha value by audio volume");

        ImGui::DragFloatRange2("Modulation Range", &_editorPreset.modWaveAlphaStart, &_editorPreset.modWaveAlphaEnd, 0.01, 0.0, 1.0);
        DrawHelpTooltip("Clamps alpha modulation 0->1 within this relative volume range.");

        ImGui::Unindent(16.0f);
        ImGui::Spacing();
    }
}

void PresetEditorGUI::DrawWaveformModeSelection()
{
    static const char* waveformModes[] = {
        "Circle",
        "X/Y Oscillation Spiral",
        "Centered Spiro",
        "Centered Spiro (Volume)",
        "Derivative Line",
        "Explosive Hash",
        "Line",
        "Double Line",
        "Spectrum Line",
        "Wave 9",
        "Wave X",
        "Wave 11",
        "Wave Skewed",
        "Wave Star",
        "Wave Flower",
        "Wave Lasso"};

    ImGui::TextUnformatted("Mode");

    ImGui::Indent(16.0f);

    int selectionIndex = _editorPreset.waveMode % 16;
    const char* comboboxPreviewValue = waveformModes[selectionIndex];
    if (ImGui::BeginCombo("##DefaultWaveformMode", comboboxPreviewValue))
    {
        for (int index = 0; index < 16; index++)
        {
            if (index == 0)
            {
                ImGui::TextDisabled("%s", "-- Original Milkdrop Modes --");
            }
            if (index == 9)
            {
                ImGui::TextDisabled("%s", "-- projectM / Milkdrop 3 Extras --");
            }
            const bool isSelected = (selectionIndex == index);
            if (ImGui::Selectable(waveformModes[index], isSelected))
            {
                _editorPreset.waveMode = index;
            }

            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }

        ImGui::EndCombo();
    }
    DrawHelpTooltip("Determines the drawing style of the classic default waveform.\n"
                    "Selecting any of the \"extra\" waveform modes will wrap around to "
                    "the original modes in classic Milkdrop versions.");

    ImGui::Unindent(16.0f);
    ImGui::Spacing();
}

void PresetEditorGUI::DrawMotionVectorSettings()
{
    if (ImGui::CollapsingHeader("Motion Vector Grid"))
    {
        ImGui::TextUnformatted("Layout And Position");

        ImGui::Indent(16.0f);

        ImGui::SliderFloat("Size X##MotionVectorX", &_editorPreset.mvX, 0.00f, 64.0f);
        DrawHelpTooltip("The number of motion vectors in the X direction");

        ImGui::SliderFloat("size Y##MotionVectorY", &_editorPreset.mvY, 0.00f, 48.0f);
        DrawHelpTooltip("The number of motion vectors in the Y direction");

        ImGui::SliderFloat("Length##MotionVectorLength", &_editorPreset.mvL, 0.00f, 5.0f);
        DrawHelpTooltip("The length of the motion vectors\n0=no trail, 1=normal, 2=double...");

        ImGui::SliderFloat("X Offset##MotionVectorOffsetX", &_editorPreset.mvDX, -1.00f, 1.0f);
        DrawHelpTooltip("Horizontal placement offset of the motion vectors");

        ImGui::SliderFloat("Y Offset##MotionVectorOffsetY", &_editorPreset.mvDY, -1.00f, 1.0f);
        DrawHelpTooltip("Vertical placement offset of the motion vectors");

        ImGui::Unindent(16.0f);
        ImGui::Spacing();

        ImGui::TextUnformatted("Color");

        ImGui::Indent(16.0f);

        ImGui::ColorEdit4("Color##MotionVectorColor", &_editorPreset.mvColor.red, ImGuiColorEditFlags_Float);
        DrawHelpTooltip("The color of the motion vector grid");

        ImGui::Unindent(16.0f);
        ImGui::Spacing();
    }
}

void PresetEditorGUI::DrawMotionCodeSettings()
{
    if (ImGui::CollapsingHeader("Motion Code"))
    {

        ImGui::TextUnformatted("Per-Frame Init");
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        DrawHelpTooltip("This code will only run once when the preset is loaded.\n"
                        "It can be used to set up the Per-Frame, qXX, regXX and megabuf variables to known initial (non-zero) values.\n"
                        "Calculations only required once can also be done in this code.");

        ImGui::Indent(16.0f);

        if (ImGui::Button(ICON_FA_PENCIL " Edit##EditPerFrameInitCode"))
        {
            EditCode(ExpressionCodeTypes::PerFrameInit, _editorPreset.perFrameInitCode);
        }
        ImGui::SameLine();
        ImGui::Text("(%lu lines / %lu characters)", GetLoC(_editorPreset.perFrameInitCode), _editorPreset.perFrameInitCode.length());

        ImGui::Unindent(16.0f);
        ImGui::Spacing();


        ImGui::TextUnformatted("Per-Frame");
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        DrawHelpTooltip("This code runs at the very beginning of each rendered frame.\n"
                        "Any complex/expensive calculations should be done here, eventually passing the results via qXX or regXX vars"
                        "or gmegabuf to effects later in the rendering process.\n"
                        "All qXX var values set here are copied to the Per-Vertex code and all custom waveforms and shapes.");

        ImGui::Indent(16.0f);

        if (ImGui::Button(ICON_FA_PENCIL " Edit##EditPerFrameCode"))
        {
            EditCode(ExpressionCodeTypes::PerFrame, _editorPreset.perFrameCode);
        }
        ImGui::SameLine();
        ImGui::Text("(%lu lines / %lu characters)", GetLoC(_editorPreset.perFrameCode), _editorPreset.perFrameCode.length());

        ImGui::Unindent(16.0f);
        ImGui::Spacing();


        ImGui::TextUnformatted("Per-Vertex");
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        DrawHelpTooltip("This code is executed for every grid vertex in the warp mesh.\n"
                        "Since this code may run several thousand times PER FRAME, it is key to keep it fast and slim.\n"
                        "Any values which don't depend on the x/y grid coordinates should be pre-calculated in the Per-Frame code"
                        "and passed in via qXX or regXX vars or using gmegabuf.\n"
                        "Also known als \"Per-Pixel\" code.");

        ImGui::Indent(16.0f);

        if (ImGui::Button(ICON_FA_PENCIL " Edit##EditPerVertexCode"))
        {
            EditCode(ExpressionCodeTypes::PerVertex, _editorPreset.perPixelCode);
        }
        ImGui::SameLine();
        ImGui::Text("(%lu lines / %lu characters)", GetLoC(_editorPreset.perPixelCode), _editorPreset.perPixelCode.length());

        ImGui::Unindent(16.0f);
        ImGui::Spacing();
    }
}

void PresetEditorGUI::DrawWarpMotionSettings()
{
    if (ImGui::CollapsingHeader("Warping and Motion"))
    {
        ImGui::TextUnformatted("Translation");
        ImGui::Indent(16.0f);

        ImGui::SliderFloat("Horizontal Motion", &_editorPreset.xPush, -1.00f, 1.0f);
        DrawHelpTooltip("Controls amount of constant horizontal motion.\n-0.01 = move left 1% per frame, 0=none, 0.01 = move right 1%");

        ImGui::SliderFloat("Vertical Motion", &_editorPreset.yPush, -1.00f, 1.0f);
        DrawHelpTooltip("Controls amount of constant vertical motion.\n-0.01 = move up 1% per frame, 0=none, 0.01 = move down 1%");

        ImGui::Unindent(16.0f);
        ImGui::Spacing();

        ImGui::TextUnformatted("Rotation");
        ImGui::Indent(16.0f);

        ImGui::SliderFloat("Rotation##WarpRotation", &_editorPreset.rot, -1.00f, 1.0f);
        DrawHelpTooltip("Controls the amount of rotation.\n0=none, 0.1=slightly right, -0.1=slightly clockwise, 0.1=CCW");

        ImGui::SliderFloat("Center X##WarpCenterX", &_editorPreset.rotCX, 0.00f, 1.0f);
        DrawHelpTooltip("Controls where the center of rotation and stretching is, horizontally.\n0=left, 0.5=center, 1=right");

        ImGui::SliderFloat("Center Y##WarpCenterY", &_editorPreset.rotCY, 0.00f, 1.0f);
        DrawHelpTooltip("Controls where the center of rotation and stretching is, vertically.\n0=top, 0.5=center, 1=bottom");

        ImGui::Unindent(16.0f);
        ImGui::Spacing();

        ImGui::TextUnformatted("Scaling");
        ImGui::Indent(16.0f);

        ImGui::SliderFloat("Stretch X##WarpStretchX", &_editorPreset.stretchX, 0.00f, 2.0f);
        DrawHelpTooltip("Controls amount of constant horizontal stretching.\n0.99=shrink 1%, 1=normal, 1.01=stretch 1%");

        ImGui::SliderFloat("Stretch Y##WarpStretchY", &_editorPreset.stretchY, 0.00f, 2.0f);
        DrawHelpTooltip("Controls amount of constant vertical stretching.\n0.99=shrink 1%, 1=normal, 1.01=stretch 1%");

        ImGui::SliderFloat("Zoom##WarpZoom", &_editorPreset.zoom, 0.00f, 2.0f);
        DrawHelpTooltip("Controls inward/outward motion.\n0.9=zoom out 10% per frame, 1.0=no zoom, 1.1=zoom in 10%");

        ImGui::SliderFloat("Zoom Exponent##Warp", &_editorPreset.zoomExponent, 0.00f, 5.0f);
        DrawHelpTooltip("Controls the curvature of the zoom; 1=normal");

        ImGui::Unindent(16.0f);
        ImGui::Spacing();

        ImGui::TextUnformatted("Warping");
        ImGui::Indent(16.0f);

        ImGui::SliderFloat("Warp Amount##WarpAmount", &_editorPreset.warpAmount, 0.00f, 10.0f);
        DrawHelpTooltip("Controls the magnitude of the warping.\n0=none, 1=normal, 2=major warping...");

        ImGui::SliderFloat("Warp Scale##WarpScale", &_editorPreset.warpScale, 0.00f, 10.0f);
        DrawHelpTooltip("Controls the scale of the warp effect.");

        ImGui::SliderFloat("Warp Animation Speed##WarpAnimSpeed", &_editorPreset.warpAnimSpeed, 0.00f, 5.0f);
        DrawHelpTooltip("Controls the speed of the warp effect.");

        ImGui::Unindent(16.0f);
        ImGui::Spacing();

        ImGui::TextUnformatted("Options");
        ImGui::Indent(16.0f);

        ImGui::Unindent(16.0f);
        ImGui::Spacing();
    }
}

void PresetEditorGUI::DrawBorderSettings()
{
    if (ImGui::CollapsingHeader("Border Effect"))
    {
        ImGui::TextUnformatted("Outer Border");
        ImGui::Indent(16.0f);

        ImGui::SliderFloat("Thickness##OuterBorderThickness", &_editorPreset.outerBorderSize, 0.0f, 1.0f);
        DrawHelpTooltip("Thickness of the outer border drawn at the edges of the screen every frame");
        ImGui::ColorEdit4("Color##OuterBorderColor", &_editorPreset.outerBorderColor.red, ImGuiColorEditFlags_Float);
        DrawHelpTooltip("Color of the outer border drawn at the edges of the screen every frame");

        ImGui::Unindent(16.0f);

        ImGui::Spacing();

        ImGui::TextUnformatted("Inner Border");
        ImGui::Indent(16.0f);

        ImGui::SliderFloat("Thickness##InnerBorderThickness", &_editorPreset.innerBorderSize, 0.0f, 1.0f);
        DrawHelpTooltip("Thickness of the inner border drawn at the edges of the screen every frame");

        ImGui::ColorEdit4("Color##InnerBorderColor", &_editorPreset.innerBorderColor.red, ImGuiColorEditFlags_Float);
        DrawHelpTooltip("Color of the inner border drawn at the edges of the screen every frame");

        ImGui::Unindent(16.0f);

        ImGui::Spacing();
    }
}

void PresetEditorGUI::DrawCustomWaveSettings(EditorPreset::Wave& waveform)
{
    std::string idx = std::to_string(waveform.index + 1);

    ImGui::PushID(std::string("CustomWave" + idx).c_str());

    if (ImGui::CollapsingHeader(std::string("Waveform " + idx).c_str()))
    {
        ImGui::Checkbox("Enabled", &waveform.enabled);
        DrawHelpTooltip("This waveform is only rendered if enabled explicitly.");

        ImGui::Spacing();

        ImGui::BeginDisabled(!waveform.enabled);

        ImGui::TextUnformatted("Position And Style");
        ImGui::Indent(16.0f);

        ImGui::SliderFloat("X", &waveform.x, 0.0f, 1.0f);
        DrawHelpTooltip("Horizontal position of the waveform.\n"
                        "0=left, 0.5=center, 1=right");

        ImGui::SliderFloat("Y", &waveform.x, 0.0f, 1.0f);
        DrawHelpTooltip("Vertical position of the waveform.\n"
                        "0=bottom, 0.5=center, 1=top");

        ImGui::SliderInt("Samples", &waveform.samples, 1, 512);
        DrawHelpTooltip("Number of waveform points to draw.");

        ImGui::SliderInt("Separation", &waveform.sep, 0, 255);
        DrawHelpTooltip("Separation distance of dual waveforms.");

        ImGui::SliderFloat("Scaling", &waveform.scaling, 0.0f, 10.0f);
        DrawHelpTooltip("Waveform value scaling factor.");

        ImGui::SliderFloat("Smoothing", &waveform.scaling, 0.0f, 1.0f);
        DrawHelpTooltip("Waveform smoothing value.\n"
                        "0.0=no smoothing, 0.5=default, 1.0=extreme smoothing");

        ImGui::ColorEdit4("Color", &waveform.color.red, ImGuiColorEditFlags_Float);
        DrawHelpTooltip("The default color of the waveform.");

        ImGui::Unindent(16.0f);
        ImGui::Spacing();

        ImGui::TextUnformatted("Drawing Flags");
        ImGui::Indent(16.0f);

        ImGui::Checkbox("Spectrum", &waveform.spectrum);
        DrawHelpTooltip("Use spectrum instead of oscilloscope data.");

        ImGui::Checkbox("Dots", &waveform.useDots);
        DrawHelpTooltip("Draw Waveform as dots instead of lines.");

        ImGui::Checkbox("Thick", &waveform.drawThick);
        DrawHelpTooltip("Draw waveform lines or dots twice as thick.");

        ImGui::Checkbox("Additive", &waveform.additive);
        DrawHelpTooltip("Use additive color blending when drawing.");

        ImGui::Unindent(16.0f);
        ImGui::Spacing();

        ImGui::TextUnformatted("Drawing Code");
        ImGui::Indent(16.0f);

        ImGui::TextUnformatted("Initialization");
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        DrawHelpTooltip("This code will only run once when the preset is loaded.\n"
                        "It can be used to set up variables specific to this waveform to"
                        " known initial (non-zero) values. The waveform tX variables can also be initialized here.");

        ImGui::Indent(16.0f);

        if (ImGui::Button(ICON_FA_PENCIL " Edit##InitCode"))
        {
            EditCode(ExpressionCodeTypes::CustomWaveInit, waveform.initCode, waveform.index + 1);
        }
        ImGui::SameLine();
        ImGui::Text("(%lu lines / %lu characters)", GetLoC(waveform.initCode), waveform.initCode.length());

        ImGui::Unindent(16.0f);
        ImGui::Spacing();


        ImGui::TextUnformatted("Per-Frame");
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        DrawHelpTooltip("This code runs once before rendering the waveform.\n"
                        "Any complex/expensive calculations should be done here, eventually passing the results via qXX/tX vars "
                        "or (g)megabuf to the per-point code.\n"
                        "All qXX/tX var values set here are copied to each Per-Point code instance.");

        ImGui::Indent(16.0f);

        if (ImGui::Button(ICON_FA_PENCIL " Edit##PerFrameCode"))
        {
            EditCode(ExpressionCodeTypes::CustomWavePerFrame, waveform.perFrameCode, waveform.index + 1);
        }
        ImGui::SameLine();
        ImGui::Text("(%lu lines / %lu characters)", GetLoC(waveform.perFrameCode), waveform.perFrameCode.length());

        ImGui::Unindent(16.0f);
        ImGui::Spacing();


        ImGui::TextUnformatted("Per-Point");
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        DrawHelpTooltip("This code is executed for every point of the waveform.\n"
                        "This allows full control over the placement of each vertex of the waveform, "
                        "enabling drawing arbitrary shapes.");

        ImGui::Indent(16.0f);

        if (ImGui::Button(ICON_FA_PENCIL " Edit##PerPointCode"))
        {
            EditCode(ExpressionCodeTypes::CustomWavePerPoint, waveform.perPointCode, waveform.index + 1);
        }
        ImGui::SameLine();
        ImGui::Text("(%lu lines / %lu characters)", GetLoC(waveform.perPointCode), waveform.perPointCode.length());

        ImGui::Unindent(16.0f);
        ImGui::Spacing();

        ImGui::Unindent(16.0f);
        ImGui::Spacing();

        ImGui::EndDisabled();
    }
    else if (waveform.enabled)
    {
        ImGui::SameLine();
        ImGui::TextDisabled(" " ICON_FA_CHECK " Enabled");
    }

    ImGui::PopID();
}

void PresetEditorGUI::DrawCustomShapeSettings(EditorPreset::Shape& shape)
{
    std::string idx = std::to_string(shape.index + 1);

    ImGui::PushID(std::string("CustomShape" + idx).c_str());

    if (ImGui::CollapsingHeader(std::string("Shape " + idx).c_str()))
    {
        ImGui::Checkbox("Enabled", &shape.enabled);
        DrawHelpTooltip("This shape is only rendered if enabled explicitly.");

        ImGui::Spacing();

        ImGui::BeginDisabled(!shape.enabled);

        ImGui::TextUnformatted("Position And Style");
        ImGui::Indent(16.0f);

        ImGui::SliderFloat("X", &shape.x, 0.0f, 1.0f);
        DrawHelpTooltip("Default horizontal position of the shape.\n"
                        "0=left, 0.5=center, 1=right");

        ImGui::SliderFloat("Y", &shape.y, 0.0f, 1.0f);
        DrawHelpTooltip("Default vertical position of the shape.\n"
                        "0=bottom, 0.5=center, 1=top");

        ImGui::SliderFloat("Radius", &shape.radius, 0.001f, 10.0f);
        DrawHelpTooltip("Default radius of the shape.");

        ImGui::SliderFloat("Angle", &shape.angle, 0.0f, 2 * 3.14159265358979323846);
        DrawHelpTooltip("Default rotation angle of the shape.");

        ImGui::SliderInt("Sides", &shape.sides, 3, 100);
        DrawHelpTooltip("The default number of sides that make up the polygonal shape.");

        ImGui::SliderInt("Instances", &shape.instances, 1, 1024);
        DrawHelpTooltip("The total number of instances (the number of times to repeat the per-frame equations for, and draw, this shape).");

        ImGui::ColorEdit4("Inner Color", &shape.color.red, ImGuiColorEditFlags_Float);
        DrawHelpTooltip("The default color and opacity towards the center of the shape.");

        ImGui::ColorEdit4("Outer Color", &shape.color2.red, ImGuiColorEditFlags_Float);
        DrawHelpTooltip("The default color and opacity towards the outer edge of the shape");

        ImGui::ColorEdit4("Border Color", &shape.borderColor.red, ImGuiColorEditFlags_Float);
        DrawHelpTooltip("The default color and opacity of the border of the shape");

        ImGui::Unindent(16.0f);
        ImGui::Spacing();

        ImGui::TextUnformatted("Texturing");
        ImGui::Indent(16.0f);

        ImGui::Checkbox("Textured", &shape.textured);
        DrawHelpTooltip("If enabled, the shape will be textured with the image from the previous frame.");

        ImGui::BeginDisabled(!shape.textured);

        ImGui::SliderFloat("Angle##Texture", &shape.tex_ang, 0.0f, 2 * 3.14159265358979323846);
        DrawHelpTooltip("The angle at which to rotate the previous frame's image before applying it to the shape.");

        ImGui::SliderFloat("Zoom##Texture", &shape.tex_zoom, 0.001f, 10.0f);
        DrawHelpTooltip("The portion of the previous frame's image to use with the shape.");

        ImGui::EndDisabled();

        ImGui::Unindent(16.0f);
        ImGui::Spacing();

        ImGui::TextUnformatted("Drawing Flags");
        ImGui::Indent(16.0f);
        ImGui::Checkbox("Thick Outline", &shape.thickOutline);
        DrawHelpTooltip("If enabled, the border will be overdrawn 4 times to make it thicker, bolder, and more visible.");

        ImGui::Checkbox("Additive", &shape.additive);
        DrawHelpTooltip("If enabled, the shape will add color to saturate the image toward white; otherwise, it will replace what's there.");

        ImGui::Unindent(16.0f);
        ImGui::Spacing();

        ImGui::TextUnformatted("Drawing Code");
        ImGui::Indent(16.0f);

        ImGui::TextUnformatted("Initialization");
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        DrawHelpTooltip("This code will only run once when the preset is loaded.\n"
                        "It can be used to set up variables specific to this shape to"
                        " known initial (non-zero) values. The shape tX variables can ONLY be initialized here.");

        ImGui::Indent(16.0f);

        if (ImGui::Button(ICON_FA_PENCIL " Edit##InitCode"))
        {
            EditCode(ExpressionCodeTypes::CustomShapeInit, shape.initCode, shape.index + 1);
        }
        ImGui::SameLine();
        ImGui::Text("(%lu lines / %lu characters)", GetLoC(shape.initCode), shape.initCode.length());

        ImGui::Unindent(16.0f);
        ImGui::Spacing();


        ImGui::TextUnformatted("Per-Frame");
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        DrawHelpTooltip("This code runs once per shape instance.\n"
                        "Any complex/expensive calculations should be done in the preset per-frame code, eventually passing the results via qXX/tX vars "
                        "or gmegabuf to the shape per-frame code. The tX variables are reset to their init values for each instance.");

        ImGui::Indent(16.0f);

        if (ImGui::Button(ICON_FA_PENCIL " Edit##PerFrameCode"))
        {
            EditCode(ExpressionCodeTypes::CustomShapePerFrame, shape.perFrameCode, shape.index + 1);
        }
        ImGui::SameLine();
        ImGui::Text("(%lu lines / %lu characters)", GetLoC(shape.perFrameCode), shape.perFrameCode.length());

        ImGui::Unindent(16.0f);
        ImGui::Spacing();

        ImGui::Unindent(16.0f);
        ImGui::Spacing();

        ImGui::EndDisabled();
    }
    else if (shape.enabled)
    {
        ImGui::SameLine();
        ImGui::TextDisabled(" " ICON_FA_CHECK " Enabled");
    }

    ImGui::PopID();
}

void PresetEditorGUI::DrawShaderCodeSettings()
{
    bool shaderTabDisabled = _editorPreset.presetVersion < 200 || (_editorPreset.warpShaderVersion < 2 && _editorPreset.compositeShaderVersion < 2);

    ImGui::BeginDisabled(shaderTabDisabled);
    if (ImGui::CollapsingHeader("Warp / Composite Shaders"))
    {

        ImGui::BeginDisabled(_editorPreset.warpShaderVersion < 2);

        ImGui::TextUnformatted("Warp Shader");
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        DrawHelpTooltip("The warp shader is used to draw the warp mesh, after the motion vectors were drawn but before "
                        "any waveforms or shapes are rendered. It can be used to amend or replace the classic Milkdrop 1 "
                        "warp effect. It is not necessary to use the previous image at all - this shader may create a new"
                        "image on each frame, onto which waveforms and shapes are drawn.\n\n" ICON_FA_CODE
                        " This shader must be written in DirectX HLSL.");

        ImGui::Indent(16.0f);

        if (ImGui::Button(ICON_FA_PENCIL " Edit##WarpShader"))
        {
            EditCode(ExpressionCodeTypes::WarpShader, _editorPreset.warpShader);
        }
        ImGui::SameLine();
        ImGui::Text("(%lu lines / %lu characters)", GetLoC(_editorPreset.warpShader), _editorPreset.warpShader.length());

        ImGui::Unindent(16.0f);
        ImGui::Spacing();

        ImGui::EndDisabled();

        ImGui::BeginDisabled(_editorPreset.compositeShaderVersion < 2);

        ImGui::TextUnformatted("Composite Shader");
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        DrawHelpTooltip("The composite shader runs immediately before displaying the preset image to the user, "
                        "allowing post-processing of the previous rendering steps or even drawing a whole new image "
                        "on top of it. The input (sampler_main) to the composite shader is passed on to the next frame, "
                        "the output of the composite shader will only be displayed on screen and then discarded when "
                        "the next frame is rendered.\n\n" ICON_FA_CODE " This shader must be written in DirectX HLSL.");

        ImGui::Indent(16.0f);

        if (ImGui::Button(ICON_FA_PENCIL " Edit##CompositeShader"))
        {
            EditCode(ExpressionCodeTypes::CompositeShader, _editorPreset.compositeShader);
        }
        ImGui::SameLine();
        ImGui::Text("(%lu lines / %lu characters)", GetLoC(_editorPreset.compositeShader), _editorPreset.compositeShader.length());

        ImGui::Unindent(16.0f);
        ImGui::Spacing();

        ImGui::EndDisabled();
    }
    if (shaderTabDisabled)
    {
        DrawHelpTooltip("To enable HLSL shaders, open the compatibility settings and set the "
                        "preset version to 200 or higher and the PS version to 2 or higher.");
    }
    ImGui::EndDisabled();

    DrawShaderLossWarning();
}

void PresetEditorGUI::DrawShaderLossWarning() const
{
    if (_editorPreset.presetVersion < 200)
    {
        if (!_editorPreset.warpShader.empty() || !_editorPreset.compositeShader.empty())
        {
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
            ImGui::TextWrapped("%s", "WARNING: You have disabled shader support, but the preset still contains shader code.\n"
                                     "The shader code WILL NOT BE SAVED and gets lost once you close this preset in the editor.");
            ImGui::PopStyleColor();
        }
    }
    else
    {
        if (_editorPreset.warpShaderVersion < 2 && !_editorPreset.warpShader.empty())
        {

            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
            ImGui::TextWrapped("%s", "WARNING: You have selected a WARP shader PS Version below 2, but the preset still contains warp shader code.\n"
                                     "The warp shader code WILL NOT BE SAVED and gets lost once you close this preset in the editor.");
            ImGui::PopStyleColor();
        }
        if (_editorPreset.compositeShaderVersion < 2 && !_editorPreset.compositeShader.empty())
        {

            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
            ImGui::TextWrapped("%s", "WARNING: You have selected a COMPOSITE shader PS Version below 2, but the preset still contains composite shader code.\n"
                                     "The composite shader WILL NOT BE SAVED and gets lost once you close this preset in the editor.");
            ImGui::PopStyleColor();
        }
    }
}

void PresetEditorGUI::DrawHelpTooltip(const std::string& helpText)
{
    if (ImGui::BeginItemTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(helpText.c_str());
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

} // namespace Editor