cmake_minimum_required(VERSION 3.20)

project(tamagaki LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(TAMAGAKI_STRICT_LIBBPF "Require libbpf at configure time" OFF)

find_package(Threads REQUIRED)
find_package(PkgConfig QUIET)

if(PkgConfig_FOUND)
  pkg_check_modules(LIBBPF QUIET libbpf)
endif()

if(TAMAGAKI_STRICT_LIBBPF AND NOT LIBBPF_FOUND)
  message(FATAL_ERROR "libbpf is required when TAMAGAKI_STRICT_LIBBPF=ON")
endif()

add_executable(tamagaki
  cmd/agent/main.cpp
  src/control/runtime.cpp
  src/control/server.cpp
  src/ebpf/loader.cpp
  src/ebpf/map.cpp
  src/ebpf/tc.cpp
  src/ebpf/xdp.cpp
  src/ids/alert.cpp
  src/ids/baseline.cpp
  src/ids/monitor.cpp
  src/net/address.cpp
)

target_include_directories(tamagaki
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(tamagaki PRIVATE Threads::Threads)

if(LIBBPF_FOUND)
  target_include_directories(tamagaki PRIVATE ${LIBBPF_INCLUDE_DIRS})
  target_link_directories(tamagaki PRIVATE ${LIBBPF_LIBRARY_DIRS})
  target_link_libraries(tamagaki PRIVATE ${LIBBPF_LIBRARIES})
  target_compile_definitions(tamagaki PRIVATE TAMAGAKI_HAVE_LIBBPF=1)
endif()

add_executable(tamagaki-notify-boot src/notify/boot_success.c)

install(TARGETS tamagaki DESTINATION usr/bin)
install(
  TARGETS tamagaki-notify-boot
  DESTINATION usr/libexec
  PERMISSIONS
    OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
)
