#!/usr/bin/env bash
# Build SubConverter-Extended on a glibc Linux host (no Docker).
# Produces build/subconverter and bridge/libmihomo.so.
set -euo pipefail

ROOT="$(CDPATH= cd -- "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
THREADS="${THREADS:-$(nproc 2>/dev/null || echo 4)}"
DEPS_DIR="${SCE_DEPS_DIR:-$ROOT/.deps}"
BUILD_DIR="${SCE_BUILD_DIR:-$ROOT/build}"
export CC="${CC:-gcc}"
export CXX="${CXX:-g++}"

mkdir -p "$DEPS_DIR" "$BUILD_DIR"
export PATH="/usr/lib/go-1.22/bin:/usr/local/go/bin:$PATH"
export GOTOOLCHAIN="${GOTOOLCHAIN:-auto}"
export GOPROXY="${GOPROXY:-https://proxy.golang.org,direct}"
export CGO_ENABLED=1

need_cmd() {
  command -v "$1" >/dev/null 2>&1 || {
    echo "missing command: $1" >&2
    exit 1
  }
}

need_cmd cmake
need_cmd ninja
need_cmd g++
need_cmd pkg-config
need_cmd go
need_cmd git
need_cmd make
need_cmd curl

pkg-config --exists libcurl || { echo "libcurl development files are required" >&2; exit 1; }
pkg-config --exists libpcre2-8 || { echo "libpcre2 development files are required" >&2; exit 1; }
pkg-config --exists yaml-cpp || { echo "yaml-cpp development files are required" >&2; exit 1; }

if ! echo '#include <rapidjson/document.h>' | g++ -x c++ -E - >/dev/null 2>&1; then
  echo "rapidjson headers are required (package rapidjson-dev)" >&2
  exit 1
fi

install_quickjs_from() {
  local src_dir="$1"
  sudo install -d /usr/lib/quickjs /usr/include/quickjs
  sudo install -m644 "$src_dir/libquickjs.a" /usr/lib/quickjs/
  sudo install -m644 "$src_dir/"*.h /usr/include/quickjs/
  if [[ -f "$DEPS_DIR/quickjspp.hpp" ]]; then
    sudo install -m644 "$DEPS_DIR/quickjspp.hpp" /usr/include/quickjspp.hpp
  elif [[ -f "$ROOT/include/quickjspp.hpp" ]]; then
    sudo install -m644 "$ROOT/include/quickjspp.hpp" /usr/include/quickjspp.hpp
  fi
}

build_quickjs() {
  if [[ -f /usr/lib/quickjs/libquickjs.a && -f /usr/include/quickjs/quickjs.h ]]; then
    echo "==> quickjs already installed"
    return
  fi
  if [[ -f "$DEPS_DIR/quickjs/libquickjs.a" ]]; then
    echo "==> installing cached quickjs"
    install_quickjs_from "$DEPS_DIR/quickjs"
    return
  fi

  local existing
  existing="$(find "$DEPS_DIR/quickjspp-src" -name 'libquickjs.a' 2>/dev/null | head -n1 || true)"
  if [[ -n "$existing" ]]; then
    echo "==> reusing previously built quickjs"
    mkdir -p "$DEPS_DIR/quickjs"
    cp "$existing" "$DEPS_DIR/quickjs/"
    cp "$DEPS_DIR/quickjspp-src/quickjs/quickjs.h" \
       "$DEPS_DIR/quickjspp-src/quickjs/quickjs-libc.h" "$DEPS_DIR/quickjs/"
    cp "$DEPS_DIR/quickjspp-src/quickjspp.hpp" "$DEPS_DIR/quickjspp.hpp"
    install_quickjs_from "$DEPS_DIR/quickjs"
    return
  fi

  echo "==> building quickjs"
  rm -rf "$DEPS_DIR/quickjspp-src"
  git clone --depth=1 --recurse-submodules --shallow-submodules \
    https://github.com/ftk/quickjspp.git "$DEPS_DIR/quickjspp-src"
  cmake -S "$DEPS_DIR/quickjspp-src" -B "$DEPS_DIR/quickjspp-src/build" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_C_COMPILER="$CC" \
    -DCMAKE_CXX_COMPILER="$CXX"
  cmake --build "$DEPS_DIR/quickjspp-src/build" --target quickjs -j "$THREADS"
  mkdir -p "$DEPS_DIR/quickjs"
  archive="$(find "$DEPS_DIR/quickjspp-src" -name 'libquickjs.a' | head -n1)"
  [[ -n "$archive" ]] || { echo "libquickjs.a not produced" >&2; exit 1; }
  cp "$archive" "$DEPS_DIR/quickjs/"
  cp "$DEPS_DIR/quickjspp-src/quickjs/quickjs.h" \
     "$DEPS_DIR/quickjspp-src/quickjs/quickjs-libc.h" "$DEPS_DIR/quickjs/"
  cp "$DEPS_DIR/quickjspp-src/quickjspp.hpp" "$DEPS_DIR/quickjspp.hpp"
  install_quickjs_from "$DEPS_DIR/quickjs"
}

build_libcron() {
  if [[ -f /usr/lib/liblibcron.a || -f /usr/local/lib/liblibcron.a ]]; then
    if [[ -f /usr/include/libcron/Cron.h || -f /usr/local/include/libcron/Cron.h || -f "$ROOT/include/libcron/Cron.h" ]]; then
      echo "==> libcron already installed"
      return
    fi
  fi
  echo "==> building libcron"
  rm -rf "$DEPS_DIR/libcron-src"
  git clone --depth=1 --branch v1.3.0 --recurse-submodules --shallow-submodules \
    https://github.com/PerMalmberg/libcron.git "$DEPS_DIR/libcron-src"
  # Upstream always adds the test target; drop it so a host without the test
  # dependencies can still produce liblibcron.a.
  sed -i '/add_subdirectory(test)/d' "$DEPS_DIR/libcron-src/CMakeLists.txt" || true
  sed -i '/add_dependencies(cron_test libcron)/d' "$DEPS_DIR/libcron-src/CMakeLists.txt" || true
  cmake -S "$DEPS_DIR/libcron-src" -B "$DEPS_DIR/libcron-src/build" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_C_COMPILER="$CC" \
    -DCMAKE_CXX_COMPILER="$CXX"
  cmake --build "$DEPS_DIR/libcron-src/build" --target libcron -j "$THREADS"
  archive="$(find "$DEPS_DIR/libcron-src" -name 'liblibcron.a' | head -n1)"
  [[ -n "$archive" ]] || { echo "liblibcron.a not produced" >&2; exit 1; }
  sudo install -m644 "$archive" /usr/lib/liblibcron.a
  sudo install -d /usr/include/libcron /usr/include/date
  sudo install -m644 "$DEPS_DIR/libcron-src/libcron/include/libcron/"* /usr/include/libcron/
  if [[ -d "$DEPS_DIR/libcron-src/externals/date/include/date" ]]; then
    sudo install -m644 "$DEPS_DIR/libcron-src/externals/date/include/date/"* /usr/include/date/
  fi
}

build_bridge() {
  echo "==> building Mihomo CGO bridge"
  cd "$ROOT/bridge"
  go mod download
  export GOMODCACHE="${GOMODCACHE:-$(go env GOMODCACHE)}"
  go run ../scripts/generate_schemes.go ../src/parser/mihomo_schemes.h
  go run ../scripts/generate_param_compat.go -o ../src/parser/param_compat.h
  CGO_ENABLED=1 go build -trimpath -buildmode=c-shared -o libmihomo.so .
  test -f libmihomo.so
  test -f libmihomo.h
}

build_cpp() {
  echo "==> configuring C++ build"
  cmake -S "$ROOT" -B "$BUILD_DIR" \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    -DCMAKE_C_COMPILER="$CC" \
    -DCMAKE_CXX_COMPILER="$CXX" \
    -DCMAKE_BUILD_RPATH="$ROOT/bridge" \
    -G Ninja
  echo "==> compiling subconverter"
  cmake --build "$BUILD_DIR" -j "$THREADS"
  test -x "$BUILD_DIR/subconverter"
  cp -f "$ROOT/bridge/libmihomo.so" "$BUILD_DIR/libmihomo.so"
  echo "==> binary: $BUILD_DIR/subconverter"
  if command -v file >/dev/null 2>&1; then
    file "$BUILD_DIR/subconverter"
  fi
  if command -v ldd >/dev/null 2>&1; then
    ldd "$BUILD_DIR/subconverter" | sed -n '1,40p'
  fi
}

build_quickjs
build_libcron
build_bridge
build_cpp
echo "==> local Linux build finished"
echo "==> run with: LD_LIBRARY_PATH=$ROOT/bridge $BUILD_DIR/subconverter"
