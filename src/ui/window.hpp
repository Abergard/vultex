#pragma once

#include <vector>

#include "gfx/api.hpp"

struct GLFWwindow;

namespace ui
{
class Window
{
public:
    explicit Window();
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;

    void loop();
    [[nodiscard]] static auto getRequiredExtensions(gfx::Api) -> std::vector<const char*>;

private:
    GLFWwindow* window{nullptr};
};

} // namespace ui