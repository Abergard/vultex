#pragma once

#include <memory>

#include "api.hpp"

namespace gfx
{

class Graphics
{
public:
    virtual ~Graphics() noexcept = default;

    [[nodiscard]] static std::unique_ptr<Graphics> create(Api api);
};

} // namespace gfx