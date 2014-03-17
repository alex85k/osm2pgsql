SET(PACKAGE osm2pgsql)
SET(PACKAGE_NAME osm2pgsql)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8 FATAL_ERROR)

PROJECT(osm2pgsql)

INCLUDE (CheckIncludeFiles)
INCLUDE (CheckFunctionExists)
INCLUDE (CheckTypeSize)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

if("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")
  message(SEND_ERROR "Please run cmake from different folder. In-source builds are not allowed.")
endif()

SET(DATA_DIR \".\")
SET(PACKAGE_VERSION \"0.85-win-cmake\")

OPTION(USE64BITID "Build with 64 bit OSM ID" ON)
OPTION(BUILD_PBF "Build with PBF format support" OFF)

ADD_DEFINITIONS( -DOSM2PGSQL_DATADIR=${DATA_DIR} )
ADD_DEFINITIONS( -DVERSION=${PACKAGE_VERSION} )

if ( MSVC )
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -wd4996)
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /VERBOSE:LIB /STACK:30000000")
    message("Linker flags: ${CMAKE_EXE_LINKER_FLAGS}")
endif()

CHECK_INCLUDE_FILES (termios.h HAVE_TERMIOS_H)
CHECK_INCLUDE_FILES (libgen.h HAVE_LIBGEN_H)
CHECK_INCLUDE_FILES (unistd.h HAVE_UNISTD_H)

CHECK_FUNCTION_EXISTS(lseek64, HAVE_LSEEK64)
CHECK_FUNCTION_EXISTS(posix_fallocate, HAVE_POSIX_FALLOCATE)
CHECK_FUNCTION_EXISTS(posix_fadvice, HAVE_POSIX_FADVICE)
CHECK_FUNCTION_EXISTS(sync_file_range, HAVE_SYNC_FILE_RANGE)
CHECK_FUNCTION_EXISTS(fork, HAVE_FORK)

CHECK_TYPE_SIZE("off_t" SIZEOF_OFF_T)

find_package(ZLIB)

find_package(Threads)


if ( ZLIB_FOUND )
    include_directories(${ZLIB_INCLUDE_DIR})
else(ZLIB)
    MARK_AS_ADVANCED(CLEAR ZLIB_LIBRARY ZLIB_INCLUDE_DIR)
    message(FATAL_ERROR "Please, specify the ZLIB includes directory and the library file")
endif( ZLIB_FOUND )


if ( BUILD_PBF )
    find_package(Protobuf)
if ( PROTOBUF_INCLUDE_DIR AND PROTOBUF_LIBRARY )
    FIND_LIBRARY(PROTOBUF_C_LIBRARY protobuf-c)
    FIND_PROGRAM(PROTOBUF_C_EXECUTABLE NAMES protoc-c
                 DOC "The Protobuf-C Compiler" )
    FIND_PATH(PROTOBUF_C_INCLUDE protobuf-c/protobuf-c.h)
    if ( PROTOBUF_C_LIBRARY  AND PROTOBUF_C_EXECUTABLE AND PROTOBUF_C_INCLUDE) 
      set(PBF_READER_SOURCES parse-pbf.c protobuf/fileformat.pb-c.c protobuf/osmformat.pb-c.c)

      add_custom_command(OUTPUT protobuf/fileformat.pb-c.c protobuf/osmformat.pb-c.c 
                   COMMAND ${PROTOBUF_C_EXECUTABLE} -I${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/protobuf/fileformat.proto --c_out=${CMAKE_BINARY_DIR}
                   COMMAND ${PROTOBUF_C_EXECUTABLE} -I${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/protobuf/osmformat.proto --c_out=${CMAKE_BINARY_DIR}
                   )

      include_directories(${PROTOBUF_C_INCLUDE})
      include_directories(${CMAKE_BINARY_DIR}/protobuf)
      ADD_DEFINITIONS(-DBUILD_READER_PBF)
    else()
      message(FATAL_ERROR "Please specify the Protobuf-C library location")
    endif()
else()
    MARK_AS_ADVANCED(CLEAR PROTOBUF_INCLUDE_DIR PROTOBUF_LITE_LIBRARY )
    message(FATAL_ERROR "Please specify the Google PROTOBUF library dir")
endif()
endif()

find_package(LibXml2)
if ( LIBXML2_LIBRARIES )
    include_directories(${LIBXML2_INCLUDE_DIR})
else()
    MARK_AS_ADVANCED(CLEAR LIBXML2_INCLUDE_DIR LIBXML2_LIBRARIES)
    message(FATAL_ERROR "Please, specify the LibXML2 library dir")
endif()

find_package(BZip2)
include_directories(${BZIP2_INCLUDE_DIR})

find_package(Lua52)
if ( LUA_LIBRARIES )
include_directories(${LUA_INCLUDE_DIR})
set(HAVE_LUA 1)
else()
    MARK_AS_ADVANCED(CLEAR LUA_LIBRARIES LUA_LIBRARY)
    message(FATAL_ERROR "Please, specify the Lua libraries and include dir")
endif()

find_package(PostgreSQL)
if ( PostgreSQL_FOUND )
    include_directories(${PostgreSQL_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "PostgreSQL not found, please install and give the path to CMAKE")
endif()

##### Find GEOS Library 
find_path(GEOS_INCLUDE_DIR geos/geom.h)
find_library(GEOS_LIBRARY NAMES geos)
if(GEOS_INCLUDE_DIR AND GEOS_LIBRARY)
  MESSAGE( STATUS "GEOS library found")
  SET(GEOS_FOUND 1)
  include_directories(${GEOS_INCLUDE_DIR})
else()
  MESSAGE( STATUS "GEOS library not found")
endif()
#####

##### Find Proj.4 Library 
find_path(PROJ_INCLUDE_DIR proj_api.h)
find_library(PROJ_LIBRARY NAMES proj)
if(PROJ_INCLUDE_DIR AND PROJ_LIBRARY)
  MESSAGE( STATUS "PROJ library found")
  SET(GEOS_FOUND 1)
  include_directories(${PROJ_INCLUDE_DIR})
else()
  MESSAGE( STATUS "PROJ library not found")
endif()
#####

if(WIN32)
#	ADD_DEFINITIONS( -DWIN32 -D_WIN32 -DMSWIN32 -DBGDWIN32 -DWINVER=0x0500  -D_WIN32_WINNT=0x0500 -D_WIN32_IE=0x0600)
#	set(CPACK_GENERATOR ZIP)
else(WIN32)
#	set(CPACK_GENERATOR TGZ)
endif(WIN32)

if(NOT CMAKE_COMPILER_IS_GNUCC AND WIN32)
   find_path(GETOPT_INCLUDE_DIR getopt.h)
   find_library(GETOPT_LIBRARY NAMES wingetopt )
   if (GETOPT_INCLUDE_DIR AND GETOPT_LIBRARY)
       include_directories(${GETOPT_INCLUDE_DIR})
   endif()
endif()

if (NOT HAVE_UNISTD_H) 
   file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/unistd.h "//Generated stub include\n #include <windows.h>\n")
endif()


include_directories(${CMAKE_CURRENT_BINARY_DIR})

if (USE64BITID)
   ADD_DEFINITIONS( -DOSMID64 )
endif()

SET (osm2pgsql_SOURCES 
        build_geometry.cpp 
        input.c middle.h middle-ram.h output-gazetteer.h 
        output-pgsql.c rb.c sanitizer.h text-tree.h build_geometry.h input.h 
        middle-pgsql.c osm2pgsql.c output.h output-pgsql.h rb.h sprompt.c UTF8sanitizer.c 
        expire-tiles.c keyvals.c middle-pgsql.h osmtypes.h output-null.c parse-o5m.c parse-o5m.h 
        parse-primitive.c parse-primitive.h parse-xml2.c parse-xml2.h pgsql.c reprojection.c sprompt.h 
        expire-tiles.h keyvals.h middle-ram.c output-gazetteer.c output-null.h pgsql.h reprojection.h 
        text-tree.c node-ram-cache.c wildcmp.c node-ram-cache.h node-persistent-cache.c node-persistent-cache.h 
        binarysearcharray.c binarysearcharray.h tagtransform.c ${PBF_READER_SOURCES})

SET(nodecachefilereader_SOURCES node-persistent-cache-reader.c node-persistent-cache.c node-ram-cache.c binarysearcharray.c)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

add_executable(osm2pgsql ${osm2pgsql_SOURCES})

if (BUILD_PBF)
  if (WIN32)
   SET(WIN_MORE_LIBS ws2_32)
  endif()
endif()

target_link_libraries(osm2pgsql ${GEOS_LIBRARY} ${PROJ_LIBRARY} 
  ${LIBXML2_LIBRARIES} ${GETOPT_LIBRARY} ${PostgreSQL_LIBRARY} 
  ${LUA_LIBRARY} ${BZIP2_LIBRARY} ${PROTOBUF_C_LIBRARY} ${WIN_MORE_LIBS} ${ZLIB_LIBRARIES} 
  )

install(TARGETS osm2pgsql DESTINATION bin)

INCLUDE(CPack)
