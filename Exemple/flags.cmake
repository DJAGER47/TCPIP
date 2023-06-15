############### DEBUG ####################
SET(CMAKE_ASM_FLAGS_DEBUG " \
  -D__STARTUP_CLEAR_BSS \
  --specs=nano.specs \
  -x assembler-with-cpp \
")

SET(CMAKE_C_FLAGS_DEBUG " \
  -Og \
  -g3 \
  --specs=nano.specs \
")

SET(CMAKE_CXX_FLAGS_DEBUG " \
  -Og \
  -g3 \
  --specs=nano.specs \
")

#Пока не пойму почему, но в линкере дублируються флаги из С, по этому пока удаляем что есть в С
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG " \
  -T${ProjDirPath}/${LD_FILE} -static \
  --specs=nosys.specs \
  -Wl,-Map=${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}.map,--cref \
  \
  -Xlinker --gc-sections \
  -Xlinker -static \
  ")
  


############### RELEASE ####################
SET(CMAKE_ASM_FLAGS_RELEASE " \
  -D__STARTUP_CLEAR_BSS \
  --specs=nano.specs \
  -x assembler-with-cpp \
")

SET(CMAKE_C_FLAGS_RELEASE " \
  -O2 \
  --specs=nano.specs \
")

SET(CMAKE_CXX_FLAGS_RELEASE " \
  -O2 \
  --specs=nano.specs \
")

SET(CMAKE_EXE_LINKER_FLAGS_RELEASE " \
  -T${ProjDirPath}/${LD_FILE} -static \
  --specs=nosys.specs \
  -Wl,-Map=${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}.map,--cref \
  \
  -Xlinker --gc-sections \
  -Xlinker -static \
  ")