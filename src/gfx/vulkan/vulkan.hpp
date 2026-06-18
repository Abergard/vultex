#pragma once

#include <vulkan/vulkan_core.h>

#include "gfx/graphics.hpp"

namespace gfx::vk
{

class Vulkan final : public ::gfx::Graphics
{
public:
    Vulkan();
    ~Vulkan() noexcept override;

private:
    VkInstance instance{nullptr};
    VkDebugUtilsMessengerEXT debugMessenger{nullptr};

    VkDevice logicalDevice{nullptr};
    VkQueue graphicsQueue{nullptr};
};

} // namespace gfx::vk