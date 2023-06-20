CMAKE_MINIMUM_REQUIRED (VERSION 3.10.0)

project(tests)
ENABLE_LANGUAGE(C CXX ASM)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_BUILD_TYPE})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC 
    ../TCPIP/Src/ARP.cpp
    ../TCPIP/Src/ICMP.cpp
    ../TCPIP/Src/IPv4.cpp
    ../TCPIP/Src/MACEthernet.cpp
    ../TCPIP/Src/Utility.cpp
)
  
set(INCLUDE 
    ../TCPIP/Inc
    ../TCPIP/Inc/Interface
    /
)

##### Modul


##### Code
add_executable(testAlloc testAlloc.cpp ${SRC})
target_include_directories(testAlloc PRIVATE ${INCLUDE})

add_executable(testStack testStack.cpp ${SRC})
target_include_directories(testStack PRIVATE ${INCLUDE})

##### TESTS

add_test(NAME tAlloc COMMAND testAlloc)
add_test(NAME tStack COMMAND testStack)
