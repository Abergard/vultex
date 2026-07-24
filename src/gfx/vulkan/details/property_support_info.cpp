#include "property_support_info.hpp"

#include <iterator>
#include <span>
#include <spdlog/spdlog.h>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "utility/logger.hpp"

namespace gfx::vk::details
{
std::string VulkanProperties::get_icon(const Availability id) const
{
    switch (id)
    {
    case Availability::Unavailable:
        return "[!]"; // required but not detected
    case Availability::Used:
        return "[x]"; // required and supported
    case Availability::Available:
        return "[ ]"; // not required but supported
    }
};

VulkanProperties::VulkanProperties()
{
    std::uint32_t layerCount{0};
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    availableLayers.resize(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    layers["VK_LAYER"] = Availability::Used;
    for (const auto& layer : availableLayers)
    {
        layers[layer.layerName] = Availability::Available;
    }

    std::uint32_t extensionCount{0};
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    {
        auto& availableExtensions = layer_to_extensions["VK_LAYER"];
        availableExtensions.resize(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());
        for (const auto& extension : availableExtensions)
        {
            extensions[extension.extensionName] = Availability::Available;
        }
    }

    // for (const auto& extension : availableExtensions)
    // {
    //     layer_to_extensions["VK_LAYER"].push_back(extension.extensionName);
    // }

    for (const auto& layer : availableLayers)
    {
        vkEnumerateInstanceExtensionProperties(layer.layerName, &extensionCount, nullptr);

        auto& availableExtensions = layer_to_extensions[layer.layerName];
        availableExtensions.resize(extensionCount);
        vkEnumerateInstanceExtensionProperties(layer.layerName, &extensionCount, availableExtensions.data());

        layer_to_extensions[layer.layerName].reserve(extensionCount);
        // for (const auto& extension : std::span(availableExtensions).subspan(previous_size, extensionCount))
        // {
        //     layer_to_extensions[layer.layerName].push_back(extension.extensionName);
        // }
        for (const auto& extension : availableExtensions)
        {
            extensions[extension.extensionName] = Availability::Available;
        }
    }
}

bool VulkanProperties::select_properties(const std::vector<const char*>& required_properties,
                                         Availabilities& property_availabilities)
{
    bool all_required_layers_supported{true};
    for (const auto& required : required_properties)
    {
        if (auto& layer = property_availabilities[std::string(required)]; Availability::Available == layer)
        {
            layer = Availability::Used;
        }
        else
        {
            all_required_layers_supported = false;
        }
    }
    return all_required_layers_supported;
}

bool VulkanProperties::select_layers(const std::vector<const char*>& required_layers)
{
    return select_properties(required_layers, layers);
}

bool VulkanProperties::select_extensions(const std::vector<const char*>& required_extensions)
{
    return select_properties(required_extensions, extensions);
}

void VulkanProperties::log_properties() const
{
    log::info("Available properties:");
    for (const auto& layer_properties : availableLayers)
    {
        const auto& layer_name = layer_properties.layerName;
        const auto& extension_properties = layer_to_extensions.at(layer_name);

        log::info("{:.<70} -> {:<40} | impl: v{} spec: v{}",
                  std::format("{} {}", get_icon(layers.at(layer_name)), layer_properties.description),
                  layer_properties.layerName,
                  layer_properties.implementationVersion,
                  layer_properties.specVersion);
        for (const auto& extension_property : extension_properties)
        {
            log::info("\t{}",
                      std::format("{} {} v{}",
                                  get_icon(extensions.at(extension_property.extensionName)),
                                  extension_property.extensionName,
                                  extension_property.specVersion));
        }
        if (not extension_properties.empty())
        {
            log::info("");
        }
    }
    log::info("{} Vulkan Internal Layer", get_icon(layers.at("VK_LAYER")));
    for (const auto& extension_property : layer_to_extensions.at("VK_LAYER"))
    {
        log::info("\t{}",
                  std::format("{} {} v{}",
                              get_icon(extensions.at(extension_property.extensionName)),
                              extension_property.extensionName,
                              extension_property.specVersion));
    }
}
} // namespace vk::details