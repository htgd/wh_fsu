#添加头文件目录(类是于 -I)
include_directories("../lib/goahead/include")

#添加需要链接的库文件目录（-L）
link_directories("../lib/goahead/lib")

# 查找当前目录下的所有源文件，并将名称保存到 DIR_LIB_SRCS 变量
#aux_source_directory(. DIR_WEB_SRCS)
SET(DIR_WEB_SRCS fsu_web.c)

# 生成链接库
ADD_EXECUTABLE(fsu_web fsu_web.c)
TARGET_LINK_LIBRARIES(fsu_web libgo.so)
TARGET_LINK_LIBRARIES(fsu_web libgoahead-mbedtls.a)
TARGET_LINK_LIBRARIES(fsu_web rt;dl;pthread;m)
