cmake_minimum_required(VERSION 3.10)
project(TrackingBasedSegmenter)

set(CMAKE_CXX_STANDARD 23) # Specify C++ standard version
set(CMAKE_CXX_STANDARD_REQUIRED ON) # Ensure the chosen version is used

set(PROJECT_NAME "TBS")
set(BUILDING_DIR "${CMAKE_SOURCE_DIR}/binDebug")

# external modules
set(BG_CORRECTION_MODULE_DIR "../BackgroundCorrectionModule")
set(THREAD_MANAGER_MODULE_DIR "../ThreadManagerModule/")
find_package(OpenCV REQUIRED)

# Check if OpenCV is found
if(OpenCV_FOUND)
  message(STATUS "OpenCV found at ${OpenCV_DIR}")
else()
  message(FATAL_ERROR "OpenCV not found. Please install OpenCV and try again.")
endif()

# ##############################################################################
# Debug:
# ##############################################################################
if(CMAKE_BUILD_TYPE MATCHES "Debug")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -fsanitize=address")
  message(
    STATUS
      "Configuring ${PROJECT_NAME} with C++ ${CMAKE_CXX_STANDARD} in debug mode..."
  )

  # Check if BackgroundCorrectionModule is available
  if(EXISTS ${BG_CORRECTION_MODULE_DIR}/libDebug/libBackgroundCorrectionModule.a
     OR EXISTS
        ${BG_CORRECTION_MODULE_DIR}/libDebug/libBackgroundCorrectionModule.so)
    message(
      STATUS "Found BackgroundCorrectionModule at ${BG_CORRECTION_MODULE_DIR}")
  else()
    message(
      FATAL_ERROR
        "BackgroundCorrectionModule library not found in ${BG_CORRECTION_MODULE_DIR}. Please check the installation."
    )
  endif()

  # Specify the directory where the ThreadManagerModule is installed Check if
  # ThreadManagerModule is available
  if(EXISTS ${THREAD_MANAGER_MODULE_DIR}/libDebug/libThreadManagerModule.a
     OR EXISTS ${THREAD_MANAGER_MODULE_DIR}/libDebug/libThreadManagerModule.so)
    message(STATUS "Found ThreadManagerModule at ${THREAD_MANAGER_MODULE_DIR}")
  else()
    message(
      FATAL_ERROR
        "ThreadManagerModule library not found in ${THREAD_MANAGER_MODULE_DIR}. Please check the installation."
    )
  endif()

  include_directories(${BG_CORRECTION_MODULE_DIR}/include
                      ${THREAD_MANAGER_MODULE_DIR}/include)
  link_directories(${BG_CORRECTION_MODULE_DIR}/libDebug
                   ${THREAD_MANAGER_MODULE_DIR}/libDebug)

  set(CMAKE_CXX_FLAGS
      "${CMAKE_CXX_FLAGS} -fsanitize=undefined -fsanitize=address")

  set(BUILDING_DIR "${CMAKE_SOURCE_DIR}/binDebug")

  # ############################################################################
  # Release:
  # ############################################################################
else()
  message(
    STATUS
      "Configuring ${PROJECT_NAME} with C++ ${CMAKE_CXX_STANDARD} in release mode..."
  )
  set(CMAKE_CXX_FLAGS "-Wall")

  # Check if BackgroundCorrectionModule is available
  if(EXISTS ${BG_CORRECTION_MODULE_DIR}/lib/libBackgroundCorrectionModule.a
     OR EXISTS ${BG_CORRECTION_MODULE_DIR}/lib/libBackgroundCorrectionModule.so)
    message(
      STATUS "Found BackgroundCorrectionModule at ${BG_CORRECTION_MODULE_DIR}")
  else()
    message(
      FATAL_ERROR
        "BackgroundCorrectionModule library not found in ${BG_CORRECTION_MODULE_DIR}. Please check the installation."
    )
  endif()

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

  include_directories(${BG_CORRECTION_MODULE_DIR}/include
                      ${THREAD_MANAGER_MODULE_DIR}/include)
  link_directories(${BG_CORRECTION_MODULE_DIR}/lib
                   ${THREAD_MANAGER_MODULE_DIR}/lib)

  set(BUILDING_DIR "${CMAKE_SOURCE_DIR}/bin")
endif()

# Automatically detect all source and header files
file(GLOB SRC_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp) # Collect all .cpp files in
                                                   # the src directory

add_compile_definitions("DEBUG=$<CONFIG:Debug>")
add_executable(${PROJECT_NAME} ${SRC_FILES})

target_link_libraries(${PROJECT_NAME} BackgroundCorrectionModule
                      ThreadManagerModule ${OpenCV_LIBS})

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                                 ${BUILDING_DIR})
