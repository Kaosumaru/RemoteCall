cmake_minimum_required (VERSION 2.8)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/Modules CACHE TYPE STRING FORCE)
include(CommonFunctions)

project (Tests)

include(conanbuildinfo.cmake)
conan_basic_setup()


if (MSVC)
add_definitions( "/W3 /D_CRT_SECURE_NO_WARNINGS /wd4018 /wd4005 /wd4996 /nologo" )
endif(MSVC)

#add_subdirectory(../RPN "${CMAKE_CURRENT_BINARY_DIR}/RPN")
#include_directories ("${RPN_Includes}")

add_folder(mtl)
add_folder(mtl/lockless)
add_folder(mtl/mappers)
add_folder(mtl/pointer)

add_executable (Tests main.cpp ${SRCS})
target_link_libraries(Tests ${CONAN_LIBS})
