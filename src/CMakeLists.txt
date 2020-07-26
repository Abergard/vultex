add_executable(vultex-exe
  main.cpp)

find_package(Vulkan REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)


target_link_libraries(vultex-exe
  PRIVATE 
  fmt::fmt-header-only spdlog::spdlog_header_only
  glfw glm Vulkan::Vulkan)
