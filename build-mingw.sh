#!/usr/bin/env bash
mkdir -p /build
rsync -a /filament-browser /build/
cmake -DCMAKE_TOOLCHAIN_FILE=/build/filament-browser/mingw-w64-x86_64.cmake -B /build/filament-browser/build -S /build/filament-browser -DCMAKE_BUILD_TYPE=Release -DWIN32=1 -G "Unix Makefiles"
cmake --build /build/filament-browser/build --target filament_manager -- -j12
cp /build/filament-browser/build /out/ -r