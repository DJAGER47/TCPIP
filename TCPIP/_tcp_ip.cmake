include_guard(GLOBAL)
message("_tcp_ip component is included.")

target_sources(${EXECUTABLE} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/Src/ARP.cpp
  ${CMAKE_CURRENT_LIST_DIR}/Src/ICMP.cpp
  ${CMAKE_CURRENT_LIST_DIR}/Src/IPv4.cpp
  ${CMAKE_CURRENT_LIST_DIR}/Src/MACEthernet.cpp
  ${CMAKE_CURRENT_LIST_DIR}/Src/Utility.cpp
)


target_include_directories(${EXECUTABLE} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/Inc
  ${CMAKE_CURRENT_LIST_DIR}/Inc/Interface
)
