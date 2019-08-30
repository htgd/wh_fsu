#需要交叉编译时 include
include (cross.cmake)
#CMake最低版本号要求
cmake_minimum_required(VERSION 3.5.1 FATAL_ERROR)
#项目信息
project(monitor)
message(STATUS "Project: SERVER") 


# 加入一个配置头文件，用于处理 CMake 对源码的设置
#configure_file (
#  "${PROJECT_SOURCE_DIR}/config.h.in"
#  "${PROJECT_BINARY_DIR}/config.h"
#  )


#add_definitions("-Wall -g")
SET(CMAKE_C_FLAGS "-std=c99 ${CMAKE_C_FLAGS}")
SET(CMAKE_C_FLAGS "-DWITH_NO_C_LOCALE")
message("${PROJECT_SOURCE_DIR}")
message("${PROJECT_BINARY_DIR}")
message(STATUS "${CMAKE_C_FLAGS}")

#SET(CMAKE_BUILD_TYPE Release)
set(CMAKE_BUILD_TYPE Debug)

#添加头文件目录(类是于 -I)
include_directories("src")
include_directories("lib/sqlite3/include")
include_directories("lib/libmodbus/include")
include_directories("soap_sc")
include_directories("lib/goahead/include")

#添加需要链接的库文件目录（-L）
link_directories("lib/sqlite3/lib")
link_directories("lib/libmodbus/lib")

#链接库文件(-l)
#link_libraries("sqlite3")

#查找目录下的所有源文件,并将名称保存到 SRC_LIST 变量
aux_source_directory(src SRC_LIST)
aux_source_directory(soap_sc SRC_LIST)


# 指定生成目标 
ADD_EXECUTABLE(monitor ${SRC_LIST})
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 添加链接库（dl:动态库加载、pthread：线程库、rt:运行时函数库（#include<time.h>）、m:数学库）
TARGET_LINK_LIBRARIES(monitor dl)
TARGET_LINK_LIBRARIES(monitor pthread)
TARGET_LINK_LIBRARIES(monitor ${CMAKE_CURRENT_SOURCE_DIR}/lib/sqlite3/lib/libsqlite3.so)
TARGET_LINK_LIBRARIES(monitor ${CMAKE_CURRENT_SOURCE_DIR}/lib/libmodbus/lib/libmodbus.so)
TARGET_LINK_LIBRARIES(monitor rt)
#TARGET_LINK_LIBRARIES(monitor m)



#添加子目录(子目录编译为库或子程序时使用)
#ADD_SUBDIRECTORY(soap_sc)
ADD_SUBDIRECTORY(web_src)








