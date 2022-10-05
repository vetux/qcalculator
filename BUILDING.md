# How to build the application
1. Download and install the required dependencies. (Qt5, CPython, mpdecimal and libarchive)
2. Open the CMakeLists.txt file in your favorite IDE.
3. Build the "qcalc" cmake target.
4. (win32 only) Because the windows PySide2 implementation requires special Qt dlls you have to copy
the Qt dlls from PySide2 to the build directory. (Qt5Core.dll, Qt5Gui.dll, Qt5Widgets.dll, styles/qwindowsvistastyle.dll and platforms/*.dll)