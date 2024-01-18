echo off
set NDK_HOME="C:\Users\proga\AppData\Local\android-ndk-r26b"
set ANDROID_ABI=arm64-v8a
set MIN_SDK_VERSION=24
set NDK_ROOT="C:\Users\proga\AppData\Local\android-ndk-r26b"

cmake -DCMAKE_CXX_FLAGS="-fexceptions" -DCMAKE_TOOLCHAIN_FILE=%NDK_HOME%\build\cmake\android.toolchain.cmake -DANDROID_ABI=%ANDROID_ABI% -DANDROID_PLATFORM=android-%MIN_SDK_VERSION% -DCMAKE_BUILD_TYPE=Debug -DGEODE_DONT_BUILD_TEST_MODS=1 -G Ninja -B build-armv8

if %errorlevel% neq 0 (
    echo Configure failed
    exit /b 1
)

cmake --build build-armv8 --config Debug

if %errorlevel% neq 0 (
    echo Build failed
    exit /b 1
)

echo Built