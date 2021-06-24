add_executable(vultex-exe
  # utilities
  vulkan_debug.cpp
  vulkan_property_support_info.cpp
  # core
  main.cpp)

find_package(Vulkan 1.2.148 REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)


target_link_libraries(vultex-exe
  PRIVATE 
  fmt::fmt-header-only spdlog::spdlog_header_only
  glfw glm::glm Vulkan::Vulkan)

# set_property(TARGET vultex-exe PROPERTY CXX_STANDARD 20)
