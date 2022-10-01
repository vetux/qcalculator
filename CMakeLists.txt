cmake_minimum_required(VERSION 3.0.0)

project(QCalculator)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)

find_package(Python COMPONENTS Interpreter Development)
message("Python_FOUND:${Python_FOUND}")
message("Python_VERSION:${Python_VERSION}")
message("Python_Development_FOUND:${Python_Development_FOUND}")
message("Python_LIBRARIES:${Python_LIBRARIES}")
message("Python_INCLUDE_DIRS:${Python_INCLUDE_DIRS}")

if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
endif (WIN32)

file(GLOB_RECURSE SRC src/*.cpp)

file(GLOB ADDON_SRC python/addon/*.py)
file(GLOB ADDON_META python/addon/*.json)
file(GLOB_RECURSE SYSTEM_SRC python/lib/*.py)

file(COPY ${ADDON_SRC} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/addon) # Copy addon scripts
file(COPY ${SYSTEM_SRC} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/lib/) # Copy system libraries

set(HDR_GUI src/gui/calculatorwindow.hpp
        src/gui/symbolseditorwindow.hpp
        src/gui/terminalwindow.hpp
        src/gui/dialog/settings/settingsdialog.hpp
        src/gui/dialog/settings/generaltab.hpp
        src/gui/dialog/settings/addontab.hpp
        src/gui/dialog/addoninstalldialog.hpp
        src/gui/widgets/addonitemwidget.hpp
        src/gui/widgets/addonwidget.hpp
        src/gui/widgets/libraryitemwidget.hpp
        src/gui/widgets/historywidget.hpp
        src/gui/widgets/historylabel.hpp
        src/gui/widgets/functionseditor.hpp
        src/gui/widgets/namedvalueeditor.hpp
        src/gui/widgets/scriptseditor.hpp
        src/gui/widgets/builtinseditor.hpp
        src/gui/widgets/symbolseditor.hpp
        src/gui/widgets/terminalwidget.hpp
        src/gui/widgets/stringedititemwidget.hpp
        src/gui/widgets/installaddonitemwidget.hpp)

qt5_wrap_cpp(WRAP_CPP ${HDR_GUI})

include_directories(${CMAKE_CURRENT_BINARY_DIR}) # Include for generated ui header files
include_directories(src/)
include_directories(${Python_INCLUDE_DIRS}) # Python

if (WIN32)
    add_executable(qcalc WIN32 ${SRC} ${WRAP_CPP} ${WRAP_UI})
else ()
    add_executable(qcalc ${SRC} ${WRAP_CPP} ${WRAP_UI})
endif ()

set_property(TARGET qcalc PROPERTY CXX_STANDARD 17)

target_link_libraries(qcalc Qt5::Core Qt5::Widgets)
target_link_libraries(qcalc ${Python_LIBRARIES}) # Python
target_link_libraries(qcalc mpdec mpdec++) # MPDECIMAL
target_link_libraries(qcalc archive) # libarchive
