cmake_minimum_required(VERSION 3.16)
project(GameMakerCpp)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

set(CMAKE_CXX_STANDARD 17)

# This assumes the SDL source is available in vendored/SDL
add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)

add_subdirectory(native)
add_subdirectory(runtime)
# VM Test executable
add_executable(vm_test native/src/VM_Test.cpp native/src/VM_Value.cpp native/src/VM_Executor.cpp)
target_include_directories(vm_test PUBLIC ${CMAKE_SOURCE_DIR}/native/include)