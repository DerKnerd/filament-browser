FROM ubuntu:22.04

ENV CONAN_REVISIONS_ENABLED 1
ENV DEBIAN_FRONTEND noninteractive
RUN apt-get update && apt-get upgrade -y
RUN apt-get install mingw-w64-x86-64-dev g++-mingw-w64-x86-64-posix gcc-mingw-w64-x86-64-posix binutils-mingw-w64-x86-64 build-essential cmake git wget rsync -y
RUN wget https://github.com/conan-io/conan/releases/latest/download/conan-ubuntu-64.deb
RUN dpkg -i conan-ubuntu-64.deb
WORKDIR /filament-browser
ENTRYPOINT "/filament-browser/build-mingw.sh"