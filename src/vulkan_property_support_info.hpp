#pragma once

#include <map>
#include <string>

namespace utility
{

using SupportMap = std::map<std::string, int>;

class RequiredVulkanProperties
{
public:
    RequiredVulkanProperties(std::string&& name,
                             SupportMap&& supported_extensions,
                             std::uint32_t count,
                             const char* const* names);
    [[nodiscard]] bool all_supported() const;
    void log_properties() const;

private:
    std::string property_type_name;
    SupportMap extensions{};
    bool all_required_extensions_supported{true};
};

RequiredVulkanProperties check_glfw_required_extensions(std::uint32_t count, const char* const* names);
RequiredVulkanProperties check_required_validation_layers(std::uint32_t count, const char* const* names);
} // namespace utility
