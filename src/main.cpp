// https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Base_code

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdint>
#include <cstdlib>
#include <exception>
#include <fmt/format.h>
#include <iostream>
#include <iterator>
#include <map>
#include <span>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <string_view>

#include "vulkan_debug.hpp"
#include "vulkan_property_support_info.hpp"

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
        createInstance();
        setupDebugMessenger();
    }
    void setupDebugMessenger()
    {
        if constexpr (!enableValidationLayers)
        {
            return;
        }

        spdlog::info("Initialize debug messenger");

        VkDebugUtilsMessengerCreateInfoEXT createInfo{};
        populateDebugMessengerCreateInfo(createInfo);

        if (VK_SUCCESS != CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger))
        {
            throw std::runtime_error("failed to set up debug messenger!");
        }
    }

    static std::vector<const char*> getRequiredExtensions()
    {
        std::uint32_t glfwExtensionCount = 0;
        const auto** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, std::next(glfwExtensions, glfwExtensionCount));

        if constexpr (enableValidationLayers)
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }
        return extensions;
    }

    void createInstance()
    {

        // fill an optional struct with application information
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello vultex!";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_2;

        // global information about the entire program about extensions etc.
        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        const auto&& glfwExtensions = getRequiredExtensions();
        // get vulkan extensions required by GLFW
        {
            // mark extensions requried by glfw
            const auto glfw_required_extensions =
                utility::check_glfw_required_extensions(glfwExtensions.size(), glfwExtensions.data());

            spdlog::info("EnabledExtensionCount: {}", glfwExtensions.size());
            createInfo.enabledExtensionCount = glfwExtensions.size();
            createInfo.ppEnabledExtensionNames = glfwExtensions.data();

            glfw_required_extensions.log_properties();
            if (!glfw_required_extensions.all_supported())
            {
                throw std::runtime_error(fmt::format("Cannot create vulkan instance! glfw all supported: {}",
                                                     glfw_required_extensions.all_supported()));
            }
        }

        const std::vector<const char*> required_validation_layer_names = {"VK_LAYER_KHRONOS_validation"};
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        // configure validation layers
        {

            if constexpr (enableValidationLayers)
            {

                const auto required_validation_layers = utility::check_required_validation_layers(
                    required_validation_layer_names.size(), required_validation_layer_names.data());

                required_validation_layers.log_properties();
                if (!required_validation_layers.all_supported())

                {
                    throw std::runtime_error("validation layers requested, but not available!");
                }
                createInfo.enabledLayerCount =
                    static_cast<std::uint32_t>(required_validation_layer_names.size());
                createInfo.ppEnabledLayerNames = required_validation_layer_names.data();

                populateDebugMessengerCreateInfo(debugCreateInfo);
                createInfo.pNext = &debugCreateInfo;
            }
            else
            {
                createInfo.enabledLayerCount = 0;
                createInfo.pNext = nullptr;
            }
        }

        const auto create_instance_status = vkCreateInstance(&createInfo, nullptr, &instance);
        if (VK_SUCCESS != create_instance_status)
        {
            throw std::runtime_error{
                fmt::format("Cannot create vulkan instance: {}", create_instance_status)};
        }

        spdlog::debug("Instance created");
    }

    void mainLoop()
    {
        spdlog::info("Start loop");
        while (1 != glfwWindowShouldClose(window))
        {
            glfwPollEvents();
        }
        spdlog::info("Loop finished");
    }

    void cleanup()
    {
        spdlog::info("Cleanup resources");

        if constexpr (enableValidationLayers)
        {
            DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        }
        vkDestroyInstance(instance, nullptr);
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    VkInstance instance{nullptr};
    VkDebugUtilsMessengerEXT debugMessenger{nullptr};
    GLFWwindow* window{nullptr};
};

int main()
try
{
    spdlog::set_level(spdlog::level::trace);

    HelloTrangleApplication{}.run();

    return EXIT_SUCCESS;
}
catch (const std::exception& e)
{
    spdlog::error("{}", e.what());
    return EXIT_FAILURE;
}
