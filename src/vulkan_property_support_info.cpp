#include "vulkan_property_support_info.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iterator>
#include <span>
#include <spdlog/spdlog.h>
#include <vector>

namespace utility
{
namespace
{
const char* get_icon(const int id)
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
} // namespace

void RequiredVulkanProperties::log_properties() const
{
    spdlog::info("{} status:", property_type_name);
    for (const auto& [name, id] : extensions)
    {
        spdlog::info("\t {} {}", get_icon(id), name);
    }
}

RequiredVulkanProperties::RequiredVulkanProperties(std::string&& name,
                                                   SupportMap&& supported_extensions,
                                                   const std::uint32_t count,
                                                   const char* const* names)
    : property_type_name{std::move(name)}, extensions{std::move(supported_extensions)}
{
    for (auto i = 0; i < count; ++i)
    {
        if (-1 == --extensions[*std::next(names, i)])
        {
            all_required_extensions_supported = false;
        }
    }
}
bool RequiredVulkanProperties::all_supported() const
{
    return all_required_extensions_supported;
}

RequiredVulkanProperties check_glfw_required_extensions(const std::uint32_t count, const char* const* names)
{
    std::uint32_t extensionCount{0};
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> extension_properties(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extension_properties.data());

    SupportMap properties{};
    for (const auto& extension : extension_properties)
    {
        // the use of span is not needed here, however it is done to silent a warning about array decay
        properties[std::span<const char>{extension.extensionName}.data()] = 1;
    }
    return RequiredVulkanProperties("Extensions", std::move(properties), count, names);
}

RequiredVulkanProperties check_required_validation_layers(const std::uint32_t count, const char* const* names)
{
    std::uint32_t layerCount{0};
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    SupportMap properties{};
    for (const auto& layer : availableLayers)
    {
        // the use of span is not needed here, however it is done to silent a warning about array decay
        properties[std::span<const char>{layer.layerName}.data()] = 1;
    }
    return RequiredVulkanProperties("Layers", std::move(properties), count, names);
}
} // namespace utility
