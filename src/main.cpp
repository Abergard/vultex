// https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Base_code

#include <cstdlib>
#include <exception>

#include "gfx/graphics.hpp"
#include "ui/window.hpp"
#include "utility/logger.hpp"

class HelloTrangleApplication
{
public:
    HelloTrangleApplication()
    {
    }

    HelloTrangleApplication(const HelloTrangleApplication&) = delete;
    HelloTrangleApplication(HelloTrangleApplication&&) = delete;
    HelloTrangleApplication& operator=(const HelloTrangleApplication&) = delete;
    HelloTrangleApplication& operator=(HelloTrangleApplication&&) = delete;

    ~HelloTrangleApplication() noexcept
    {
    }

    auto run() -> void
    {
        window.loop();
    }

private:
    ui::Window window;
    std::unique_ptr<gfx::Graphics> graphics{gfx::Graphics::create(gfx::Api::Vulkan)};
};

int main()
try
{
    utility::initSpdlog();

    HelloTrangleApplication{}.run();

    return EXIT_SUCCESS;
}
catch (const std::exception& e)
{
    log::error("{}", e.what());
    return EXIT_FAILURE;
}
