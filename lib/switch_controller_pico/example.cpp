/**
 * Example: Simple Switch Controller with Button Inputs
 *
 * This example shows how to use the Switch Controller Pico library
 * with simple GPIO button inputs instead of N64/Gamecube controllers.
 *
 * Hardware setup:
 * - Connect buttons to GPIO pins with pull-down resistors
 * - Buttons connect GPIO to 3.3V when pressed
 */

#include "switch_controller_pico.h"
#include "pico/stdlib.h"

// Define GPIO pins for buttons
#define PIN_BUTTON_A 2
#define PIN_BUTTON_B 3
#define PIN_BUTTON_X 4
#define PIN_BUTTON_Y 5
#define PIN_BUTTON_L 6
#define PIN_BUTTON_R 7
#define PIN_BUTTON_START 8
#define PIN_JOYSTICK_UP 9
#define PIN_JOYSTICK_DOWN 10
#define PIN_JOYSTICK_LEFT 11
#define PIN_JOYSTICK_RIGHT 12

/**
 * Simple input provider that reads from GPIO buttons.
 */
class SimpleButtonInput : public SwitchInputProvider {
 public:
  SimpleButtonInput() {
    // Initialize button pins as inputs with pull-downs
    gpio_init(PIN_BUTTON_A);
    gpio_set_dir(PIN_BUTTON_A, GPIO_IN);
    gpio_pull_down(PIN_BUTTON_A);

    gpio_init(PIN_BUTTON_B);
    gpio_set_dir(PIN_BUTTON_B, GPIO_IN);
    gpio_pull_down(PIN_BUTTON_B);

    gpio_init(PIN_BUTTON_X);
    gpio_set_dir(PIN_BUTTON_X, GPIO_IN);
    gpio_pull_down(PIN_BUTTON_X);

    gpio_init(PIN_BUTTON_Y);
    gpio_set_dir(PIN_BUTTON_Y, GPIO_IN);
    gpio_pull_down(PIN_BUTTON_Y);

    gpio_init(PIN_BUTTON_L);
    gpio_set_dir(PIN_BUTTON_L, GPIO_IN);
    gpio_pull_down(PIN_BUTTON_L);

    gpio_init(PIN_BUTTON_R);
    gpio_set_dir(PIN_BUTTON_R, GPIO_IN);
    gpio_pull_down(PIN_BUTTON_R);

    gpio_init(PIN_BUTTON_START);
    gpio_set_dir(PIN_BUTTON_START, GPIO_IN);
    gpio_pull_down(PIN_BUTTON_START);

    // Joystick direction pins
    gpio_init(PIN_JOYSTICK_UP);
    gpio_set_dir(PIN_JOYSTICK_UP, GPIO_IN);
    gpio_pull_down(PIN_JOYSTICK_UP);

    gpio_init(PIN_JOYSTICK_DOWN);
    gpio_set_dir(PIN_JOYSTICK_DOWN, GPIO_IN);
    gpio_pull_down(PIN_JOYSTICK_DOWN);

    gpio_init(PIN_JOYSTICK_LEFT);
    gpio_set_dir(PIN_JOYSTICK_LEFT, GPIO_IN);
    gpio_pull_down(PIN_JOYSTICK_LEFT);

    gpio_init(PIN_JOYSTICK_RIGHT);
    gpio_set_dir(PIN_JOYSTICK_RIGHT, GPIO_IN);
    gpio_pull_down(PIN_JOYSTICK_RIGHT);
  }

  void getSwitchReport(SwitchReport *report) override {
    // Clear report
    report->batteryConnection = 0x91;  // Full battery
    report->buttons[0] = 0x00;
    report->buttons[1] = 0x00;
    report->buttons[2] = 0x00;

    // Read button states and set appropriate bits
    // See SwitchConsts.h for button bit definitions

    // Byte 0: Y, X, B, A, (unused), (unused), R, ZR
    if (gpio_get(PIN_BUTTON_Y)) report->buttons[0] |= 0x01;  // Y
    if (gpio_get(PIN_BUTTON_X)) report->buttons[0] |= 0x02;  // X
    if (gpio_get(PIN_BUTTON_B)) report->buttons[0] |= 0x04;  // B
    if (gpio_get(PIN_BUTTON_A)) report->buttons[0] |= 0x08;  // A
    if (gpio_get(PIN_BUTTON_R)) report->buttons[0] |= 0x40;  // R
    // For ZR, use R button as well (you could add another pin)
    if (gpio_get(PIN_BUTTON_R)) report->buttons[0] |= 0x80;  // ZR

    // Byte 1: Minus, Plus, RStick, LStick, Home, Capture, (unused), (unused)
    if (gpio_get(PIN_BUTTON_START)) report->buttons[1] |= 0x02;  // Plus (Start)

    // Byte 2: Down, Up, Right, Left, (unused), (unused), L, ZL
    if (gpio_get(PIN_BUTTON_L)) report->buttons[2] |= 0x40;  // L
    if (gpio_get(PIN_BUTTON_L)) report->buttons[2] |= 0x80;  // ZL

    // Set analog stick positions
    // Default to center (0x7FF)
    uint16_t leftX = 0x7FF;
    uint16_t leftY = 0x7FF;
    uint16_t rightX = 0x7FF;
    uint16_t rightY = 0x7FF;

    // Move left stick based on button presses
    if (gpio_get(PIN_JOYSTICK_LEFT)) leftX = 0x000;   // Full left
    if (gpio_get(PIN_JOYSTICK_RIGHT)) leftX = 0xFFF;  // Full right
    if (gpio_get(PIN_JOYSTICK_UP)) leftY = 0xFFF;     // Full up
    if (gpio_get(PIN_JOYSTICK_DOWN)) leftY = 0x000;   // Full down

    // Pack stick values into 3-byte format
    // Format: [X_low] [X_high(4bit)|Y_low(4bit)] [Y_high(8bit)]
    report->l[0] = leftX & 0xFF;
    report->l[1] = ((leftX >> 8) & 0x0F) | ((leftY & 0x0F) << 4);
    report->l[2] = (leftY >> 4) & 0xFF;

    report->r[0] = rightX & 0xFF;
    report->r[1] = ((rightX >> 8) & 0x0F) | ((rightY & 0x0F) << 4);
    report->r[2] = (rightY >> 4) & 0xFF;
  }

  void setRumble(bool enabled) override {
    // Optional: Control rumble motor
    // For example, turn on an LED or vibration motor
    // gpio_put(PIN_RUMBLE, enabled);
  }
};

int main() {
  stdio_init_all();

  // Create input provider
  SimpleButtonInput input;

  // Create Switch controller (USB mode)
  SwitchControllerPico controller(&input, SwitchControllerPico::USB);

  // Initialize
  controller.init();

  // Main loop (USB mode)
  while (1) {
    controller.update();
    sleep_ms(1);  // Small delay to reduce CPU usage
  }

  return 0;
}
