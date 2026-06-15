#pragma once

#include <vector>

struct GLFWwindow;

namespace ui
{
enum class GraphicsLibrary : char
{
    Vulkan,
    DirectX,
    OpenGL
};

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
    [[nodiscard]] static auto getRequiredExtensions(GraphicsLibrary) -> std::vector<const char*>;

private:
    GLFWwindow* window{nullptr};
};

} // namespace ui