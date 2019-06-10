#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

void init_framebuffer();
void double_buffer(void (*draw)(void));
void draw_test();

void *get_frame_buffer();
uint32_t get_pitch();

#ifdef __cplusplus
}
#endif

#endif