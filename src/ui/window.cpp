#include "window.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "utility/logger.hpp"

namespace ui
{
const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

namespace
{
GLFWwindow* createGlfwWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    return glfwCreateWindow(WIDTH, HEIGHT, "Vultex", nullptr, nullptr);
}
} // namespace

Window::Window() : window{createGlfwWindow()}
{
    log::info("Initialize window");
}

Window::~Window()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Window::loop()
{
    log::info("Start loop");
    while (1 != glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }
    log::info("Loop finished");
}

[[nodiscard]] auto Window::getRequiredExtensions(const gfx::Api api) -> std::vector<const char*>
{
    switch (api)
    {
    case gfx::Api::DirectX:
        [[fallthrough]];
    case gfx::Api::OpenGL:
        log::warn("Graphics library {} is not supported yet, fallback to Vulkan", static_cast<char>(api));
        [[fallthrough]];
    case gfx::Api::Vulkan:
        log::info("Using Vulkan graphics library");
        break;
    }

    std::uint32_t glfwExtensionCount = 0;
    const auto** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    return {glfwExtensions, std::next(glfwExtensions, glfwExtensionCount)};
}
} // namespace ui