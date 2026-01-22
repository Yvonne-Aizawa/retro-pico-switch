/**
 * Minimal Controller implementation for the switch_controller_pico library.
 *
 * This provides a basic Controller base class without PIO functionality.
 * The library's SwitchInputAdapter uses this to bridge to the SwitchInputProvider
 * interface without needing the full PIO-based controller implementation.
 */

#include "Controller.h"

// Minimal constructor implementation for library use
Controller::Controller(InitParams *initParams, uint8_t sizeofControllerState) {
  _pin = initParams->pin;
  _pio = initParams->pio;
  _sm = initParams->sm;
  _c = initParams->c;
  _offset = initParams->offset;
  _sizeofControllerState = sizeofControllerState;
  _controllerState = nullptr; // Not used in library adapter
}

// Stub implementations for methods that aren't used by the library
void Controller::initPio(InitParams *initParams) {
  // Not used by library - stub implementation
}

void Controller::transfer(uint8_t *request, uint8_t requestLength,
                         uint8_t *response, uint8_t responseLength) {
  // Not used by library - stub implementation
}

void Controller::transfer(PIO pio, uint sm, uint8_t *request,
                         uint8_t requestLength, uint8_t *response,
                         uint8_t responseLength) {
  // Not used by library - stub implementation
}

void Controller::sendRequest(PIO pio, uint sm, uint8_t *request,
                            uint8_t requestLength) {
  // Not used by library - stub implementation
}

void Controller::getResponse(PIO pio, uint sm, uint8_t *response,
                            uint8_t responseLength) {
  // Not used by library - stub implementation
}

double Controller::getScaledAnalogAxis(double axisPos, double *minAxis,
                                      double *maxAxis) {
  // Not used by library - stub implementation
  return 0.0;
}
