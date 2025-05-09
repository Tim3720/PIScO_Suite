cmake_minimum_required(VERSION 3.10)
project(GenerateCrops)

set(CMAKE_CXX_STANDARD 23) # Specify C++ standard version
set(CMAKE_CXX_STANDARD_REQUIRED ON) # Ensure the chosen version is used

# Variable for the project name
set(PROJECT_NAME "GenerateCrops")

set(BUILDING_DIR "${CMAKE_SOURCE_DIR}/binDebug")

# Add OpenCV support

find_package(OpenCV REQUIRED)

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

  set(CMAKE_CXX_FLAGS
      "${CMAKE_CXX_FLAGS} -fsanitize=undefined -fsanitize=address")

  set(THREAD_MANAGER_MODULE_DIR "../ThreadManagerModule/")
  # Check if ThreadManagerModule is available
  if(EXISTS ${THREAD_MANAGER_MODULE_DIR}/libDebug/libThreadManagerModule.a
     OR EXISTS ${THREAD_MANAGER_MODULE_DIR}/libDebug/libThreadManagerModule.so)
    message(STATUS "Found ThreadManagerModule at ${THREAD_MANAGER_MODULE_DIR}")
  else()
    message(
      FATAL_ERROR
        "ThreadManagerModule library not found in ${THREAD_MANAGER_MODULE_DIR}. Please check the installation."
    )
  endif()

  set(BUILDING_DIR "${CMAKE_SOURCE_DIR}/binDebug")
  include_directories(${THREAD_MANAGER_MODULE_DIR}/include)
  link_directories(${THREAD_MANAGER_MODULE_DIR}/libDebug)

else()
  message(
    STATUS
      "Configuring ${PROJECT_NAME} with C++ ${CMAKE_CXX_STANDARD} in release mode..."
  )
  set(CMAKE_CXX_FLAGS "-Wall")

  # Specify the directory where the ThreadManagerModule is installed
  set(THREAD_MANAGER_MODULE_DIR "../ThreadManagerModule/")
  # Check if ThreadManagerModule is available
  if(EXISTS ${THREAD_MANAGER_MODULE_DIR}/lib/libThreadManagerModule.a
     OR EXISTS ${THREAD_MANAGER_MODULE_DIR}/lib/libThreadManagerModule.so)
    message(STATUS "Found ThreadManagerModule at ${THREAD_MANAGER_MODULE_DIR}")
  else()
    message(
      FATAL_ERROR
        "ThreadManagerModule library not found in ${THREAD_MANAGER_MODULE_DIR}. Please check the installation."
    )
  endif()
  include_directories(${THREAD_MANAGER_MODULE_DIR}/include)
  link_directories(${THREAD_MANAGER_MODULE_DIR}/lib)

  set(BUILDING_DIR "${CMAKE_SOURCE_DIR}/bin")
endif()

# Automatically detect all source and header files
file(GLOB SRC_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp) # Collect all .cpp files in
                                                   # the src directory
file(GLOB HEADER_FILES ${CMAKE_SOURCE_DIR}/include/*.hpp) # Collect all .hpp
                                                          # files in the include
                                                          # directory

add_compile_definitions("DEBUG=$<CONFIG:Debug>")
add_executable(${PROJECT_NAME} ${SRC_FILES})

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} ThreadManagerModule)

# target_link_options(${PROJECT_NAME} BEFORE PUBLIC -fsanitize=undefined PUBLIC
# -fsanitize=address) Set the output directory for the binary to a 'bin' folder

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                                 ${BUILDING_DIR})
