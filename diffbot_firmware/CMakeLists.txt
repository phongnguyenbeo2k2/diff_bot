cmake_minimum_required(VERSION 3.8)
project(diffbot_firmware)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(PkgConfig)
pkg_check_modules(SERIAL libserial)

#new: add hardware interface
find_package(hardware_interface REQUIRED)
find_package(pluginlib REQUIRED)
find_package(rclcpp_lifecycle REQUIRED)

add_library(${PROJECT_NAME} SHARED 
  src/diffbot_system.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE 
  include
  "${SERIAL_INCLUDE_DIRS}"
)

target_link_libraries(${PROJECT_NAME}
 "${SERIAL_LDFLAGS}")

 ament_target_dependencies(${PROJECT_NAME}
 hardware_interface
 pluginlib
 rclcpp
 rclcpp_lifecycle
)

pluginlib_export_plugin_description_file(hardware_interface diffbot_system.xml)

install(
  TARGETS ${PROJECT_NAME}
  DESTINATION lib
)

install(
  DIRECTORY include
  DESTINATION include
)

#old
add_executable(stm32_publisher src/stm32_publisher.cpp)
ament_target_dependencies(stm32_publisher rclcpp std_msgs)
target_include_directories(stm32_publisher PUBLIC include "${SERIAL_INCLUDE_DIRS}")
target_link_libraries(stm32_publisher "${SERIAL_LDFLAGS}")

add_executable(stm32_receiver src/stm32_receiver.cpp)
ament_target_dependencies(stm32_receiver rclcpp std_msgs)
target_include_directories(stm32_receiver PUBLIC include "${SERIAL_INCLUDE_DIRS}")
target_link_libraries(stm32_receiver "${SERIAL_LDFLAGS}")


install(TARGETS
  stm32_publisher
  stm32_receiver
  ARCHIVE DESTINATION lib 
  LIBRARY DESTINATION lib 
  RUNTIME DESTINATION bin
  DESTINATION lib/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

#new: add hardware interface
ament_export_include_directories(
  include
)

ament_export_libraries(
  ${PROJECT_NAME}
)

ament_export_dependencies(
  hardware_interface
  pluginlib
  rclcpp
  rclcpp_lifecycle
)

ament_package()
