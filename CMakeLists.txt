cmake_minimum_required(VERSION 2.8.11)

project(Qt-Calc)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)

find_package(Python COMPONENTS Interpreter Development)

message("Python_FOUND:${Python_FOUND}")
message("Python_VERSION:${Python_VERSION}")
message("Python_Development_FOUND:${Python_Development_FOUND}")
message("Python_LIBRARIES:${Python_LIBRARIES}")
message("Python_INCLUDE_DIRS:${Python_INCLUDE_DIRS}")

if (UNIX)
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11")
endif(UNIX)

if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
endif(WIN32)

file(GLOB_RECURSE SRC src/cpp/*.cpp)
file(GLOB_RECURSE HDR src/cpp/*.hpp)
file(GLOB_RECURSE HDR_GUI src/cpp/gui/*.hpp)
file(GLOB_RECURSE FORMS src/forms/*.ui)

qt5_wrap_cpp(WRAP_CPP ${HDR_GUI})
qt5_wrap_ui(WRAP_UI ${FORMS}) # Seems to output into CMAKE_CURRENT_BINARY_DIR

include_directories(${CMAKE_CURRENT_BINARY_DIR}) # Include for generated ui header files
include_directories(src/cpp/)
include_directories(${Python_INCLUDE_DIRS}) # Python

add_executable(qt-calc ${SRC} ${WRAP_CPP} ${WRAP_UI})

target_link_libraries(qt-calc Qt5::Core Qt5::Widgets)
target_link_libraries(qt-calc ${Python_LIBRARIES}) # Python