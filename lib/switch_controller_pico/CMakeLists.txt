# Switch Controller Pico Library
# Nintendo Switch Pro Controller emulation for Raspberry Pi Pico

# Build both USB and Bluetooth variants as separate libraries
# This allows applications to link against the appropriate variant

# USB variant (default)
add_library(switch_controller_pico_usb
  src/switch_controller_pico.cpp
  src/SwitchCommon.cpp
  src/SwitchUsb.cpp
  src/Controller.cpp
)

target_include_directories(switch_controller_pico_usb PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(switch_controller_pico_usb PUBLIC
  pico_stdlib
  pico_rand
  hardware_pio
  tinyusb_device
)

# Bluetooth variant (for Pico W)
add_library(switch_controller_pico_bluetooth
  src/switch_controller_pico.cpp
  src/SwitchCommon.cpp
  src/SwitchBluetooth.cpp
  src/SwitchUsb.cpp
  src/Controller.cpp
)

target_include_directories(switch_controller_pico_bluetooth PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(switch_controller_pico_bluetooth PUBLIC
  pico_stdlib
  pico_rand
  hardware_pio
  pico_btstack_cyw43
  pico_btstack_hci_transport_cyw43
  pico_btstack_ble
  pico_btstack_classic
  pico_cyw43_arch_threadsafe_background
  tinyusb_device
)

target_compile_definitions(switch_controller_pico_bluetooth PUBLIC
  SWITCH_BLUETOOTH
  CYW43_LWIP=0
)

# Alias for convenience - applications can use switch_controller_pico
# and the correct variant will be selected based on SWITCH_BLUETOOTH
if(DEFINED ENV{SWITCH_BLUETOOTH} OR SWITCH_BLUETOOTH)
  add_library(switch_controller_pico ALIAS switch_controller_pico_bluetooth)
else()
  add_library(switch_controller_pico ALIAS switch_controller_pico_usb)
endif()

# Build examples
add_subdirectory(examples)
