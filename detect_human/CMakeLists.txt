cmake_minimum_required( VERSION 2.8 )
project (Detect_Human)


#指定编译模式和标准
#set( CMAKE_CXX_COMPILER "g++" )
#set( CMAKE_BUILD_TYPE "Release" )
set( CMAKE_CXX_FLAGS "-std=c++11" )


#指定依赖的库文件
#OpenCv
find_package( OpenCV  REQUIRED )
include_directories(/usr/local/include/opencv4/)

#Threads
find_package(Threads REQUIRED)

#redis
#include_directories(/data/ty/redis-4.0.6/deps/hiredis/)

#curl
#include_directories(/data/software/anaconda2/include/curl/)

#json
#include_directories(/data/ty/imageT/json/src/json/)
#include_directories(/data/ty/imageT/json/include)

#darknet
#include_directories(/root/darknet/include)

#includes
include_directories(./include)
include_directories(./lib)
include_directories(./src)

#需要链接的库文件
link_directories(/root/darknet/)
#link_directories(/data/curl/curl/lib/)
#link_directories(/data/software/anaconda2/lib/)
link_directories(/usr/lib64/)

set(m_test ./src/main.cpp
	./src/improcess.cpp
    ./src/toolfuncs.cpp
)

add_executable( ../bin/test ${m_test})

ADD_LIBRARY(imageT SHARED ${m_test})

#target_link_libraries(test ${OpenCV_LIBS} darknet.so libhiredis.so Threads::Threads libcurl.so.4.6.0)
#target_link_libraries(imageT darknet.so ${OpenCV_LIBS} libhiredis.so Threads::Threads libcurl.so.4.6.0)

target_link_libraries(../bin/test ${OpenCV_LIBS} darknet.so Threads::Threads)
target_link_libraries(imageT darknet.so ${OpenCV_LIBS} Threads::Threads)
