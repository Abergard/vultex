#pragma once

#include <utility>
#include <vulkan/vulkan_core.h>

namespace vk
{

[[nodiscard]] auto createVulkanInstance() -> VkInstance;
[[nodiscard]] auto setupDebugMessenger(VkInstance) -> VkDebugUtilsMessengerEXT;
auto destroyDebugMessenger(VkInstance, VkDebugUtilsMessengerEXT) -> void;
[[nodiscard]] auto pickPhysicalDevice(VkInstance) -> std::pair<VkPhysicalDevice, std::uint32_t>;
[[nodiscard]] auto createLogicalDevice(VkPhysicalDevice, std::uint32_t queueFamilyIndex) -> VkDevice;

} // namespace vk