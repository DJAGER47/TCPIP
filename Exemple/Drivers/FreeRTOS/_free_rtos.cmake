include_guard(GLOBAL)
message("_free_rtos component is included.")

target_sources(${EXECUTABLE} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/Source/portable/MemMang/heap_4.c
  ${CMAKE_CURRENT_LIST_DIR}/Source/portable/GCC/ARM_CM7/r0p1/port.c
  ${CMAKE_CURRENT_LIST_DIR}/Source/CMSIS_RTOS/cmsis_os.c
  ${CMAKE_CURRENT_LIST_DIR}/Source/queue.c
  ${CMAKE_CURRENT_LIST_DIR}/Source/tasks.c
  ${CMAKE_CURRENT_LIST_DIR}/Source/croutine.c
  ${CMAKE_CURRENT_LIST_DIR}/Source/timers.c
  ${CMAKE_CURRENT_LIST_DIR}/Source/list.c

)

target_include_directories(${EXECUTABLE} PRIVATE
${CMAKE_CURRENT_LIST_DIR}/Source/
${CMAKE_CURRENT_LIST_DIR}/Source/portable/GCC/ARM_CM7/r0p1/
${CMAKE_CURRENT_LIST_DIR}/Source/CMSIS_RTOS/
${CMAKE_CURRENT_LIST_DIR}/Source/include/
)
