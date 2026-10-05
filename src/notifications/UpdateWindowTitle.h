#pragma once

#include <Poco/Notification.h>

namespace Notification {

/**
 * @brief Informs the application that the window title should be updated.
 */
class UpdateWindowTitle : public Poco::Notification
{
public:
    std::string name() const override;
};

} // namespace Notification
