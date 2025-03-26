cmake_minimum_required(VERSION 3.8)
project(mpu6050_driver)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)
find_package(hardware_interface REQUIRED)
find_package(pluginlib REQUIRED)
find_package(rclcpp_lifecycle REQUIRED)
find_package(rclcpp REQUIRED)

#COMPILE
add_library(
  ${PROJECT_NAME} 
  SHARED
  src/mpu6050_hardware_interface.cpp
  src/mpu6050_lib.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
$<INSTALL_INTERFACE:include/mpu6050_hardware_interface>
)

ament_target_dependencies(
  ${PROJECT_NAME} PUBLIC
  hardware_interface
  pluginlib
  rclcpp
  rclcpp_lifecycle
)

# Link i2c library to mpu6050_hardware_interface target
target_link_libraries(${PROJECT_NAME} PUBLIC i2c)

# Export hardware plugin
pluginlib_export_plugin_description_file(hardware_interface mpu6050_hardware.xml)

# INSTALL
install(
  DIRECTORY include
  DESTINATION include
)

install(
  TARGETS ${PROJECT_NAME}
  EXPORT export_mpu6050_hardware_interface
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)


include_directories(include)

# Create Cpp executables
add_executable(mpu6050_offsets src/mpu6050_lib.cpp src/mpu6050_offsets.cpp)
add_executable(mpu6050_covariances src/mpu6050_lib.cpp src/mpu6050_covariances.cpp)
add_executable(test_mpu6050 src/mpu6050_lib.cpp src/test_mpu6050.cpp)

# Install Cpp executables
install(TARGETS
  mpu6050_offsets
  mpu6050_covariances
  test_mpu6050
  DESTINATION lib/${PROJECT_NAME}
)

# Link i2c to mpu6050_offsets and mpu6050_covariances targets
target_link_libraries(mpu6050_offsets i2c)
target_link_libraries(mpu6050_covariances i2c)
target_link_libraries(test_mpu6050 i2c)

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

ament_export_include_directories(
  include
)
# EXPORT
ament_export_libraries(${PROJECT_NAME})

ament_export_dependencies(
  hardware_interface
  pluginlib
  rclcpp
  rclcpp_lifecycle
)

ament_package()
