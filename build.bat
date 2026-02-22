call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat"
cd build
cmake --build . --config Release
cd Release
"runtime.exe"