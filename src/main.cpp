// https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Base_code

#include <cstdlib>
#include <exception>

#include "ui/window.hpp"
#include "utility/logger.hpp"
#include "vulkan/vulkan.hpp"

class HelloTrangleApplication
{
public:
    HelloTrangleApplication()
        : instance{vk::createVulkanInstance()}, debugMessenger{vk::setupDebugMessenger(instance)}
    {
        auto [physicalDevice, queueFamilyIndex] = vk::pickPhysicalDevice(instance);
        logicalDevice = vk::createLogicalDevice(physicalDevice, queueFamilyIndex);

        // get family queue indice from logical device
        constexpr auto firstQueueIndex = 0;
        vkGetDeviceQueue(logicalDevice, queueFamilyIndex, firstQueueIndex, &graphicsQueue);
    }

    HelloTrangleApplication(const HelloTrangleApplication&) = delete;
    HelloTrangleApplication(HelloTrangleApplication&&) = delete;
    HelloTrangleApplication& operator=(const HelloTrangleApplication&) = delete;
    HelloTrangleApplication& operator=(HelloTrangleApplication&&) = delete;

    ~HelloTrangleApplication()
    {
        log::info("Cleanup resources");

        vkDestroyDevice(logicalDevice, nullptr);
        vk::destroyDebugMessenger(instance, debugMessenger);
        vkDestroyInstance(instance, nullptr);
    }

    auto run() -> void
    {
        window.loop();
    }

private:
    ui::Window window;
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;

    VkDevice logicalDevice{nullptr};
    VkQueue graphicsQueue{nullptr};
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
