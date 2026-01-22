/**
 * Example: Auto Press A Button Every 5 Seconds
 *
 * This example demonstrates using the Switch Controller Pico library
 * to automatically press the A button every 5 seconds. Useful for:
 * - Testing Switch controller connectivity
 * - Auto-farming in games
 * - Demonstrating the library API
 *
 * Hardware: Any Raspberry Pi Pico (USB mode)
 * Connection: Connect Pico to Switch via USB cable
 */

#include "switch_controller_pico.h"
#include "pico/stdlib.h"
#include <stdio.h>

/**
 * Simple input provider that presses A button every 5 seconds.
 */
class AutoPressInput : public SwitchInputProvider {
 public:
  AutoPressInput() : _lastPressTime(0), _buttonPressed(false) {}

  void getSwitchReport(SwitchReport* report) override {
    // Clear all buttons and set default values
    report->batteryConnection = 0x91;  // Full battery, connected
    report->buttons[0] = 0x00;
    report->buttons[1] = 0x00;
    report->buttons[2] = 0x00;

    // Get current time in milliseconds
    uint32_t currentTime = to_ms_since_boot(get_absolute_time());

    // Check if it's time to press or release the button
    if (_buttonPressed) {
      // Button is currently pressed, check if we should release it
      if (currentTime - _lastPressTime >= PRESS_DURATION_MS) {
        _buttonPressed = false;
        printf("A button released\n");
      } else {
        // Keep button pressed
        report->buttons[0] |= 0x08;  // SWITCH_A = 0x08
      }
    } else {
      // Button is not pressed, check if we should press it
      if (currentTime - _lastPressTime >= PRESS_INTERVAL_MS) {
        _buttonPressed = true;
        _lastPressTime = currentTime;
        report->buttons[0] |= 0x08;  // SWITCH_A = 0x08
        printf("A button pressed\n");
      }
    }

    // Set analog sticks to center position (0x7FF = center for 12-bit value)
    uint16_t centerX = 0x7FF;
    uint16_t centerY = 0x7FF;

    // Pack left stick values into 3-byte format
    // Format: [X_low] [X_high(4bit)|Y_low(4bit)] [Y_high(8bit)]
    report->l[0] = centerX & 0xFF;
    report->l[1] = ((centerX >> 8) & 0x0F) | ((centerY & 0x0F) << 4);
    report->l[2] = (centerY >> 4) & 0xFF;

    // Pack right stick values (also centered)
    report->r[0] = centerX & 0xFF;
    report->r[1] = ((centerX >> 8) & 0x0F) | ((centerY & 0x0F) << 4);
    report->r[2] = (centerY >> 4) & 0xFF;
  }

  void setRumble(bool enabled) override {
    // Optional: respond to rumble
    if (enabled) {
      printf("Rumble enabled\n");
    } else {
      printf("Rumble disabled\n");
    }
  }

 private:
  static const uint32_t PRESS_INTERVAL_MS = 5000;  // 5 seconds between presses
  static const uint32_t PRESS_DURATION_MS = 100;   // Hold button for 100ms

  uint32_t _lastPressTime;
  bool _buttonPressed;
};

int main() {
  // Initialize stdio for printf output
  stdio_init_all();

  printf("\n=== Switch Controller Auto Press Example ===\n");
  printf("This will press the A button every 5 seconds\n");
  printf("Connect your Pico to the Switch via USB\n\n");

  // Create input provider
  AutoPressInput input;

  // Create Switch controller in USB mode
  SwitchControllerPico controller(&input, SwitchControllerPico::USB);

  printf("Initializing Switch controller...\n");
  controller.init();
  printf("Controller initialized! Waiting for Switch to connect...\n\n");

  // Main loop - process USB events
  while (1) {
    controller.update();
    sleep_ms(1);  // Small delay to reduce CPU usage
  }

  return 0;
}
