# Switch Controller Pico Library

A Raspberry Pi Pico library for emulating a Nintendo Switch Pro Controller over USB or Bluetooth.

## Features

- ✅ Full Nintendo Switch Pro Controller emulation
- ✅ USB HID support (works on any Pico)
- ✅ Bluetooth HID support (requires Pico W)
- ✅ Handles all Switch subcommands (device info, calibration, player lights, vibration, IMU)
- ✅ Simple input provider interface
- ✅ Works with any input source (buttons, sensors, other controllers, etc.)

## Installation

1. Copy the `switch_controller_pico` directory to your project's `lib/` folder
2. Add to your project's `CMakeLists.txt`:

```cmake
add_subdirectory(lib/switch_controller_pico)
target_link_libraries(your_project switch_controller_pico_usb)  # For USB
# Or for Bluetooth:
# target_link_libraries(your_project switch_controller_pico_bluetooth)
```

## Quick Start

### 1. Implement the Input Provider

```cpp
#include "switch_controller_pico.h"

class MyInputProvider : public SwitchInputProvider {
 public:
  void getSwitchReport(SwitchReport *report) override {
    // Fill in button states
    report->buttons[0] = 0x00;  // Byte 0: Y, X, B, A, R, ZR, R3, L3
    report->buttons[1] = 0x00;  // Byte 1: Minus, Plus, RStick, LStick, Home, Capture, (unused), (unused)
    report->buttons[2] = 0x00;  // Byte 2: Down, Up, Right, Left, SR, SL, L, ZL

    // Set analog sticks (12-bit values: 0x000-0xFFF, center: 0x7FF)
    uint16_t leftX = 0x7FF;   // Center
    uint16_t leftY = 0x7FF;   // Center
    uint16_t rightX = 0x7FF;  // Center
    uint16_t rightY = 0x7FF;  // Center

    // Pack stick values into 3-byte format
    report->l[0] = leftX & 0xFF;
    report->l[1] = ((leftX >> 8) & 0x0F) | ((leftY & 0x0F) << 4);
    report->l[2] = (leftY >> 4) & 0xFF;

    report->r[0] = rightX & 0xFF;
    report->r[1] = ((rightX >> 8) & 0x0F) | ((rightY & 0x0F) << 4);
    report->r[2] = (rightY >> 4) & 0xFF;
  }

  void setRumble(bool enabled) override {
    // Handle rumble/vibration (optional)
  }
};
```

### 2. Initialize and Run (USB Mode)

```cpp
#include "pico/stdlib.h"
#include "switch_controller_pico.h"

int main() {
  stdio_init_all();

  MyInputProvider input;
  SwitchControllerPico controller(&input, SwitchControllerPico::USB);

  controller.init();

  while (1) {
    controller.update();  // Process USB events
    sleep_ms(1);
  }
}
```

### 3. Initialize and Run (Bluetooth Mode)

For Bluetooth, you need:
- Pico W board
- Define `SWITCH_BLUETOOTH` in CMakeLists.txt

```cpp
#include "pico/stdlib.h"
#include "switch_controller_pico.h"

#ifdef SWITCH_BLUETOOTH
#include "btstack.h"

extern void packet_handler(SwitchCommon *inst, uint8_t packet_type, uint8_t *packet);
extern void hid_report_data_callback(SwitchCommon *inst, uint16_t report_id,
                                     uint8_t *report, int report_size);

static btstack_packet_callback_registration_t hci_event_callback_registration;
SwitchControllerPico *g_controller = nullptr;

static void packet_handler_wrapper(uint8_t packet_type, uint16_t channel,
                                   uint8_t *packet, uint16_t packet_size) {
  packet_handler(g_controller->getSwitchCommon(), packet_type, packet);
}

static void hid_report_data_callback_wrapper(uint16_t cid,
                                             hid_report_type_t report_type,
                                             uint16_t report_id,
                                             int report_size, uint8_t *report) {
  hid_report_data_callback(g_controller->getSwitchCommon(), report_id,
                          report - 1, report_size + 1);
}
#endif

int main() {
  stdio_init_all();

  MyInputProvider input;
  SwitchControllerPico controller(&input, SwitchControllerPico::BLUETOOTH);
  g_controller = &controller;

  controller.init();

#ifdef SWITCH_BLUETOOTH
  hci_event_callback_registration.callback = &packet_handler_wrapper;
  hci_add_event_handler(&hci_event_callback_registration);
  hid_device_register_packet_handler(&packet_handler_wrapper);
  hid_device_register_report_data_callback(&hid_report_data_callback_wrapper);

  hci_power_control(HCI_POWER_ON);
  btstack_run_loop_execute();  // Blocking call
#endif
}
```

## Button Mapping

Buttons are stored in 3 bytes. See `SwitchConsts.h` for all button masks:

```cpp
// Byte 0
#define SWITCH_Y       0x01
#define SWITCH_X       0x02
#define SWITCH_B       0x04
#define SWITCH_A       0x08
#define SWITCH_R       0x40
#define SWITCH_ZR      0x80

// Byte 1
#define SWITCH_MINUS   0x01
#define SWITCH_PLUS    0x02
#define SWITCH_HOME    0x10
#define SWITCH_CAPTURE 0x20

// Byte 2
#define SWITCH_DOWN    0x01
#define SWITCH_UP      0x02
#define SWITCH_RIGHT   0x04
#define SWITCH_LEFT    0x08
#define SWITCH_L       0x40
#define SWITCH_ZL      0x80
```

## Analog Sticks

Sticks use 12-bit values (0x000 to 0xFFF):
- Center: 0x7FF (2047)
- Minimum: 0x000 (0)
- Maximum: 0xFFF (4095)

Values are packed into 3 bytes per stick:
```
[X_low] [X_high(4bit)|Y_low(4bit)] [Y_high(8bit)]
```

## Example Projects

See the parent project for a complete example that:
- Reads from N64/Gamecube controllers
- Translates to Switch format
- Supports both USB and Bluetooth

## API Reference

### `SwitchInputProvider` (Interface)

Implement this interface to provide input to the controller emulator.

#### Methods
- `void getSwitchReport(SwitchReport *report)` - Fill report with current state
- `void setRumble(bool enabled)` - Handle rumble on/off

### `SwitchControllerPico` (Main Class)

Main controller emulator class.

#### Constructor
```cpp
SwitchControllerPico(SwitchInputProvider *inputProvider, Mode mode = USB)
```

#### Methods
- `void init()` - Initialize controller (call once)
- `void update()` - Process events (call in main loop for USB mode)
- `SwitchCommon* getSwitchCommon()` - Get internal instance for advanced usage
- `Mode getMode()` - Get current mode (USB or BLUETOOTH)

#### Modes
- `SwitchControllerPico::USB` - USB HID mode
- `SwitchControllerPico::BLUETOOTH` - Bluetooth HID mode (Pico W only)

## Advanced Usage

### Custom Device Info

You can modify device info by accessing the `SwitchCommon` instance:

```cpp
SwitchCommon *sw = controller.getSwitchCommon();
// Modify internal state (use with caution)
```

### Multiple Controllers

Create multiple input providers and controllers:

```cpp
MyInputProvider input1, input2;
SwitchControllerPico controller1(&input1, SwitchControllerPico::USB);
SwitchControllerPico controller2(&input2, SwitchControllerPico::USB);
```

## Requirements

- Raspberry Pi Pico or Pico W
- Pico SDK 1.5.0+
- TinyUSB (included in Pico SDK)
- BTstack (included in Pico SDK for Pico W)

## License

See parent project for license information.

## Credits

Based on the retro-pico-switch project, which provides N64 and Gamecube controller support for Nintendo Switch.
