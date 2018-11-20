cmake_minimum_required(VERSION 2.6)

project(hw5)

include_directories(include)

find_package( OpenCV REQUIRED )

set(CMAKE_CXX_FLAGS "-std=c++17 -DBDBG -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs")
set(SOURCES Source/main.cpp Source/Filters.cpp Source/ScopedTimer.cpp)

message("CXX flags: ${CMAKE_CXX_FLAGS}")

add_executable(1612840_Lab04 ${SOURCES})

target_link_libraries(1612840_Lab04 ${OpenCV_LIBS})
