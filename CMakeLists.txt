#设置CMAKE最小版本
CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

#设置构建类型，及相应的编译选项
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -std=c++11 /D _DEBUG /MTd /ZI /Ob0 /Od")
set (CMAKE_EXE_LINKER_FLAGS   "/DEBUG")           #仅针对vc编译器，否则无法正常调试
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -Ob3 -Wall")

#设置执行文件输出目录
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#设置库输出路径
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

SET(MY_TARGET CPPTest)

MESSAGE("++++++++++++++Start Build+++++++++++++++++")
#当前目录的文件
file(GLOB_RECURSE SRC_LIST src/*.cpp src/*.cc src/*.cxx)

#头文件搜索目录
INCLUDE_DIRECTORIES(inc)
#链接库搜索路径
LINK_DIRECTORIES(lib)
#设置链接库
SET(EXTRA_LIBS ${EXTRA_LIBS} )

#生成可执行文件
ADD_EXECUTABLE(${MY_TARGET} ${SRC_LIST})

#执行文件链接属性
TARGET_LINK_LIBRARIES(${MY_TARGET} ${EXTRA_LIBS})