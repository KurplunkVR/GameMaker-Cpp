# GameMaker-Cpp

A C++ port of the GameMaker HTML5 runtime with abstracted I/O for graphics, audio, input, and file operations. Designed for portability to desktop, mobile, and consoles.

Yes, Generative AI helped port the engine. Yes, it was all checked over by an actual programmer (ME). And, yes it did make tons of mistakes, they are all fixed though.

## Build Instructions

### Prerequisites
- CMake >= 3.15
- SDL2 development libraries
- C++17 compiler

### Build
```bash
mkdir build
cmake -S . -B build
cmake --build build --config Release
```

## Project Structure
- `native/` — Platform abstraction layer (SDL2, OpenGL, etc.)
- `runtime/` — GameMaker runtime logic (to be ported from JS)
- `thirdparty/` — External libraries (Box2D, etc.)

## Next Steps
- Implement Audio, FileIO, Timer interfaces
- Begin porting runtime logic from JS to C++
- Add test harness for basic window/input/audio
