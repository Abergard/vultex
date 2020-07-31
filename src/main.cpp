// https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Base_code

#include <iterator>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdint>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <map>
#include <span>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <string_view>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

class VulkanExtensionSuppportInfo
{
public:
    VulkanExtensionSuppportInfo()
    {
        std::uint32_t extensionCount{0};
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> extension_properties(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extension_properties.data());

        for (const auto& extension : extension_properties)
        {
            // the use of span is not needed here, however it is done to silent a warning about array decay
            extensions[std::span<const char>{extension.extensionName}.data()] = 1;
        }
    }

    bool mark_glfw_required_extensions(const std::uint32_t count, const char** names)
    {
        auto all_supported{true};
        for (auto i = 0; i < count; ++i)
        {
            if (-1 == --extensions[*std::next(names, i)])
            {
                all_supported = false;
            }
        }
        return all_supported;
    }

    void log_extension_list()
    {
        spdlog::info("Extensions status:");
        for (const auto& [name, id] : extensions)
        {
            spdlog::info("\t {} {}", get_icon(id), name);
        }
    }

private:
    static const char* get_icon(const int id)
    {
        switch (id)
        {
        case -1:
            return " ! ";
        case 0:
            return "[x]";
        case 1:
            return "[ ]";
        default:
            return " ? ";
        }
    };

    using ExtensionMap = std::map<std::string, int>;
    ExtensionMap extensions{};
};

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

        // prepare extensions

        VulkanExtensionSuppportInfo extensionSupportInfo{};

        // get vulkan extensions required by GLFW
        std::uint32_t glfwExtensionCount = 0;
        const auto** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        // mark extensions requried by glfw
        const auto allGlfwExtensionsSupported =
            extensionSupportInfo.mark_glfw_required_extensions(glfwExtensionCount, glfwExtensions);

        spdlog::info("EnabledExtensionCount: {}", glfwExtensionCount);
        createInfo.enabledExtensionCount = glfwExtensionCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;
        createInfo.enabledLayerCount = 0;

        extensionSupportInfo.log_extension_list();

        if (!allGlfwExtensionsSupported || VK_SUCCESS != vkCreateInstance(&createInfo, nullptr, &instance))
        {
            spdlog::error("Cannot create vulkan instance!");
            throw std::runtime_error{"Cannot create vulkan instance!"};
        }
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
        vkDestroyInstance(instance, nullptr);
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    VkInstance instance{nullptr};
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
