#include "switch_controller_pico.h"
#include "SwitchUsb.h"
#include "SwitchBluetooth.h"
#include "pico/stdlib.h"

#ifdef SWITCH_BLUETOOTH
#include "btstack.h"
#include "BtStackUtils.h"
#endif

// Dummy InitParams for SwitchInputAdapter
static InitParams dummyInitParams = {
  .pin = 0,
  .controllerType = N64,
  .pio = nullptr,
  .sm = 0,
  .c = nullptr,
  .offset = 0
};

// SwitchInputAdapter implementation
SwitchInputAdapter::SwitchInputAdapter(SwitchInputProvider *provider)
    : Controller(&dummyInitParams, 0), _provider(provider) {
  // We don't use the PIO-based controller, so pass dummy InitParams
}

void SwitchInputAdapter::getSwitchReport(SwitchReport *switchReport) {
  _provider->getSwitchReport(switchReport);
}

void SwitchInputAdapter::setRumble(bool rumble) {
  _provider->setRumble(rumble);
}

// SwitchControllerPico implementation
SwitchControllerPico::SwitchControllerPico(SwitchInputProvider *inputProvider, Mode mode)
    : _inputProvider(inputProvider), _mode(mode), _switchCommon(nullptr) {
}

void SwitchControllerPico::init() {
  // Create adapter to bridge input provider to Controller interface
  static SwitchInputAdapter adapter(_inputProvider);

#ifdef SWITCH_BLUETOOTH
  if (_mode == BLUETOOTH) {
    _switchCommon = new SwitchBluetooth();
  } else {
    _switchCommon = new SwitchUsb();
  }
#else
  // USB mode only
  _switchCommon = new SwitchUsb();
#endif

  // Initialize the Switch controller with our adapter
  _switchCommon->init(&adapter);
}

void SwitchControllerPico::update() {
#ifndef SWITCH_BLUETOOTH
  // USB mode - process TinyUSB tasks
  // This should be called in the main loop
  extern void tud_task();
  tud_task();
#else
  // Bluetooth mode - nothing to do here, btstack_run_loop_execute() is blocking
#endif
}
