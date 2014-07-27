set CMAKE_PREFIX_PATH=C:\PROJ;x:\libs18;C:\Program Files\PostgreSQL\9.3
set BZIPOPTS=-DBZIP2_INCLUDE_DIR=x:\libs18\include -DBZIP2_LIBRARY_RELEASE=x:\libs18\lib\libbz2.lib

call "%VS120COMNTOOLS%\..\..\VC\vcvarsall.bat" x86_amd64
cmake .. -G "NMake Makefiles" -DBUILD_PBF=ON -DCMAKE_BUILD_TYPE=Release %BZIPOPTS% 
nmake
