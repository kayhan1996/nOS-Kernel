#ifndef CANVAS_H
#define CANVAS_H

#include "stdint.h"

/* C++ Declarations */
#ifdef __cplusplus

extern "C" {
    #include "Libraries/printx.h"
}


struct Color_t {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
public:
    void setColor(int red, int green, int blue){
        this->red = red;
        this->green = green;
        this->blue = blue;
    }

    uint32_t getRBGA(){
        uint32_t tmp = red << 8*4 + green << 8*3 + blue << 8*2;
        printf("Writing %x\n", tmp);
        return tmp;
    }
};

class Canvas
{
private:
    uint32_t *framebuffer;
    uint32_t pitch;
    int x, y;
    bool pen_down;
    Color_t color;
public:
    Canvas(/* args */);
    ~Canvas();

    void Draw();
    void Begin();
    void End();
    void point(int x, int y);
    void lineTo(int x, int y);
    void moveTo(int x, int y);
    void drawRect(int x, int y, int width, int height);
};


#endif

/* C declarations */
#ifdef __cplusplus
extern "C" {
#endif

void testCanvas();

#ifdef __cplusplus
}
#endif



#endif /* CANVAS_H */
