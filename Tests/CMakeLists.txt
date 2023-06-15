CMAKE_MINIMUM_REQUIRED (VERSION 3.10.0)

project(tests)
ENABLE_LANGUAGE(C CXX ASM)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_BUILD_TYPE})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC 
    ../TCPIP/Src/Utility.cpp
    ../TCPIP/Src/ARP.cpp
    ../TCPIP/Src/ICMP.cpp
    ../TCPIP/Src/IPv4.cpp
)
  
set(INCLUDE 
    ../TCPIP/Inc
    /
)

##### Modul


##### Code
add_executable(testArp testArp.cpp ${SRC})
target_include_directories(testArp PRIVATE ${INCLUDE})

add_executable(testIcmp testIcmp.cpp ${SRC})
target_include_directories(testIcmp PRIVATE ${INCLUDE})

##### TESTS

add_test(NAME tArp COMMAND testArp)
add_test(NAME tICMP COMMAND testIcmp)
