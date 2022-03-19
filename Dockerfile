FROM ubuntu:latest

RUN apt install mingw-w64 mingw-w64-x86-64-dev g++-mingw-w64-x86-64 gcc-mingw-w64-x86-64 binutils-mingw-w64-x86-64 build-essential cmake git wget -y
RUN git clone https://github.com/DerKnerd/filament-browser.git --recurse-submodules
RUN cd filament-browser
RUN cmake -DCMAKE_TOOLCHAIN_FILE=mingw-w64-x86_64.cmake -B build -S .
RUN cmake --build build/