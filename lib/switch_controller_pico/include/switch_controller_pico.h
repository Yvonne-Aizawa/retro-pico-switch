/**
 * Switch Controller Pico Library
 *
 * A Raspberry Pi Pico library for emulating a Nintendo Switch Pro Controller.
 * Supports both USB and Bluetooth modes.
 *
 * Usage:
 *   1. Implement SwitchInputProvider to provide button/stick data
 *   2. Create SwitchControllerPico instance with your input provider
 *   3. Call init() once
 *   4. For USB: call update() in main loop
 *   5. For Bluetooth: library handles loop automatically
 */

#ifndef SWITCH_CONTROLLER_PICO_H
#define SWITCH_CONTROLLER_PICO_H

#include "SwitchCommon.h"
#include "SwitchConsts.h"

/**
 * Simple input provider interface for Switch controller emulation.
 * Implement this interface to provide your own input source.
 */
class SwitchInputProvider {
 public:
  /**
   * Fill the SwitchReport with current button and analog stick states.
   *
   * SwitchReport structure:
   *   - batteryConnection: Battery level/connection state (default: 0x91)
   *   - buttons[3]: Button states (see SwitchConsts.h for button masks)
   *   - l[3]: Left analog stick position (12-bit X/Y, center: 0x7FF)
   *   - r[3]: Right analog stick position (12-bit X/Y, center: 0x7FF)
   */
  virtual void getSwitchReport(SwitchReport *switchReport) = 0;

  /**
   * Called when the Switch enables/disables rumble.
   * Implement this to control rumble/vibration on your controller.
   */
  virtual void setRumble(bool rumble) = 0;

  virtual ~SwitchInputProvider() = default;
};

/**
 * Main Switch Controller Pico library class.
 * Handles USB or Bluetooth Pro Controller emulation.
 */
class SwitchControllerPico {
 public:
  enum Mode {
    USB,        // USB HID mode (default)
    BLUETOOTH   // Bluetooth HID mode (requires Pico W and SWITCH_BLUETOOTH defined)
  };

  /**
   * Create a Switch controller emulator.
   *
   * @param inputProvider Your input provider implementation
   * @param mode USB or BLUETOOTH mode
   */
  SwitchControllerPico(SwitchInputProvider *inputProvider, Mode mode = USB);

  /**
   * Initialize the controller emulation.
   * Call this once in your setup code.
   */
  void init();

  /**
   * Update the controller state (USB mode only).
   * Call this repeatedly in your main loop for USB mode.
   * For Bluetooth mode, this is handled automatically by btstack_run_loop_execute().
   */
  void update();

  /**
   * Get the underlying SwitchCommon instance for advanced usage.
   */
  SwitchCommon* getSwitchCommon() { return _switchCommon; }

  /**
   * Get the current mode.
   */
  Mode getMode() const { return _mode; }

 private:
  SwitchInputProvider *_inputProvider;
  SwitchCommon *_switchCommon;
  Mode _mode;
};

/**
 * Internal adapter class that bridges SwitchInputProvider to the Controller interface.
 * Users don't need to use this directly.
 */
class SwitchInputAdapter : public Controller {
 public:
  SwitchInputAdapter(SwitchInputProvider *provider);

  void init() override {}
  void getSwitchReport(SwitchReport *switchReport) override;
  void setRumble(bool rumble) override;

 protected:
  void updateState() override {}

 private:
  SwitchInputProvider *_provider;
};

#endif // SWITCH_CONTROLLER_PICO_H
