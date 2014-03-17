set CMAKE_PREFIX_PATH=C:\Proj;d:\libs;C:\Program Files\PostgreSQL\9.3
set BZIPOPTS=-DBZIP2_INCLUDE_DIR=d:\libs\include -DBZIP2_LIBRARY_RELEASE=d:\libs\lib\libbz2.lib

call "%VS120COMNTOOLS%\..\..\VC\vcvarsall.bat" x86_amd64
cmake .. -G "NMake Makefiles" -DBUILD_PBF=ON -DCMAKE_BUILD_TYPE=Debug %BZIPOPTS%
nmake
