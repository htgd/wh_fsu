#告知当前使用的是交叉编译方式，必须配置
SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_PROCESSOR arm)

#交叉编译环境路径
SET(TOOLCHAIN_DIR "/home/wangh/Tools/gcc-linaro-4.9-2014.11-x86_64_arm-linux-gnueabihf")
#指定交叉编译环境查找目录
SET(CMAKE_FIND_ROOT_PATH ${TOOLCHAIN_DIR})
#从来不在指定目录下查找工具程序
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
#只在指定目录下查找库文件
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
#只在指定目录下查找头文件
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

#指定编译工具或交叉编译器使用绝对地址
SET(CMAKE_C_COMPILER ${TOOLCHAIN_DIR}/bin/arm-linux-gnueabihf-gcc)
#指定C++交叉编译器
SET(CMAKE_CXX_COMPILER ${TOOLCHAIN_DIR}/bin/arm-linux-gnueabihf-g++)
 

