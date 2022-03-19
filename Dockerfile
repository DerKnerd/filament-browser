FROM ubuntu:rolling

ENV DEBIAN_FRONTEND noninteractive
RUN apt-get update && apt-get upgrade -y
RUN apt-get install mingw-w64-x86-64-dev g++-mingw-w64-x86-64-posix gcc-mingw-w64-x86-64-posix binutils-mingw-w64-x86-64 build-essential cmake git wget -y
RUN wget https://github.com/conan-io/conan/releases/latest/download/conan-ubuntu-64.deb
RUN dpkg -i conan-ubuntu-64.deb
RUN git clone https://github.com/DerKnerd/filament-browser.git
WORKDIR /filament-browser
#RUN #git checkout e513b687d92de70bd83ad100b642032c8c44124c
RUN git submodule update --init --recursive
RUN cmake -DCMAKE_TOOLCHAIN_FILE=/filament-browser/mingw-w64-x86_64.cmake -B build -S /filament-browser -DCMAKE_BUILD_TYPE=Release -DMINGW=1
RUN cmake --build build/