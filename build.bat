cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_BUILD_TYPE=RelWithDebInfo -B build -DGEODE_DISABLE_FMT_CONSTEVAL=1
cmake --build build --config RelWithDebInfo