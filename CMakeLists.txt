cmake_minimum_required(VERSION 3.16)
project(schedsim CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Release)


include_directories(${CMAKE_SOURCE_DIR}/include)


add_executable(schedsim
    src/main.cpp
    src/io.cpp
    src/fifo.cpp
    src/sjf_np.cpp
    src/sjf_p.cpp
    src/rr.cpp
    src/lottery.cpp
    src/mlfq.cpp
)