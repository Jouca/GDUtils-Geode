cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_BUILD_TYPE=RelWithDebInfo -B build -DGEODE_DISABLE_FMT_CONSTEVAL=1
cmake --build build --config RelWithDebInfo
xcopy /s /y "build\gdutilsdevs.gdutils.geode" "C:\Users\proga\Desktop\ClarifyGDPS 2.204 GEODE\geode\mods"