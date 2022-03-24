FROM ubuntu:21.10

ENV CONAN_REVISIONS_ENABLED 1
ENV DEBIAN_FRONTEND noninteractive
RUN apt-get update && apt-get upgrade -y
RUN apt-get install mingw-w64-x86-64-dev g++-mingw-w64-x86-64-posix gcc-mingw-w64-x86-64-posix binutils-mingw-w64-x86-64 build-essential cmake git wget -y
RUN wget https://github.com/conan-io/conan/releases/latest/download/conan-ubuntu-64.deb
RUN dpkg -i conan-ubuntu-64.deb
ADD . /filament-browser
WORKDIR /filament-browser
RUN rm -rf /filament-browser/libs
RUN git submodule update --init --recursive
RUN cmake -DCMAKE_TOOLCHAIN_FILE=/filament-browser/mingw-w64-x86_64.cmake -B build -S /filament-browser -DCMAKE_BUILD_TYPE=Release -DWIN32=1 -G "Unix Makefiles"
RUN cmake --build build/ --target filament_manager