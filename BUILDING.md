# How to build the application
1. Download and install the required dependencies. (Qt5, CPython)
2. Clone the libarchive git submodule
3. Build and store mpdecimal headers and libraries in thirdparty/libmpdec/bin and thirdparty/libmpdec/include
4. Open the CMakeLists.txt file in your favorite IDE.
5. Build the "qcalculator" cmake target.
6. (win32 only) Because the windows PySide2 implementation requires special Qt dlls you have to copy
the Qt dlls from PySide2 to the build directory. (Qt5Core.dll, Qt5Gui.dll, Qt5Widgets.dll, styles/qwindowsvistastyle.dll and platforms/*.dll)