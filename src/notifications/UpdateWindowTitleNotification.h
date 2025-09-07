#pragma once

#include <Poco/Notification.h>

#include <utility>

/**
 * @brief Informs the application that the window title should be updated.
 */
class UpdateWindowTitleNotification : public Poco::Notification
{
public:
    UpdateWindowTitleNotification() = default;

    explicit UpdateWindowTitleNotification(std::string customTitle)
        : _customTitle(std::move(customTitle))
    {}

    std::string name() const override;

    std::string _customTitle;
};
