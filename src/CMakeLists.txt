add_executable(vultex-exe
  main.cpp)

# target_link_libraries()
find_package(Vulkan REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

target_link_libraries(vultex-exe
  PRIVATE glfw glm Vulkan::Vulkan)
