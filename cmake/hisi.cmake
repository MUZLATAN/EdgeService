set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR armv7-a)

# set(CMAKE_C_COMPILER arm-himix200-linux-gcc)
# set(CMAKE_CXX_COMPILER arm-himix200-linux-g++)

set(CMAKE_C_COMPILER arm-himix200-linux-gcc)
set(CMAKE_CXX_COMPILER arm-himix200-linux-g++)

add_definitions("-mfpu=neon -mfloat-abi=softfp")
add_definitions("-D__PLATFORM_HISI__")