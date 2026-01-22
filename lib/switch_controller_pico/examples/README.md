# Switch Controller Pico Library - Examples

This directory contains example programs demonstrating how to use the Switch Controller Pico library.

## Available Examples

### 1. Auto Press Example (`auto_press_example.cpp`)
Automatically presses the A button every 5 seconds.

**Features:**
- No external hardware required (besides USB cable)
- Demonstrates basic button pressing
- Shows timing logic for periodic actions
- Useful for testing Switch connectivity
- Great for auto-farming in games

**How to use:**
1. Build the example (see below)
2. Flash `auto_press_example.uf2` to your Pico
3. Connect Pico to Switch via USB
4. Watch the A button get pressed automatically every 5 seconds

### 2. Simple Button Example (`example.cpp`)
Uses GPIO pins with physical buttons to control the Switch.

**Hardware needed:**
- Buttons connected to GPIO pins 2-12
- Pull-down resistors (or use internal pull-downs)

**Features:**
- Real button input from GPIO
- Demonstrates analog stick simulation
- Shows proper button mapping

## Building Examples

### Option 1: Build from Library Directory

```bash
cd lib/switch_controller_pico/examples
mkdir build
cd build
cmake ..
make
```

This will create:
- `auto_press_example.uf2`
- `simple_button_example.uf2`

### Option 2: Build from Main Project

Add this to your project's main `CMakeLists.txt`:

```cmake
add_subdirectory(lib/switch_controller_pico/examples)
```

Then build normally:
```bash
mkdir build
cd build
cmake ..
make auto_press_example
```

## Flashing to Pico

1. Hold BOOTSEL button while plugging in Pico
2. Copy the `.uf2` file to the RPI-RP2 drive
3. Pico will reboot and run the example

## Connecting to Switch

### USB Mode (Both Examples)
1. Flash the example to your Pico
2. Connect Pico to Switch via USB cable (micro-USB on Pico)
3. Go to Controllers → Change Grip/Order on Switch
4. The controller should be detected automatically

### Debugging Output

Both examples use USB serial for debug output. To view:

**Linux/macOS:**
```bash
screen /dev/ttyACM0 115200
```

**Windows:**
Use PuTTY or Arduino Serial Monitor on the appropriate COM port.

## Customizing the Auto Press Example

Edit `auto_press_example.cpp` to customize behavior:

```cpp
// Change press interval (default: 5000ms = 5 seconds)
static const uint32_t PRESS_INTERVAL_MS = 3000;  // 3 seconds

// Change button hold duration (default: 100ms)
static const uint32_t PRESS_DURATION_MS = 200;   // 200ms

// Press different buttons (in getSwitchReport method)
report->buttons[0] |= 0x08;  // A button
// Change to:
report->buttons[0] |= 0x01;  // Y button
report->buttons[0] |= 0x02;  // X button
report->buttons[0] |= 0x04;  // B button
report->buttons[1] |= 0x02;  // Plus (Start)
```

See `include/SwitchConsts.h` for all button masks.

## Creating Your Own Example

Use the auto press example as a template:

1. Create a new `.cpp` file
2. Implement `SwitchInputProvider`:
   ```cpp
   class MyInput : public SwitchInputProvider {
     void getSwitchReport(SwitchReport* report) override {
       // Your logic here
     }
     void setRumble(bool enabled) override {
       // Optional
     }
   };
   ```
3. In `main()`:
   ```cpp
   MyInput input;
   SwitchControllerPico controller(&input, SwitchControllerPico::USB);
   controller.init();
   while(1) controller.update();
   ```
4. Add to `CMakeLists.txt`:
   ```cmake
   add_executable(my_example my_example.cpp)
   target_link_libraries(my_example pico_stdlib switch_controller_pico_usb)
   pico_add_extra_outputs(my_example)
   ```

## Troubleshooting

**Switch doesn't detect controller:**
- Make sure Pico is properly flashed
- Try unplugging and reconnecting
- Go to Controllers → Change Grip/Order on Switch
- Check that USB cable supports data (not just power)

**Button presses don't work:**
- Verify controller is connected (LED on Switch controller icon)
- Check serial output for "Button pressed" messages
- Make sure you're setting the correct button bits

**Build errors:**
- Ensure PICO_SDK_PATH is set correctly
- Make sure you've initialized SDK submodules
- Verify the library is in `lib/switch_controller_pico/`

## Next Steps

- Modify the auto press example for your specific use case
- Combine multiple button presses
- Add analog stick movement
- Create macros or sequences of inputs
- Add sensor input (accelerometer, gyro, etc.)

See the main library README for full API documentation.
