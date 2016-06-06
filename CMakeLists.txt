cmake_minimum_required(VERSION 3.0.2)
project("FACCHA")

option(COMPILE_64BIT "compile for 64bit" ON)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Debug CACHE STRING
      "Set buld type. Possible values: Debug Release RelWithDebInfo MinSizeRel"
      FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

# Set output directories.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

function(append_cxx_flag flag)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}" PARENT_SCOPE)
endfunction(append_cxx_flag)

function(append_cxx_flag_debug flag)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${flag}" PARENT_SCOPE)
endfunction(append_cxx_flag_debug)

function(append_cxx_flag_release flag)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${flag}" PARENT_SCOPE)
endfunction(append_cxx_flag_release)

macro(common_gcc_clang_setup)
	append_cxx_flag("-std=c++14")
	append_cxx_flag("-march=native")
	if(COMPILE_64BIT)
		append_cxx_flag("-m64")
	else()
		append_cxx_flag("-m32")
	endif()
	# Enable all warnings.
	append_cxx_flag("-Wall")

	append_cxx_flag_debug("-DDEBUG")

	append_cxx_flag_release("-O3")
	append_cxx_flag_release("-flto")
	append_cxx_flag_release("-DNDEBUG")
endmacro(common_gcc_clang_setup)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	common_gcc_clang_setup()

	# Enable additional warnings
	append_cxx_flag("-Weverything")

	# We don't care about compatibility
	append_cxx_flag("-Wno-c++98-compat")
	append_cxx_flag("-Wno-c++98-compat-pedantic")
	append_cxx_flag("-Wno-global-constructors")
	append_cxx_flag("-Wno-exit-time-destructors")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	common_gcc_clang_setup()

	# Enable additional warnings
	append_cxx_flag("-Wextra")
endif()

find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)
find_package(ZLIB REQUIRED)

include_directories(SYSTEM ${JPEG_INCLUDE_DIR} ${PNG_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS})
include_directories(include)

add_subdirectory(src)