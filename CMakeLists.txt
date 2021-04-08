cmake_minimum_required(VERSION 2.8.11)

project(QCalc)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)

find_package(Python COMPONENTS Interpreter Development)
message("Python_FOUND:${Python_FOUND}")
message("Python_VERSION:${Python_VERSION}")
message("Python_Development_FOUND:${Python_Development_FOUND}")
message("Python_LIBRARIES:${Python_LIBRARIES}")
message("Python_INCLUDE_DIRS:${Python_INCLUDE_DIRS}")

if (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif (UNIX)

if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
endif (WIN32)

file(GLOB_RECURSE SRC src/cpp/*.cpp)
file(GLOB_RECURSE FORMS src/forms/*.ui)

file(GLOB ADDON_SRC src/python/*.py)
file(GLOB ADDON_META src/python/*.json)
set(SYSTEM_SRC src/python/qcalc)

file(COPY ${ADDON_SRC} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/addon) # Copy addon scripts
file(COPY ${ADDON_META} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/addon) # Copy addon metadata
file(COPY ${SYSTEM_SRC} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/system) # Copy system scripts

set(HDR_GUI src/cpp/gui/mainwindow.hpp
        src/cpp/gui/settingsdialog.hpp
        src/cpp/gui/addontesterdialog.hpp
        src/cpp/gui/widgets/addonitemwidget.hpp
        src/cpp/gui/widgets/historyitemwidget.hpp
        src/cpp/gui/widgets/historywidget.hpp
        src/cpp/gui/widgets/functionseditor.hpp
        src/cpp/gui/widgets/namedvalueeditor.hpp
        src/cpp/gui/widgets/scriptseditor.hpp
        src/cpp/gui/widgets/symbolseditor.hpp)

qt5_wrap_cpp(WRAP_CPP ${HDR_GUI})
qt5_wrap_ui(WRAP_UI ${FORMS}) # Seems to output into CMAKE_CURRENT_BINARY_DIR

include_directories(${CMAKE_CURRENT_BINARY_DIR}) # Include for generated ui header files
include_directories(src/cpp/)
include_directories(${Python_INCLUDE_DIRS}) # Python

if (WIN32)
    add_executable(qcalc WIN32 ${SRC} ${WRAP_CPP} ${WRAP_UI})
else ()
    add_executable(qcalc ${SRC} ${WRAP_CPP} ${WRAP_UI})
endif ()

target_link_libraries(qcalc Qt5::Core Qt5::Widgets)
target_link_libraries(qcalc ${Python_LIBRARIES}) # Python