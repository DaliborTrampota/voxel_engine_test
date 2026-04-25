#!/usr/bin/env bash
set -e

BUILD_DIR="build-linux"
BUILD_TYPE="Release"
WAYLAND=ON
X11=OFF
CC_COMPILER="gcc"
CXX_COMPILER="g++"

print_usage() {
    echo "Usage: ./build.sh [options]"
    echo ""
    echo "Options:"
    echo "  -d, --debug       Build in Debug mode (default: Release)"
    echo "  -x11              Use X11 instead of Wayland"
    echo "  --clang           Use Clang instead of GCC"
    echo "  --deps            Install system dependencies (apt)"
    echo "  --conf            Configure only, do not build"
    echo "  --cc              Configure with compile_commands.json export"
    echo "  -h, --help        Show this help"
}

CONFIGURE_ONLY=false
EXPORT_CC=false
INSTALL_DEPS=false

for arg in "$@"; do
    case $arg in
        -d|--debug)   BUILD_TYPE="Debug" ;;
        -x11)         WAYLAND=OFF; X11=ON ;;
        --clang)      CC_COMPILER="clang"; CXX_COMPILER="clang++" ;;
        --deps)       INSTALL_DEPS=true ;;
        --conf)       CONFIGURE_ONLY=true ;;
        --cc)         EXPORT_CC=true ;;
        -h|--help)    print_usage; exit 0 ;;
        *) echo "Unknown option: $arg"; print_usage; exit 1 ;;
    esac
done

if $INSTALL_DEPS; then
    echo "--- Installing system dependencies ---"
    sudo apt update
    sudo apt install -y build-essential cmake libwayland-dev libxkbcommon-dev wayland-protocols pkg-config
fi

mkdir -p "$BUILD_DIR"

CMAKE_ARGS=(
    -B "$BUILD_DIR"
    -DCMAKE_C_COMPILER="$CC_COMPILER"
    -DCMAKE_CXX_COMPILER="$CXX_COMPILER"
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE"
    -DGLFW_BUILD_WAYLAND="$WAYLAND"
    -DGLFW_BUILD_X11="$X11"
    -S .
)

if $EXPORT_CC; then
    CMAKE_ARGS+=(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)
fi

echo "--- Configuring ($BUILD_TYPE, CC=$CC_COMPILER) ---"
cmake "${CMAKE_ARGS[@]}"

if $EXPORT_CC; then
    cp "$BUILD_DIR/compile_commands.json" build/compile_commands.json 2>/dev/null || true
    echo "compile_commands.json copied to build/"
fi

if $CONFIGURE_ONLY; then
    echo "--- Configuration done (skipping build) ---"
    exit 0
fi

echo "--- Building ---"
cmake --build "$BUILD_DIR" --config "$BUILD_TYPE" -- -j"$(nproc)"

echo ""
echo "--- Done. Binary is in $BUILD_DIR/Stomata/$BUILD_TYPE/ ---"
