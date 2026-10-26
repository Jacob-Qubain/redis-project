#!/usr/bin/env bash
# Build helper.  ./dev.sh build | test | run | bench | clean
#
# Why this exists: on this Mac the default compiler (/usr/bin/c++) can't link
# against the macOS 27 SDK yet, so we point CMake at Xcode's own compiler + SDK.
# Once you update Command Line Tools (System Settings > General > Software Update),
# plain `cmake -B build && cmake --build build` should work too.
set -euo pipefail
cd "$(dirname "$0")"

X=/Applications/Xcode.app/Contents/Developer
export CXX="$X/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang++"
SDK="$X/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk"

configure() {
  [ -f build/CMakeCache.txt ] || cmake -B build -DCMAKE_BUILD_TYPE=Debug -DSANITIZE=ON -DCMAKE_OSX_SYSROOT="$SDK"
}

case "${1:-test}" in
  build) configure; cmake --build build -j ;;
  test)  configure; cmake --build build -j && ./build/tests ;;
  run)   configure; cmake --build build -j && ./build/mini-redis ;;
  bench)
    # Separate optimized build: sanitizers and -O0 would distort timings.
    [ -f build-release/CMakeCache.txt ] || cmake -B build-release -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_SYSROOT="$SDK"
    cmake --build build-release -j --target bench && ./build-release/bench ;;
  clean) rm -rf build build-release ;;
  *) echo "usage: ./dev.sh build|test|run|bench|clean"; exit 1 ;;
esac
