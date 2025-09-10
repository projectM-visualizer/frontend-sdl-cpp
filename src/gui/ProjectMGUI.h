#pragma once

#include "notifications/DisplayToastNotification.h"

#include <SDL2/SDL.h>

#include <Poco/Logger.h>
#include <Poco/NObserver.h>

#include <Poco/Util/Subsystem.h>

#include <memory>

namespace Editor {
class PresetEditorGUI;
}

struct ImFont;
class ProjectMWrapper;
class SDLRenderingWindow;
class MainMenu;
class SettingsWindow;
class AboutWindow;
class HelpWindow;
class ToastMessage;

class ProjectMGUI : public Poco::Util::Subsystem
{
public:
    ProjectMGUI();

    ~ProjectMGUI() override;

    const char* name() const override;

    void initialize(Poco::Util::Application& app) override;

    void uninitialize() override;

    /**
     * @brief Updates the font size after DPI changes.
     */
    void UpdateFontSize();

    /**
     * @brief Processes SDL input events in Dear ImGui.
     * @param event The SDL event.
     */
    void ProcessInput(const SDL_Event& event);

    /**
     * @brief Displays or hides the UI.
     */
    void Toggle();

    /**
     * @brief Sets the visibility of the UI.
     * @param visible true if the UI will be rendered, false if not.
     */
    void Visible(bool visible);

    /**
     * @brief Returns the visibiity of the UI.
     * @return true if the UI is visible, false if not.
     */
    bool Visible() const;

    /**
     * @brief Draws the UI, including toasts.
     * If neither a toast nor the UI are visible, this is basically a no-op.
     */
    void Draw();

    /**
     * @brief Tells the caller whether the UI currently wants the keyboard input.
     * @return True if the UI wants the keyboard input, false if the app should process the events.
     */
    bool WantsKeyboardInput();

    /**
     * @brief Tells the caller whether the UI currently wants the mouse input.
     * @return True if the UI wants the mouse input, false if the app should process the events.
     */
    bool WantsMouseInput();

    /**
     * @brief Pushes the "toast" font to the render stack
     */
    void PushToastFont();

    /**
     * @brief Pushes the "UI" font to the render stack
     */
    void PushUIFont();

    /**
     * @brief Pushes the "Solid" icon font to the render stack
     */
    void PushSolidIconsFont();

    /**
     * @brief Pushes the "Regular" icon font to the render stack
     */
    void PushRegularIconsFont();

    /**
     * @brief Pops the last font from the stack
     */
    void PopFont();

    /**
     * @brief Opens the preset editor UI.
     * @param presetFileName The file name of the preset to edit, or empty to use the currently loaded preset.
     */
    void ShowPresetEditor(const std::string& presetFileName);

    /**
     * @brief Displays the settings window.
     */
    void ShowSettingsWindow();

    /**
     * @brief Displays the about window.
     */
    void ShowAboutWindow();

    /**
     * @brief Displays the help window.
     */
    void ShowHelpWindow();

private:
    float GetScalingFactor();

    static float GetClampedUserScalingFactor();

    void DisplayToastNotificationHandler(const Poco::AutoPtr<DisplayToastNotification>& notification);

    ProjectMWrapper* _projectMWrapper{nullptr};

    Poco::NObserver<ProjectMGUI, DisplayToastNotification> _displayToastNotificationObserver{*this, &ProjectMGUI::DisplayToastNotificationHandler};

    std::string _uiIniFileName; //!< Path and filename of the UI configuration (positions etc.)

    SDL_Window* _renderingWindow{nullptr}; //!< Pointer to the SDL window used for rendering.
    SDL_GLContext _glContext{nullptr}; //!< Pointer to the OpenGL context associated with the window.
    ImFont* _uiFont{nullptr}; //!< Main UI font (monospaced).
    ImFont* _toastFont{nullptr}; //!< Toast message font (sans-serif, larger).
    ImFont* _fontAwesomeIconsSolid{nullptr}; //!< "Solid" icons set by FontAwesome.
    ImFont* _fontAwesomeIconsRegular{nullptr}; //!< "Regular" icons set by FontAwesome.

    uint64_t _lastFrameTicks{0}; //!< Tick count of the last frame (see SDL_GetTicks64)

    float _userScalingFactor{1.0f}; //!< The user-defined UI scaling factor.
    float _textScalingFactor{0.0f}; //!< The text scaling factor.

    std::unique_ptr<MainMenu> _mainMenu;
    std::unique_ptr<Editor::PresetEditorGUI> _presetEditorGUI; //!< The preset editor GUI.
    std::unique_ptr<SettingsWindow> _settingsWindow; //!< The settings window.
    std::unique_ptr<AboutWindow> _aboutWindow; //!< The about window.
    std::unique_ptr<HelpWindow> _helpWindow; //!< Help window with shortcuts and tips.

    std::unique_ptr<ToastMessage> _toast; //!< Current toast to be displayed.

    bool _visible{false}; //!< Flag for settings window visibility.

    Poco::Logger& _logger{Poco::Logger::get("ProjectMGUI")}; //!< The class logger.
};
