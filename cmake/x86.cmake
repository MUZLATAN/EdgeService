set(CMAKE_SYSTEM_NAME Linux)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

add_definitions("-D__PLATFORM_X86__ --std=c++14")