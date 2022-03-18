cmake_minimum_required(VERSION 3.20)
project(filament_manager CXX)

set(CMAKE_CXX_STANDARD 20)

include(./ucm.cmake)
include(./conan.cmake)

ucm_set_runtime(STATIC)
if (WIN32 OR MINGW)
    set(CMAKE_SYSTEM_NAME Windows)
    set(CMAKE_SYSTEM_PROCESSOR x86_64)
    set(CONAN_ARCHITECTURE x86_64)
    set(options Poco:enable_netssl_win=True Poco:enable_netssl=False)

    conan_cmake_run(
            BASIC_SETUP
            ARCH ${CONAN_ARCHITECTURE}
            ENV CC=${CMAKE_C_COMPILER}
            ENV CXX=${CMAKE_CXX_COMPILER}
            ENV CFLAGS=${CMAKE_C_FLAGS}
            ENV CXXFLAGS=${CMAKE_CXX_FLAGS}
            SETTINGS os=Windows
            SETTINGS compiler.cppstd=20
    )
endif ()

set(wxUSE_STL ON)
set(wxBUILD_SHARED OFF)
add_subdirectory(libs/wxWidgets)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

conan_cmake_configure(
        REQUIRES libpqxx/7.7.0
        GENERATORS cmake_find_package
)

conan_cmake_autodetect(settings)

conan_cmake_install(PATH_OR_REFERENCE .
        BUILD missing
        REMOTE conancenter
        SETTINGS ${settings}
        SETTINGS ${CONAN_SETTINGS}
        OPTIONS Poco:enable_netssl_win=True Poco:enable_netssl=False)

find_package(libpqxx)

add_executable(filament_manager ${target} main.cpp MainApp.h MainApp.cpp MainWindow.cpp MainWindow.h)
target_link_libraries(filament_manager PRIVATE libpqxx::libpqxx wx::core wx::base)
