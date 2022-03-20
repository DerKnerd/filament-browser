cmake_minimum_required(VERSION 3.16)
project(filament_manager CXX)


set(CMAKE_CXX_STANDARD 20)

include(./ucm.cmake)
include(./conan.cmake)

ucm_set_runtime(STATIC)
if (WIN32 OR MINGW)
    add_compile_definitions(__WXMSW__ _UNICODE)
    set(BUILD_SHARED_LIBS false)
    set(CMAKE_SYSTEM_NAME Windows)
    set(CMAKE_SYSTEM_PROCESSOR x86_64)
    set(CMAKE_WIN32_EXECUTABLE 1)
endif ()

set(wxUSE_STL ON)
set(wxBUILD_SHARED OFF)
add_subdirectory(libs/wxWidgets)
add_subdirectory(libs/libpqxx)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

conan_add_remote(NAME conan.imanuel.dev
        URL https://conan.imanuel.dev/artifactory/api/conan/conan-virtual)

conan_cmake_configure(
        REQUIRES libpq/12.2
        GENERATORS cmake_find_package
)

conan_cmake_autodetect(settings)

if (MINGW)
    conan_cmake_install(PATH_OR_REFERENCE .
            BUILD missing
            REMOTE conan.imanuel.dev
            SETTINGS ${settings}
            PROFILE ${CMAKE_SOURCE_DIR}/linux_to_win64_conan)
else ()
    conan_cmake_install(PATH_OR_REFERENCE .
            BUILD missing
            REMOTE conan.imanuel.dev
            SETTINGS ${settings})
endif ()

find_package(pqxx)

add_executable(filament_manager ${target} main.cpp MainApp.h MainApp.cpp MainWindow.cpp MainWindow.h)
target_link_libraries(filament_manager PRIVATE pqxx wx::core wx::base)
