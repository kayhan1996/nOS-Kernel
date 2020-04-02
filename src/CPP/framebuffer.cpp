#include "Drivers/framebuffer.h"

extern "C" {
#include "Drivers/mailbox.h"
#include "Libraries/printx.h"
#include "Drivers/asm.h"
#include "Drivers/timer.h"
}

#ifdef DEBUG            
    #define kprintf         tfp_printf
#else
    #define kprintf         
#endif

int width;
int height;
int pitch;
void *framebuffer;

void *buffer1;
void *buffer2;

typedef union {
    struct {
        uint8_t red;
        uint8_t green;
        uint8_t blue;
    };
    uint32_t data;
} Pixel;

void init_framebuffer(){
    uint32_t message[35];

    message[0] = 35*4;  //size
    message[1] = 0;     //request

    message[2] = 0x48003;
    message[3] = 8;
    message[4] = 8;
    message[5] = 640;
    message[6] = 480;

    message[7] = 0x48004;
    message[8] = 8;
    message[9] = 8;
    message[10] = 641;
    message[11] = 480*2;

    message[12] = 0x48009;
    message[13] = 8;
    message[14] = 8;
    message[15] = 0;
    message[16] = 0;

    message[17] = 0x48005;
    message[18] = 4;
    message[19] = 4;
    message[20] = 32;

    message[21] = 0x48006;
    message[22] = 4;
    message[23] = 4;
    message[24] = 1;

    message[25] = 0x40001;
    message[26] = 8;
    message[27] = 8;
    message[28] = 4096;
    message[29] = 0;

    message[30] = 0x40008;
    message[31] = 4;
    message[32] = 4;
    message[33] = 0;

    message[34] = 0;

    int status = call_mailbox(message, 8);

    if(status && message[20] == 32 && message[28] != 0){
        
        width = message[5];
        height = message[6];
        pitch = message[33];

        buffer1 = (void *)(message[28] & 0x3FFFFFFF);
        buffer2 = (void *)((long)buffer1 + (width+1)*height*4);

        framebuffer = buffer1;

        printf("Width: %d\n", message[5]);
        printf("height: %d\n", message[6]);
        printf("pitch: %d\n", pitch);

        printf("buffer 1: %d\n", buffer1);
        printf("buffer 2: %d\n", buffer2);

        printf("framebuffer: 0x%x\n", framebuffer);

    }else{
        printf("Framebuffer intitialization error!\n");
    }
    
}

void draw_pixel(int x, int y, Pixel color){
    Pixel *ctx = (Pixel *)((long)framebuffer + (y*pitch) + (x*4));
    *ctx = color;
}

void draw_rect(int x_pos, int y_pos, int width, int height, Pixel color){
    for(int y = y_pos; y < (y_pos+height); y++){
        for(int x = x_pos; x < (x_pos+width); x++){
            draw_pixel(x, y, color);
        }
    }
}

void color_effects(){
    Pixel p;
    int square = 10;
    int x = 0, y = 0, width = square, height = square;
    for(int red = 0; red < 255; red += 15){
        for(int green = 0; green < 255; green += 15){
            for(int blue = 0; blue < 255; blue += 15){
                p.red = red;
                p.blue = blue;
                p.green = green;

                draw_rect(x, y, width, height, p);
                x+= square;
                if(x > 750){
                    x = 0;
                    y += square;
                }
            }
        }
    }
}

void Clear(){
    Pixel black;
    black.red = 0;
    black.green = 0;
    black.blue = 30;
    draw_rect(0, 0, width, height, black);
}

void set_offset(int x, int y){
    static int y_int = 0;
    uint32_t message[8];
    message[0] = 8*4;

    message[1] = 0;
    message[2] = 0x48009;
    message[3] = 8;
    message[4] = 8;
    message[5] = x;
    message[6] = y;

    message[7] = 0;

    call_mailbox(message, 8);
}

void movingRectangle(){
    Clear();
    static int x = 0;

    Pixel p;
    p.blue = 0;
    p.green = 255;
    p.red = 255;

    draw_rect(x++, 100, 50, 50, p);

    if(x > 500){
        x = 0;
    }

    kprintf("Here!\n");
}

void double_buffer(void (*draw)(void)){
    printf("Double buffered drawing\n");
    
    int delayTime = 100000000;

    int buffer = 0;

    while(1){
        if(buffer == 0){
            framebuffer = buffer2;
            set_offset(0, 0);
            kprintf("buffer 0\n");
        }else{
            framebuffer = buffer1;
            set_offset(0, 640);
            kprintf("buffer 1\n");
        } 

        draw();

        buffer = !buffer;
        
        delay_ms(16);   //WARNING: Might be broken

        //delayTime -= 1000000;

        kprintf("delay time: %d\n", delayTime);
    }
}

void* get_frame_buffer(){
    return framebuffer;
}

uint32_t get_pitch(){
    return pitch;
}

void draw_test(){

    double_buffer(movingRectangle);

    kprintf("Created drawing!\n");
    while(1){asm("nop");}
}