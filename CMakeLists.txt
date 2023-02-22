cmake_minimum_required(VERSION 3.0.0)

project(qCalculator)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)

find_package(Python COMPONENTS Interpreter Development)
message("Python_FOUND:${Python_FOUND}")
message("Python_VERSION:${Python_VERSION}")
message("Python_Development_FOUND:${Python_Development_FOUND}")
message("Python_LIBRARIES:${Python_LIBRARIES}")
message("Python_INCLUDE_DIRS:${Python_INCLUDE_DIRS}")

find_package(Threads REQUIRED)

if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
endif (WIN32)

file(GLOB_RECURSE SRC src/*.cpp)

file(GLOB_RECURSE SYSTEM_SRC python/lib/*.py)

file(COPY ${SYSTEM_SRC} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/lib/) # Copy system libraries

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/res/icons DESTINATION ${CMAKE_CURRENT_BINARY_DIR}) # Copy icons

set(HDR_GUI src/windows/calculatorwindow.hpp
        src/windows/symbolseditorwindow.hpp
        src/windows/pythonconsolewindow.hpp
        src/windows/settingsdialog.hpp
        src/windows/addoninstalldialog.hpp
        src/widgets/generaltab.hpp
        src/widgets/addontab.hpp
        src/widgets/pythontab.hpp
        src/widgets/addonitemwidget.hpp
        src/widgets/addonwidget.hpp
        src/widgets/historywidget.hpp
        src/widgets/historylabel.hpp
        src/widgets/functionseditor.hpp
        src/widgets/namedvalueeditor.hpp
        src/widgets/scriptseditor.hpp
        src/widgets/builtinseditor.hpp
        src/widgets/symbolseditor.hpp
        src/widgets/terminalwidget.hpp
        src/widgets/stringedititemwidget.hpp
        src/widgets/installaddonitemwidget.hpp)

qt5_wrap_cpp(WRAP_CPP ${HDR_GUI})

include_directories(${CMAKE_CURRENT_BINARY_DIR}) # Include for generated ui header files
include_directories(src/)
include_directories(${Python_INCLUDE_DIRS}) # Python

if (WIN32)
    set(app_icon_resource_windows "${CMAKE_CURRENT_SOURCE_DIR}/res/app.rc")
    add_executable(qcalculator WIN32 ${SRC} ${WRAP_CPP} ${WRAP_UI}  ${app_icon_resource_windows})
else ()
    add_executable(qcalculator ${SRC} ${WRAP_CPP} ${WRAP_UI})
endif ()

set_property(TARGET qcalculator PROPERTY CXX_STANDARD 17)

target_link_libraries(qcalculator Qt5::Core Qt5::Widgets)
target_link_libraries(qcalculator ${Python_LIBRARIES}) # Python
target_link_libraries(qcalculator mpdec mpdec++) # mpdecimal
target_link_libraries(qcalculator archive) # libarchive
target_link_libraries(qcalculator Threads::Threads) # Threads