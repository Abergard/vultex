#include "logger.hpp"

#include <spdlog/sinks/ansicolor_sink.h>

namespace utility
{
void initSpdlog()
{
    // Forcing always coloring, because it doesn't work in some environments, e.g. Emacs shell in Windows
    auto sink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>();
    sink->set_color_mode(spdlog::color_mode::always);
    auto logger = std::make_shared<spdlog::logger>("vultex", sink);
    spdlog::set_default_logger(logger);
    spdlog::set_level(spdlog::level::info);
}
} // namespace utility