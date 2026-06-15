#pragma once

#include <utility>
#include <vulkan/vulkan_core.h>

namespace vk
{

[[nodiscard]] auto createVulkanInstance() -> VkInstance;
[[nodiscard]] auto setupDebugMessenger(VkInstance instance) -> VkDebugUtilsMessengerEXT;
[[nodiscard]] auto pickPhysicalDevice(VkInstance instance) -> std::pair<VkPhysicalDevice, std::uint32_t>;
[[nodiscard]] auto createLogicalDevice(VkPhysicalDevice physicalDevice, std::uint32_t queueFamilyIndex) -> VkDevice;

} // namespace vk