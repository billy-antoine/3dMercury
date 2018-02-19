cmake_minimum_required(VERSION 2.8)
set (CMAKE_CXX_STANDARD 11)
add_compile_options(-std=c++11) # CMake 2.8.12 or newer
project( 3dMercury )
find_package( OpenCV REQUIRED )
add_executable( 3dMercury main.cpp )
target_link_libraries( 3dMercury ${OpenCV_LIBS} )