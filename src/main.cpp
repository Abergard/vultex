// https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Base_code

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <fmt/format.h>
#include <iostream>
#include <iterator>
#include <map>
#include <optional>
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
    auto run()
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
        pickPhysicalDevice();
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

    void pickPhysicalDevice()
    {
        std::uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

        if (0 == deviceCount)
        {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }

        spdlog::debug("Detected {} devices", deviceCount);

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        std::multimap<int, VkPhysicalDevice> candidates{};
        std::transform(
            devices.begin(),
            devices.end(),
            std::inserter(candidates, candidates.begin()),
            [](const auto& device) { return std::make_pair(rateDeviceSuitability(device), device); });

        if (candidates.empty())
        {
            throw std::runtime_error("failed to find a suitable GPU!");
        }

        spdlog::debug("Device choosen with score: {}", candidates.rbegin()->first);
        physicalDevice = candidates.rbegin()->second;
    }

    struct QueueFaimilyIndices
    {
        std::optional<std::uint32_t> graphicsFamily;

        [[nodiscard]] auto isComplete() const -> bool
        {
            return graphicsFamily.has_value();
        }
    };

    static auto findQueueFamilies(VkPhysicalDevice device)
    {
        QueueFaimilyIndices indices{};

        std::uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        const auto it = std::ranges::find_if(queueFamilies, [](const auto& queueFamily) {
            return queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT;
        });

        if (it != queueFamilies.end())
        {
            indices.graphicsFamily = std::distance(queueFamilies.begin(), it);
        }

        return indices;
    }

    static auto rateDeviceSuitability(const auto& device) -> int
    {
        // get device properties
        VkPhysicalDeviceProperties deviceProperties{};
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        spdlog::debug("Device GPU {} of type: {}, max image dimension 2d: {}",
                      deviceProperties.deviceName,
                      deviceProperties.deviceType,
                      deviceProperties.limits.maxImageDimension2D);

        // get device feature
        VkPhysicalDeviceFeatures deviceFeatures{};
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
        spdlog::debug("Device GPU {} support geometry shader: {}",
                      deviceProperties.deviceName,
                      deviceFeatures.geometryShader);

        // Application can't function without geomtry shaders
        if (!deviceFeatures.geometryShader)
        {
            return 0;
        }

        const auto queueFamilyIndices = findQueueFamilies(device);
        spdlog::debug("Device GPU {} support graphics queue: {}",
                      deviceProperties.deviceName,
                      queueFamilyIndices.isComplete());
        if (!queueFamilyIndices.isComplete())
        {
            return 0;
        }

        // -----

        auto score = 0;

        // Discrete GPUs have a significant performance advantage
        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            constexpr auto discrete_gpu_score = 1000;
            score += discrete_gpu_score;
        }

        // Maximum possible size of textures affects graphics quality
        score += deviceProperties.limits.maxImageDimension2D;

        spdlog::debug("Device GPU {} got score: {}", deviceProperties.deviceName, score);

        return score;
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
            getRequiredByGlfwVulkanExtensions(createInfo, glfwExtensions);
        }

        const std::vector<char const*> required_validation_layer_names = {"VK_LAYER_KHRONOS_validation"};
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        // configure validation layers
        {

            if constexpr (enableValidationLayers)
            {

                configureValidationLayers(createInfo, required_validation_layer_names, debugCreateInfo);
            }
            else
            {
                configureValidationLayers(createInfo);
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

    auto getRequiredByGlfwVulkanExtensions(auto& createInfo, const auto& glfwExtensions) -> void
    {
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

    static auto configureValidationLayers(auto& createInfo,
                                          const auto& required_validation_layer_names,
                                          auto& debugCreateInfo) -> void
    {
        const auto required_validation_layers = utility::check_required_validation_layers(
            required_validation_layer_names.size(), required_validation_layer_names.data());

        required_validation_layers.log_properties();
        if (!required_validation_layers.all_supported())

        {
            throw std::runtime_error("validation layers requested, but not available!");
        }
        createInfo.enabledLayerCount = static_cast<std::uint32_t>(required_validation_layer_names.size());
        createInfo.ppEnabledLayerNames = required_validation_layer_names.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = &debugCreateInfo;
    }

    static auto configureValidationLayers(auto& createInfo) -> void
    {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    auto mainLoop() -> void
    {
        spdlog::info("Start loop");
        while (1 != glfwWindowShouldClose(window))
        {
            glfwPollEvents();
        }
        spdlog::info("Loop finished");
    }

    auto cleanup() -> void
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
    VkPhysicalDevice physicalDevice{VK_NULL_HANDLE};
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
