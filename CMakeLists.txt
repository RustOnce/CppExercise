##Project Name
project(AlgorithmExercise)
##Minimum cmake version
cmake_minimum_required(VERSION 3.10)
##Include files
include_directories(${CMAKE_CURRENT_LIST_DIR}/include)
##Souce code directory
aux_source_directory(${CMAKE_CURRENT_LIST_DIR} Src)
##Link directories
link_directories(${CMAKE_CURRENT_LIST_DIR}/lib)
##Linked libraries
#target_link_libraries();
##Set CXX flags
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -std=c++11 /D _DEBUG /MTd /ZI /Ob0 /Od")
set (CMAKE_EXE_LINKER_FLAGS   "/DEBUG")           #仅针对vc编译器，否则无法正常调试
#set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG " )   # release包优化
##Executable file
add_executable(${PROJECT_NAME} ${Src})