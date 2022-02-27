cmake_minimum_required(VERSION 3.21)
project(filament_manager CXX)

set(CMAKE_CXX_STANDARD 23)

include(./ucm.cmake)

ucm_set_runtime(STATIC)
if (WIN32 OR MINGW)
    set(target WIN32 configuration.h)
    set(options Poco:enable_netssl_win=True Poco:enable_netssl=False)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
    set(CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_STATIC_LIBRARY_SUFFIX})
    set(BUILD_SHARED_LIBS false)
    set(CONAN_SETTINGS compiler.runtime=MT)
endif ()

# Request the required wxWidgets libs
find_package(wxWidgets 3.0 REQUIRED)
# Include the wxWidgets use file to initialize various settings
include(${wxWidgets_USE_FILE})

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

include(./conan.cmake)

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
        OPTIONS ${options})

find_package(libpqxx)

add_executable(filament_manager ${target} main.cpp MainApp.h MainApp.cpp MainWindow.cpp MainWindow.h)
target_include_directories(filament_manager PRIVATE ${wxWidgets_INCLUDE_DIRS})
target_link_libraries(filament_manager PRIVATE libpqxx::libpqxx ${wxWidgets_LIBRARIES})
