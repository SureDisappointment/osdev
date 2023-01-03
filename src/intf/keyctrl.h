#pragma once

#include "io_port.h"
#include "key.h"

// led names
typedef enum
{
    caps_lock_led = 4,
    num_lock_led = 2,
    scroll_lock_led = 1
} key_led;

void keyctrl_init();
Key keyctrl_key_hit();
void keyctrl_reboot();
void keyctrl_set_repeat_rate(int speed, int delay);
void keyctrl_set_led(key_led led, bool on);
