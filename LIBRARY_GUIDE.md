# Switch Controller Pico Library

This project now includes a reusable library for Nintendo Switch Pro Controller emulation on Raspberry Pi Pico!

## What Changed?

The Switch controller emulation code has been extracted into a standalone library at `lib/switch_controller_pico/`. This allows you to use the Switch emulation in your own Pico projects with any input source (sensors, buttons, keyboards, other controllers, etc.).

### Project Structure

```
retro-pico-switch/
├── lib/switch_controller_pico/    # NEW: Reusable library
│   ├── include/                    # Library headers
│   ├── src/                        # Library implementation
│   ├── examples/                   # Example programs
│   │   ├── CMakeLists.txt         # Build examples
│   │   └── README.md              # Example documentation
│   ├── auto_press_example.cpp     # Auto press A every 5 seconds
│   ├── example.cpp                # Simple GPIO button example
│   ├── CMakeLists.txt             # Library build configuration
│   └── README.md                  # Library documentation
├── src/
│   ├── main.cpp                   # UPDATED: Uses new library API
│   ├── otherController/           # N64/Gamecube controller code (unchanged)
│   └── switchController/          # OLD: No longer used (kept for reference)
└── include/                       # Shared headers (Controller.h, etc.)
```

### Backward Compatibility

The existing retro-pico-switch functionality is **100% preserved**. The project still works exactly as before for N64 and Gamecube controllers. The main.cpp has been refactored to use the new library, but the behavior is identical.

## Using the Library in Your Own Projects

### Quick Start

1. **Copy the library to your project:**
   ```bash
   cp -r lib/switch_controller_pico /path/to/your/project/lib/
   ```

2. **Add to your CMakeLists.txt:**
   ```cmake
   add_subdirectory(lib/switch_controller_pico)

   # Link against USB variant
   target_link_libraries(your_project switch_controller_pico_usb)

   # Or Bluetooth variant (Pico W only)
   # target_link_libraries(your_project switch_controller_pico_bluetooth)
   ```

3. **Implement the input provider:**
   ```cpp
   #include "switch_controller_pico.h"

   class MyInput : public SwitchInputProvider {
     void getSwitchReport(SwitchReport* report) override {
       // Fill report with your button/stick data
     }

     void setRumble(bool enabled) override {
       // Optional: handle rumble
     }
   };
   ```

4. **Initialize and run:**
   ```cpp
   int main() {
     stdio_init_all();

     MyInput input;
     SwitchControllerPico controller(&input, SwitchControllerPico::USB);

     controller.init();

     while (1) {
       controller.update();
     }
   }
   ```

### Full Documentation

See `lib/switch_controller_pico/README.md` for:
- Detailed API reference
- Button mapping guide
- Analog stick format
- USB and Bluetooth examples
- Advanced usage

### Example Projects

- **Auto press A button**: `lib/switch_controller_pico/auto_press_example.cpp` - Presses A every 5 seconds (great for testing!)
- **Simple GPIO buttons**: `lib/switch_controller_pico/example.cpp` - Physical buttons on GPIO pins
- **N64/Gamecube adapters**: `src/main.cpp` - Full retro controller adapter (this project)

Build examples: `cd lib/switch_controller_pico/examples && mkdir build && cd build && cmake .. && make`

## Library Features

- ✅ **Simple API** - Just implement 2 methods: `getSwitchReport()` and `setRumble()`
- ✅ **USB and Bluetooth** - Full support for both connection modes
- ✅ **Pico-optimized** - Leverages TinyUSB and BTstack from Pico SDK
- ✅ **Complete emulation** - Handles all Switch subcommands, calibration, player lights, etc.
- ✅ **Flexible input** - Works with any input source (buttons, sensors, other controllers)
- ✅ **Well-documented** - Extensive comments and examples

## Technical Details

### What's in the Library?

- **SwitchCommon**: Core protocol logic (subcommands, SPI emulation, IMU, etc.)
- **SwitchUsb**: USB HID implementation
- **SwitchBluetooth**: Bluetooth HID implementation
- **switch_controller_pico**: Main API wrapper with simple interface

### Dependencies

The library depends on:
- **Pico SDK** (pico_stdlib, hardware_pio)
- **TinyUSB** (for USB mode)
- **BTstack** (for Bluetooth mode, Pico W only)
- **Controller.h** (from parent project, for internal adapter)

### Build Variants

The library provides two build variants:
- **switch_controller_pico_usb** - USB-only (works on any Pico)
- **switch_controller_pico_bluetooth** - Bluetooth support (requires Pico W)

Link against the appropriate variant based on your needs.

## Migration Notes

If you're maintaining the retro-pico-switch project:

1. **Old code location**: `src/switchController/` (deprecated, can be removed)
2. **New code location**: `lib/switch_controller_pico/` (active)
3. **Main.cpp changes**: Now uses library API with `ControllerAdapter` bridge class
4. **CMakeLists changes**: Links against library instead of switchController subdirectory

The old `switchController` directory can be safely deleted if desired. It's kept for now to avoid breaking any custom forks.

## Future Improvements

Potential enhancements for the library:
- [ ] Standalone distribution (separate git repository)
- [ ] Platform abstraction for non-Pico boards
- [ ] Unit tests for protocol logic
- [ ] Support for Joy-Con emulation
- [ ] Real IMU sensor integration
- [ ] Custom calibration data

## Questions?

- **Library usage**: See `lib/switch_controller_pico/README.md`
- **Example code**: See `lib/switch_controller_pico/example.cpp`
- **This project**: See main `README.md`

## License

Same license as the parent project (see LICENSE file).
