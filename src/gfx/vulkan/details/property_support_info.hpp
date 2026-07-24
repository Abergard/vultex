#pragma once

#include <map>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace gfx::vk::details
{

// using SupportMap = std::map<std::string, int>;

// class RequiredVulkanProperties
// {
// public:
//     // RequiredVulkanProperties(std::string&& name,
//     //                          SupportMap&& supported_extensions,
//     //                          const std::vector<const char*>& required_extensions);
//     [[nodiscard]] bool all_supported() const;
//     void log_properties() const;

//     // private:
//     // TODO: refactor below class to not use friend
//     // friend class VulkanPropertiesLogger;
//     std::string property_type_name;
//     SupportMap extensions{};
//     bool all_required_extensions_supported{true};
// };

class VulkanProperties
{
public:
    VulkanProperties();

    bool select_layers(const std::vector<const char*>& required_layers);
    bool select_extensions(const std::vector<const char*>& required_extensions);

    void log_properties() const;

private:
    enum class Availability
    {
        Unavailable = 0,
        Available,
        Used
    };

    using PropertyName = std::string;
    using Availabilities = std::map<PropertyName, Availability>;

    bool select_properties(const std::vector<const char*>& required_properties,
                           Availabilities& property_availabilities);

    std::string get_icon(Availability id) const;

    Availabilities layers{};
    std::vector<VkLayerProperties> availableLayers;

    Availabilities extensions{};
    std::map<PropertyName, std::vector<VkExtensionProperties>> layer_to_extensions{};
};

} // namespace gfx::vk::details
