#pragma once

#include <functional>
#include <memory>

void requestClipboardText(std::function<void(const std::string&)> callback);
