export GEODE_SDK=~/geodeiosreal 
export GEODE_BINDINGS_REPO_PATH=~/geodeiosbindings
cmake -B build-ios -DGEODE_TARGET_PLATFORM=iOS \
    -DCMAKE_SYSTEM_NAME=iOS \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo
cd build-ios
gmake -j8
