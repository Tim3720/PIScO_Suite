cmake_minimum_required(VERSION 3.10)
project(Segmenter)

set(CMAKE_CXX_STANDARD 23) # Specify C++ standard version
set(CMAKE_CXX_STANDARD_REQUIRED ON) # Ensure the chosen version is used

# Variable for the project name
set(PROJECT_NAME "Segmenter")

set(ERROR_DATA_TYPES "../GlobalDataTypes/Error")
file(GLOB ERROR_DATA_TYPE_FILES ${ERROR_DATA_TYPES}/*.cpp)
include_directories(${ERROR_DATA_TYPES})

set(PARSER "../GlobalDataTypes/Parser")
file(GLOB PARSER_FILES ${PARSER}/*.cpp)
include_directories(${PARSER})

# Add OpenCV support
find_package(OpenCV REQUIRED)

# Add HDF5 support
find_package(HDF5 REQUIRED COMPONENTS C CXX)

# Check if OpenCV is found
if(OpenCV_FOUND)
  message(STATUS "OpenCV found at ${OpenCV_DIR}")
else()
  message(FATAL_ERROR "OpenCV not found. Please install OpenCV and try again.")
endif()

if(CMAKE_BUILD_TYPE MATCHES "Debug")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")
  # set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")
  message(
    STATUS
      "Configuring ${PROJECT_NAME} with C++ ${CMAKE_CXX_STANDARD} in debug mode..."
  )

  # Specify the directory where the ThreadManagerModule is installed

  set(CMAKE_CXX_FLAGS
      "${CMAKE_CXX_FLAGS} -fsanitize=undefined -fsanitize=address")

  set(BUILDING_DIR "${CMAKE_SOURCE_DIR}/binDebug")

else()
  message(
    STATUS
      "Configuring ${PROJECT_NAME} with C++ ${CMAKE_CXX_STANDARD} in release mode..."
  )
  set(CMAKE_CXX_FLAGS "-Wall")

  set(BUILDING_DIR "${CMAKE_SOURCE_DIR}/bin")
endif()

# Automatically detect all source and header files
file(GLOB SRC_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp) # Collect all .cpp files in
                                                   # the src directory
file(GLOB HEADER_FILES ${CMAKE_SOURCE_DIR}/include/*.hpp) # Collect all .hpp
                                                          # files in the include
                                                          # directory

add_compile_definitions("DEBUG=$<CONFIG:Debug>")
add_executable(${PROJECT_NAME} ${SRC_FILES} ${ERROR_DATA_TYPE_FILES}
                               ${PARSER_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE ${HDF5_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} ${HDF5_LIBRARIES})

# target_link_options(${PROJECT_NAME} BEFORE PUBLIC -fsanitize=undefined PUBLIC
# -fsanitize=address) Set the output directory for the binary to a 'bin' folder

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                                 ${BUILDING_DIR})
