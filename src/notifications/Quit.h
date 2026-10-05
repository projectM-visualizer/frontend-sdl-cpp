#pragma once

#include <Poco/Notification.h>

namespace Notification {

/**
 * @brief Informs the application that the user wants to quit.
 */
class Quit : public Poco::Notification
{
public:
    std::string name() const override;
};

} // namespace Notification
