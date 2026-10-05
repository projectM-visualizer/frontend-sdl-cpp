#pragma once

#include <Poco/Notification.h>

namespace Notification {

/**
 * @brief Informs the GUI subsystem to queue a new toast message.
 */
class DisplayToast : public Poco::Notification
{
public:
    std::string name() const override;

    DisplayToast() = delete;

    explicit DisplayToast(std::string toastText);

    const std::string& ToastText() const;

private:
    std::string _toastText;
};

} // namespace Notification
