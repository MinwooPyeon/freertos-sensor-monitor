cmake_minimum_required(VERSION 3.16)

project(freertos-sensor-monitor C ASM)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

# ARM cross-compiler
set(CMAKE_C_COMPILER    arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER  arm-none-eabi-gcc)
set(CMAKE_OBJCOPY       arm-none-eabi-objcopy)
set(CMAKE_SIZE          arm-none-eabi-size)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Cortex-M3 flags
set(CPU_FLAGS "-mcpu=cortex-m3 -mthumb")
set(CMAKE_C_FLAGS   "${CPU_FLAGS} -O2 -g -Wall -Wextra -ffunction-sections -fdata-sections")
set(CMAKE_EXE_LINKER_FLAGS
    "${CPU_FLAGS} -T${CMAKE_SOURCE_DIR}/src/startup/lm3s6965.ld \
    -Wl,--gc-sections -Wl,-Map=${PROJECT_NAME}.map --specs=nano.specs --specs=nosys.specs")

# FreeRTOS kernel path (added as git submodule)
set(FREERTOS_DIR ${CMAKE_SOURCE_DIR}/FreeRTOS-Kernel)

set(FREERTOS_SOURCES
    ${FREERTOS_DIR}/tasks.c
    ${FREERTOS_DIR}/queue.c
    ${FREERTOS_DIR}/list.c
    ${FREERTOS_DIR}/timers.c
    ${FREERTOS_DIR}/portable/GCC/ARM_CM3/port.c
    ${FREERTOS_DIR}/portable/MemMang/heap_4.c
)

set(APP_SOURCES
    src/startup/startup_lm3s6965.c
    src/main.c
    src/drivers/uart_driver.c
    src/drivers/virtual_sensor.c
    src/tasks/sensor_task.c
    src/tasks/process_task.c
    src/tasks/uart_task.c
    src/tasks/watchdog_task.c
)

add_executable(${PROJECT_NAME}.elf ${APP_SOURCES} ${FREERTOS_SOURCES})

target_include_directories(${PROJECT_NAME}.elf PRIVATE
    src/config
    src/drivers
    src/tasks
    src/startup
    ${FREERTOS_DIR}/include
    ${FREERTOS_DIR}/portable/GCC/ARM_CM3
)

# Generate binary and hex files
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_OBJCOPY} -O ihex   $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_SIZE}    $<TARGET_FILE:${PROJECT_NAME}.elf>
)
