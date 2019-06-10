#include "canvas.h"

extern "C" {
    #include "framebuffer.h"
}



void testCanvas(){
    Canvas ctx;

    printf("Hadoop!");

    ctx.Begin();
    ctx.lineTo(100, 100);
    ctx.lineTo(200, 500);
    ctx.drawRect(0, 0, 100, 100);
}

Canvas::Canvas(){
    framebuffer = (uint32_t*)get_frame_buffer();
    pitch = get_pitch();
    x = 0;
    y = 0;
    pen_down = false;

    color.red = 0;
    color.blue = 0;
    color.green = 255;
}

Canvas::~Canvas(){
}

void Canvas::Begin(){
    pen_down = true;
}

void Canvas::point(int x, int y){
    uint32_t* ctx = framebuffer + (y*pitch) + (x*4);
    *ctx = 0xffff0000;
}

void Canvas::lineTo(int x, int y){
    int slope = (this->y - y)/(this->x - x);

    while(this->x++ < x){
        this->y += slope;
        point(this->x, this->y);
    }
}

void Canvas::drawRect(int x, int y, int width, int height){
    for(int y_pos = x; y_pos < (y + height); y++){
        for(int x_pos = y; x_pos < (x + width); x++){
            point(x_pos, y_pos);
        }
    }
}

void Canvas::Draw(){

}