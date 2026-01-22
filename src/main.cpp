#include <stdio.h>
#include <string.h>

#include "GamecubeController.h"
#include "N64Controller.h"
#include "switch_controller_pico.h"
#include "pico/stdlib.h"

#ifdef SWITCH_BLUETOOTH
#include "SwitchBluetooth.h"
#include "btstack.h"
static btstack_packet_callback_registration_t hci_event_callback_registration;
SwitchControllerPico *g_switchController = nullptr;

// Bluetooth callback wrappers
static void packet_handler_wrapper(uint8_t packet_type, uint16_t channel,
                                   uint8_t *packet, uint16_t packet_size) {
  packet_handler((SwitchBluetooth *)g_switchController->getSwitchCommon(),
                 packet_type, packet);
}

static void hid_report_data_callback_wrapper(uint16_t cid,
                                             hid_report_type_t report_type,
                                             uint16_t report_id,
                                             int report_size, uint8_t *report) {
  // USB report callback includes 2 bytes excluded here, prepending 2 bytes to
  // keep alignment
  hid_report_data_callback(g_switchController->getSwitchCommon(), report_id,
                           report - 1, report_size + 1);
}
#else
#include "SwitchUsb.h"
#include "tusb.h"
SwitchControllerPico *g_switchController = nullptr;

// USB callback wrapper
void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id,
                           hid_report_type_t report_type, uint8_t const *buffer,
                           uint16_t bufsize) {
  hid_report_data_callback(g_switchController->getSwitchCommon(),
                           (uint16_t)buffer[0], (uint8_t *)buffer, bufsize);
}
#endif

/**
 * Adapter class that bridges legacy Controller to SwitchInputProvider.
 * This allows the existing N64/Gamecube controller classes to work
 * with the new library API without modification.
 */
class ControllerAdapter : public SwitchInputProvider {
 public:
  ControllerAdapter(Controller *controller) : _controller(controller) {}

  void getSwitchReport(SwitchReport *switchReport) override {
    _controller->getSwitchReport(switchReport);
  }

  void setRumble(bool rumble) override {
    _controller->setRumble(rumble);
  }

 private:
  Controller *_controller;
};

int main() {
  stdio_init_all();

  // Initialize legacy controller (N64 or Gamecube)
  InitParams *initParams = new InitParams();
  initParams->pin = 18;
  Controller::initPio(initParams);
  Controller *controller;

  if (initParams->controllerType == N64) {
    controller = new N64Controller(initParams);
  } else if (initParams->controllerType == Gamecube) {
    controller = new GamecubeController(initParams);
  }

  controller->init();

  // Create adapter and Switch controller
  ControllerAdapter adapter(controller);

#ifdef SWITCH_BLUETOOTH
  SwitchControllerPico switchController(&adapter,
                                        SwitchControllerPico::BLUETOOTH);
#else
  SwitchControllerPico switchController(&adapter, SwitchControllerPico::USB);
#endif

  g_switchController = &switchController;
  switchController.init();

#ifdef SWITCH_BLUETOOTH
  // Setup Bluetooth callbacks
  hci_event_callback_registration.callback = &packet_handler_wrapper;
  hci_add_event_handler(&hci_event_callback_registration);

  hid_device_register_packet_handler(&packet_handler_wrapper);
  hid_device_register_report_data_callback(&hid_report_data_callback_wrapper);

  // Turn on Bluetooth
  hci_power_control(HCI_POWER_ON);
  btstack_run_loop_execute();  // Blocking call
#else
  // USB mode main loop
  while (1) {
    switchController.update();
  }
#endif
}
