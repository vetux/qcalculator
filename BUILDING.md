# How to build the application

1. Download and install the required dependencies. (Prebuilt MPFR binaries for win32 can be found in the releases
   section. (Source: [CGAL](https://github.com/CGAL/cgal/releases)))
2. Open the CMakeLists.txt file in your favorite IDE.
3. Build the "qcalc" cmake target.
4. (win32 only) Because on windows the python interpreter does not have a standard path to look for the python standard library
   you have to copy the "Lib" and "DLLs" folder from your python installation into the build directory. Also you need to copy the python dlls (python39.dll and python3.dll) from your python installation because win32 does not have standard link paths.
5. (win32 only) Because the windows PySide2 implementation requires special Qt dlls you have to copy
the Qt dlls from PySide2 to the build directory. (Qt5Core.dll, Qt5Gui.dll, Qt5Widgets.dll, styles/qwindowsvistastyle.dll and platforms/*.dll)