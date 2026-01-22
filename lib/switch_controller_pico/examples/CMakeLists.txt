# Switch Controller Pico Library - Examples
# Build examples demonstrating library usage

# Auto Press Example - Presses A button every 5 seconds
add_executable(auto_press_example
  ../auto_press_example.cpp
)

target_include_directories(auto_press_example PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

target_link_libraries(auto_press_example
  pico_stdlib
  switch_controller_pico_usb
)

# Enable USB output for printf
pico_enable_stdio_usb(auto_press_example 1)
pico_enable_stdio_uart(auto_press_example 0)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(auto_press_example)

# Simple Button Example - GPIO buttons
add_executable(simple_button_example
  ../example.cpp
)

target_include_directories(simple_button_example PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

target_link_libraries(simple_button_example
  pico_stdlib
  switch_controller_pico_usb
)

pico_enable_stdio_usb(simple_button_example 1)
pico_enable_stdio_uart(simple_button_example 0)

pico_add_extra_outputs(simple_button_example)
