#include "graphics.hpp"

#include "utility/logger.hpp"
#include "vulkan/vulkan.hpp"

namespace gfx
{
std::unique_ptr<Graphics> Graphics::create(Api api)
{
    switch (api)
    {
    case Api::DirectX:
        [[fallthrough]];
    case Api::OpenGL:
        log::warn("Graphics library {} is not supported yet, fallback to Vulkan", static_cast<char>(api));
        [[fallthrough]];
    case Api::Vulkan:
        log::info("Creating Vulkan graphics library");
        return std::make_unique<vk::Vulkan>();
    }
}
} // namespace gfx