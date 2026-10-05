#include "DisplayToast.h"

namespace Notification {

DisplayToast::DisplayToast(std::string toastText)
    : _toastText(std::move(toastText))
{
}

std::string DisplayToast::name() const
{
    return "DisplayToastNotification";
}

const std::string& DisplayToast::ToastText() const
{
    return _toastText;
}

} // namespace Notification
