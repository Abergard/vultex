// https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Base_code

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <exception>
#include <iostream>
#include <spdlog/spdlog.h>
#include <stdexcept>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

class HelloTrangleApplication
{
public:
    void run()
    {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    void initWindow()
    {
        spdlog::info("Initialize window");

        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(WIDTH, HEIGHT, "Vultex", nullptr, nullptr);
    }

    void initVulkan()
    {
        spdlog::info("Initialize Vulkan");
    }

    void mainLoop()
    {
        spdlog::info("Start loop");        
        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();
        }
        spdlog::info("Loop finished");
    }

    void cleanup()
    {
        spdlog::info("Cleanup resources");
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    GLFWwindow* window{nullptr};
};

int main()
try
{
    HelloTrangleApplication{}.run();

    return EXIT_SUCCESS;
}
catch (const std::exception& e)
{
    return EXIT_FAILURE;
}
