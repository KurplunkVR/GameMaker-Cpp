add_library(native STATIC
    src/Platform_SDL.cpp
    src/GMLTypes.cpp
    src/Instance.cpp
    src/Object.cpp
    src/Room.cpp
    src/Managers.cpp
    src/GameEngine.cpp
    src/Sprite.cpp
    src/Graphics.cpp
    src/Audio.cpp
    src/Layer.cpp
    src/AssetLoader.cpp
    src/VM_Value.cpp
    src/VM_Executor.cpp
)

target_include_directories(native PUBLIC
    ${CMAKE_SOURCE_DIR}/native/include
    ${CMAKE_SOURCE_DIR}/vendored/SDL/include
    ${CMAKE_SOURCE_DIR}/vendored
)

link_directories(${CMAKE_SOURCE_DIR}/vendored/SDL/lib)

target_link_libraries(native PUBLIC SDL3::SDL3)