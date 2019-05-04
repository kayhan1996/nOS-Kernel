#include "LED.h"
#include "stdint.h"
#include "asm.h"

void act_led(led_state state){
    if(state == On){
        LED_ACT_ON();
    }else{
        LED_ACT_OFF();
    }
}