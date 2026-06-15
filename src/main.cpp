// https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Base_code

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <format>
#include <iterator>
#include <map>
#include <optional>
#include <stdexcept>

#include "logger.hpp"
#include "vulkan_debug.hpp"
#include "vulkan_property_support_info.hpp"
#include "window.hpp"

namespace
{

struct QueueFaimilyIndices
{
    std::optional<std::uint32_t> graphicsFamily;

    [[nodiscard]] auto isComplete() const -> bool
    {
        return graphicsFamily.has_value();
    }
};

// 3 times, need to refactor
[[nodiscard]] auto findQueueFamilies(VkPhysicalDevice device) -> QueueFaimilyIndices
{
    QueueFaimilyIndices indices{};

    std::uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    const auto queueIt = std::ranges::find_if(
        queueFamilies,
        [](const auto& queueFamily)
        { return queueFamily.queueFlags & static_cast<std::uint32_t>(VK_QUEUE_GRAPHICS_BIT); });

    if (queueIt != queueFamilies.end())
    {
        indices.graphicsFamily = std::distance(queueFamilies.begin(), queueIt);
    }

    return indices;
}

[[nodiscard]] auto rateDeviceSuitability(const auto& device) -> std::int32_t
{
    // get device properties
    VkPhysicalDeviceProperties deviceProperties{};
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    log::info("Device GPU {} of type: {}, max image dimension 2d: {}",
              deviceProperties.deviceName,
              static_cast<int>(deviceProperties.deviceType),
              deviceProperties.limits.maxImageDimension2D);

    // get device feature
    VkPhysicalDeviceFeatures deviceFeatures{};
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
    log::info("Device GPU {} support geometry shader: {}",
              deviceProperties.deviceName,
              deviceFeatures.geometryShader);

    // Application can't function without geomtry shaders
    if (!deviceFeatures.geometryShader)
    {
        return 0;
    }

    const auto queueFamilyIndices = findQueueFamilies(device);
    log::info("Device GPU {} support graphics queue: {}",
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
    score += static_cast<int>(deviceProperties.limits.maxImageDimension2D);

    log::info("Device GPU {} got score: {}", deviceProperties.deviceName, score);

    return score;
}

auto configureValidationLayers(auto& createInfo,
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
    createInfo.enabledLayerCount = required_validation_layer_names.size();
    createInfo.ppEnabledLayerNames = required_validation_layer_names.data();

    populateDebugMessengerCreateInfo(debugCreateInfo);
    createInfo.pNext = &debugCreateInfo;
}

auto getRequiredExtensions(auto& createInfo, const auto& windowExtensions) -> void
{
    const auto window_required_extensions =
        utility::check_required_extensions(windowExtensions.size(), windowExtensions.data());

    log::info("EnabledExtensionCount: {}", windowExtensions.size());
    createInfo.enabledExtensionCount = windowExtensions.size();
    createInfo.ppEnabledExtensionNames = windowExtensions.data();

    window_required_extensions.log_properties();
    if (!window_required_extensions.all_supported())
    {
        throw std::runtime_error(std::format("Cannot create vulkan instance! window all supported: {}",
                                             window_required_extensions.all_supported()));
    }
}

[[nodiscard]] auto createInstance() -> VkInstance
{
    // fill an optional struct with application information
    VkApplicationInfo appInfo{.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                              .pApplicationName = "Hello vultex!",
                              .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
                              .pEngineName = "No Engine",
                              .engineVersion = VK_MAKE_VERSION(1, 0, 0),
                              .apiVersion = VK_API_VERSION_1_2};

    // global information about the entire program about extensions etc.
    VkInstanceCreateInfo createInfo{.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                                    .pApplicationInfo = &appInfo};

    const auto windowExtensions = ui::Window::getRequiredExtensions(ui::GraphicsLibrary::Vulkan);

    { // setup required extensions
        getRequiredExtensions(createInfo, windowExtensions);
    }

    const std::vector<char const*> required_validation_layer_names = {"VK_LAYER_KHRONOS_validation"};
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

    { // configure validation layers
        if constexpr (enableValidationLayers)
        {
            configureValidationLayers(createInfo, required_validation_layer_names, debugCreateInfo);
        }
    }

    VkInstance instance{nullptr};
    const auto create_instance_status = vkCreateInstance(&createInfo, nullptr, &instance);
    if (VK_SUCCESS != create_instance_status)
    {
        throw std::runtime_error{
            std::format("Cannot create vulkan instance: {}", static_cast<int>(create_instance_status))};
    }

    log::info("Instance created");
    return instance;
}

[[nodiscard]] auto setupDebugMessenger(auto* const instance) -> VkDebugUtilsMessengerEXT
{
    if constexpr (!enableValidationLayers)
    {
        return nullptr;
    }

    log::info("Initialize debug messenger");

    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    populateDebugMessengerCreateInfo(createInfo);

    VkDebugUtilsMessengerEXT debugMessenger{nullptr};
    if (VK_SUCCESS != CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger))
    {
        throw std::runtime_error("failed to set up debug messenger!");
    }
    return debugMessenger;
}

[[nodiscard]] auto pickPhysicalDevice(auto* const instance) -> VkPhysicalDevice
{
    std::uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if (0 == deviceCount)
    {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    log::info("Detected {} devices", deviceCount);

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    std::multimap<int, VkPhysicalDevice> candidates{};
    std::ranges::transform(devices,
                           std::inserter(candidates, candidates.begin()),
                           [](const auto& device)
                           { return std::make_pair(rateDeviceSuitability(device), device); });

    if (candidates.empty())
    {
        throw std::runtime_error("Cannot found any GPU device!");
    }
    if (candidates.rbegin()->first == 0)
    {
        throw std::runtime_error("Cannot found any suitable GPU!");
    }

    log::info("Device choosen with score: {}", candidates.rbegin()->first);
    auto* physicalDevice = candidates.rbegin()->second;
    return physicalDevice;
}

[[nodiscard]] auto createLogicalDevice(const auto physicalDevice) -> VkDevice
{

    auto indices = findQueueFamilies(physicalDevice);

    float queuePriority = 1.0F;
    VkDeviceQueueCreateInfo queueCreateInfo{.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                                            .queueFamilyIndex = indices.graphicsFamily.value(),
                                            .queueCount = 1,
                                            .pQueuePriorities = &queuePriority};

    VkPhysicalDeviceFeatures deviceFeatures{};

    // For older implementation there is a need to configure validation layers
    // as like for instance !
    VkDeviceCreateInfo createInfo{.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                                  .queueCreateInfoCount = 1,
                                  .pQueueCreateInfos = &queueCreateInfo,
                                  .pEnabledFeatures = &deviceFeatures};

    VkDevice logicalDevice{nullptr};
    if (VK_SUCCESS != vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice))
    {
        throw std::runtime_error("Failed to create logical device!");
    }

    return logicalDevice;
}
} // namespace

class HelloTrangleApplication
{
public:
    HelloTrangleApplication()
        : instance{createInstance()},
          debugMessenger{setupDebugMessenger(instance)},
          physicalDevice{pickPhysicalDevice(instance)},
          logicalDevice{createLogicalDevice(physicalDevice)}
    {
        const auto indices = findQueueFamilies(physicalDevice);
        constexpr auto firstQueueIndex = 0;

        vkGetDeviceQueue(logicalDevice, indices.graphicsFamily.value(), firstQueueIndex, &graphicsQueue);
    }

    HelloTrangleApplication(const HelloTrangleApplication&) = delete;
    HelloTrangleApplication(HelloTrangleApplication&&) = delete;
    HelloTrangleApplication& operator=(const HelloTrangleApplication&) = delete;
    HelloTrangleApplication& operator=(HelloTrangleApplication&&) = delete;

    ~HelloTrangleApplication()
    {
        log::info("Cleanup resources");

        vkDestroyDevice(logicalDevice, nullptr);

        if constexpr (enableValidationLayers)
        {
            DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        }

        vkDestroyInstance(instance, nullptr);
    }

    auto run()
    {
        window.loop();
    }

private:
    ui::Window window;
    VkInstance instance{nullptr};
    VkDebugUtilsMessengerEXT debugMessenger{nullptr};
    VkPhysicalDevice physicalDevice{VK_NULL_HANDLE};
    VkDevice logicalDevice{nullptr};
    VkQueue graphicsQueue{nullptr};
};

int main()
try
{
    logger::initSpdlog();

    HelloTrangleApplication{}.run();

    return EXIT_SUCCESS;
}
catch (const std::exception& e)
{
    log::error("{}", e.what());
    return EXIT_FAILURE;
}
