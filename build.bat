
call "C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/VC/Auxiliary/Build/vcvarsall.bat" %1

if "%1" == "x86" "C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/VC/Tools/MSVC/14.10.25017/bin/HostX86/x86/cl.exe" -ZI /Fe:exercise.exe *.cpp
if "%1" == "x64" "C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/VC/Tools/MSVC/14.10.25017/bin/HostX64/x64/cl.exe" -ZI /Fe:exercise.exe *.cpp
