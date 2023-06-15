include_guard(GLOBAL)
message("_debug component is included.")

target_sources(${EXECUTABLE} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/debug.c
  ${CMAKE_CURRENT_LIST_DIR}/SEGGER_RTT.c
  ${CMAKE_CURRENT_LIST_DIR}/SEGGER_RTT_printf.c
)


target_include_directories(${EXECUTABLE} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/
)
