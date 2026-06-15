find_package(Vulkan 1.2.148 REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(spdlog REQUIRED)

add_executable(vultex)

target_sources(vultex
  PRIVATE
  main.cpp
  window.cpp
  vulkan_debug.cpp
  vulkan_property_support_info.cpp
  logger.cpp)


target_link_libraries(vultex
  PRIVATE
  spdlog::spdlog_header_only
  glfw
  glm::glm
  Vulkan::Vulkan
)

# if(MSVC)
# else()
#   target_compile_options(vultex
#     PRIVATE -fmodules)
# endif()
