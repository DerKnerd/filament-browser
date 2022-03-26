cmake_minimum_required(VERSION 3.16)
project(filament_manager CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(./ucm.cmake)
include(./conan.cmake)

if (WIN32 OR MINGW)
    ucm_set_runtime(STATIC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    add_compile_definitions(__WXMSW__ _UNICODE)
    set(BUILD_SHARED_LIBS true)
    set(CMAKE_SYSTEM_NAME Windows)
    set(CMAKE_SYSTEM_PROCESSOR x86_64)
    set(CMAKE_WIN32_EXECUTABLE 1)
endif ()

set(wxUSE_STL ON)
set(wxBUILD_SHARED OFF)
set(wxUSE_AUI OFF)
set(wxUSE_RIBBON OFF)
set(wxUSE_RICHTEXT OFF)
set(wxUSE_XRC OFF)
set(wxUSE_ADV OFF)
set(wxUSE_OPENGL OFF)
set(wxUSE_PROPGRID OFF)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

conan_add_remote(NAME conan.imanuel.dev
        URL https://conan.imanuel.dev/artifactory/api/conan/conan-virtual)

conan_cmake_autodetect(settings)

if (MINGW)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    conan_cmake_configure(
            REQUIRES libpq/13.6
            GENERATORS cmake_find_package
            IMPORTS "bin, *.dll -> /out/bin"
            IMPORTS "lib, *.dll -> /out/bin"
            OPTIONS libq:shared=True
    )
    conan_cmake_install(PATH_OR_REFERENCE .
            BUILD missing
            REMOTE conan.imanuel.dev
            SETTINGS ${settings}
            PROFILE ${CMAKE_SOURCE_DIR}/linux_to_win64_conan)
else ()
    conan_cmake_configure(
            REQUIRES libpq/13.6
            GENERATORS cmake_find_package
    )
    conan_cmake_install(PATH_OR_REFERENCE .
            BUILD all
            REMOTE conan.imanuel.dev
            SETTINGS ${settings})
endif ()

find_package(PostgreSQL)
add_subdirectory(${CMAKE_SOURCE_DIR}/libs/wxWidgets)
add_subdirectory(${CMAKE_SOURCE_DIR}/libs/libpqxx)

add_executable(filament_manager ${target} main.rc main.cpp MainApp.h MainApp.cpp MainWindow.cpp MainWindow.h)
if (MINGW)
    target_link_libraries(filament_manager PRIVATE -static PostgreSQL::PostgreSQL pqxx wx::core wx::base gcc stdc++ winpthread)
else ()
    target_link_libraries(filament_manager PRIVATE PostgreSQL::PostgreSQL pqxx wx::core wx::base)
endif ()