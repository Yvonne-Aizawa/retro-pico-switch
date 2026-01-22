# Options:
# SWITCH_BLUETOOTH: Remove for USB
add_compile_definitions(SWITCH_BLUETOOTH)
add_subdirectory(otherController)

add_executable(${PROJECT_NAME} main.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

# Link against the appropriate library variant based on SWITCH_BLUETOOTH
if(DEFINED SWITCH_BLUETOOTH OR CMAKE_C_FLAGS MATCHES "SWITCH_BLUETOOTH")
  target_link_libraries(${PROJECT_NAME} pico_stdlib switch_controller_pico_bluetooth n64 gamecube)
else()
  target_link_libraries(${PROJECT_NAME} pico_stdlib switch_controller_pico_usb n64 gamecube)
endif()

pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 1)
pico_add_extra_outputs(${PROJECT_NAME})