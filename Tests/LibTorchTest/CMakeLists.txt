cmake_minimum_required(VERSION 3.10)
project(LibTorchTest)

set(CMAKE_CXX_STANDARD 23) # Specify C++ standard version
set(CMAKE_CXX_STANDARD_REQUIRED ON) # Ensure the chosen version is used

# Variable for the project name
set(PROJECT_NAME "LibTorchTest")

find_package(Torch REQUIRED)
find_package(OpenCV REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} "${TORCH_LIBRARIES}" "${OpenCV_LIBS}")
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 23)
