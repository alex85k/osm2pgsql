set CMAKE_PREFIX_PATH=C:\PROJ;r:\libs;C:\Program Files\PostgreSQL\9.3
set BZIPOPTS=-DBZIP2_INCLUDE_DIR=r:\libs\include -DBZIP2_LIBRARY_RELEASE=r:\libs\lib\libbz2.lib
cmake .. -G "Visual Studio 12 Win64" -DCMAKE_BUILD_TYPE=Release %BZIPOPTS%
