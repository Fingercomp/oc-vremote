cmake_minimum_required(VERSION 2.8)
project(OC-VRemote)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(EXECUTABLE_NAME "vremote")

file(GLOB SRC_FILES
    "src/*.h"
    "src/*.cpp"
)

add_executable(${EXECUTABLE_NAME} ${SRC_FILES})

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake-modules" ${CMAKE_SOURCE_PATH})

find_package(SFML 2 REQUIRED system window graphics network)
if(SFML_FOUND)
  include_directories(${SFML_INCLUDE_DIR})
  target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES})
endif()

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package(Threads REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} ${CMAKE_THREAD_LIBS_INIT})

include_directories("${PROJECT_SOURCE_DIR}/src")

set_property(TARGET ${EXECUTABLE_NAME} PROPERTY CXX_STANDARD 14)
set(CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -W -Wall")

install(TARGETS ${EXECUTABLE_NAME} DESTINATION bin)
