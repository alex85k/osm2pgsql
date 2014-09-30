set CMAKE_PREFIX_PATH=C:\Program Files\PostgreSQL\9.3
call "%VS120COMNTOOLS%\..\..\VC\vcvarsall.bat" x86_amd64
copy CMakeLists.txt ..
cmake .. -DCMAKE_INSTALL_PREFIX=d:/libs18d -G "NMake Makefiles" -DBUILD_PBF=ON -DCMAKE_BUILD_TYPE=Debug -DBOOST_ROOT=c:\boost -DBoost_USE_STATIC_LIBS=ON
nmake
