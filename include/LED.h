#ifndef LED_H
#define LED_H

typedef enum {
    On,
    Off
} led_state;

void act_led(led_state state);

#endif