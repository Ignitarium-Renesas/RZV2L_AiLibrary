cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 17)
project(01_Head_count)

# Setting and including TVM Directory
set(TVM_ROOT $ENV{TVM_HOME})
include_directories(${TVM_ROOT}/include)
include_directories(${TVM_ROOT}/3rdparty/dlpack/include)
include_directories(${TVM_ROOT}/3rdparty/dmlc-core/include)
include_directories(${TVM_ROOT}/3rdparty/compiler-rt)

set(TVM_RUNTIME_LIB ${TVM_ROOT}/build_runtime/libtvm_runtime.so)
set(EXE_NAME "head_count_app")

file(GLOB SRC ./*.cpp ./*.h)
add_executable(${EXE_NAME} ${SRC})
find_package(OpenCV REQUIRED)
if(OpenCV_FOUND)
    include_directories(${OpenCV_INCLUDE_DIRS})
endif()
target_link_libraries(${EXE_NAME} ${OpenCV_LIBS})
target_link_libraries(${EXE_NAME} ${TVM_RUNTIME_LIB})
target_compile_definitions(${EXE_NAME} PRIVATE -DIMAGE_MODE)
