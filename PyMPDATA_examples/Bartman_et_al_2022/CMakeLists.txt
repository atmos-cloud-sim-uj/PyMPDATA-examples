cmake_minimum_required(VERSION 3.18)
project(revolving_sphere_3d CXX)
find_package(libmpdata++)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${libmpdataxx_CXX_FLAGS_RELEASE}")
add_executable(revolving_sphere_3d revolving_sphere_3d.cpp)
target_link_libraries(revolving_sphere_3d ${libmpdataxx_LIBRARIES})
target_include_directories(revolving_sphere_3d PUBLIC ${libmpdataxx_INCLUDE_DIRS})

