#include <Drivers/GPIO.h>
#include <Libraries/types.h>

/*  Selects the function of a pin using either general purpose IO
    or one of the alternative modes listed in the BCM2835 manual
    Requires: Valid pin in range [0, 53], GPIO Function specifier
    Returns: -1 on failure, 0 on success */
int GPIO_set_function(int pin, enum GPIOFunc func){

    if(pin < 0 || pin > 53){
        return -1;
    }

    u32 offset = pin % 10;  
    u32 regoffset = ((pin - offset)/10);    //Find which register bank this pin belongs to
    volatile u32 *GPFSEL = GPIO_BASE + (regoffset * sizeof(u32));

    offset = 3*offset;              //Find offset of pin in the function select register

    u32 reg = *GPFSEL;
    reg &= ~(0b111 << offset);      //Clear function at pin
    reg |= func << offset;          //Set new function
    *GPFSEL = reg;

    return 0;
}

/*  Sets the pin pull-up or pull-down mode and performs the steps
    needed for GPIO pin initialization
    Requires: Valid pin in range [0,53], GPIO Pull Mode
    Returns: -1 on failure, 0 on success */
int GPIO_set_pull_mode(int pin, enum GPIOPullMode mode){
    GPIO->PUD = mode;            
    
    delay(150);

    if(pin < 0){
        return -1;
    }else if(pin < 32){
        GPIO->PUDCLK0 = (1<<pin);
        delay(150);
        GPIO->PUD = 0;
        GPIO->PUDCLK0 = 0;
        return 0; 
    }else if(pin < 54){
        GPIO->PUDCLK1 = (1<<pin);
        delay(150);
        GPIO->PUD = 0;
        GPIO->PUDCLK1 = 0;
        return 0; 
    }else{
        return -1;
    }        
}