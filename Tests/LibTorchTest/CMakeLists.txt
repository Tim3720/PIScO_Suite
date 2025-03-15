cmake_minimum_required(VERSION 3.18 FATAL_ERROR)
project(LibtorchText)
set(PROJECT_NAME LibtorchText)

find_package(OpenCV REQUIRED)

set(CMAKE_PREFIX_PATH
    "/nix/store/sg65f6n7yxqpx45743a6jcl511lrhg40-libtorch-2.6.0-dev")

find_package(Torch REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

file(GLOB SRC_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp)
file(GLOB HEADER_FILES ${CMAKE_SOURCE_DIR}/include/*.hpp)

add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} "${TORCH_LIBRARIES}" "${OpenCV_LIBS}")
# target_precompile_headers(${PROJECT_NAME} PUBLIC torch_includes.h)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
