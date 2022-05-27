#include <iostream>
#include <vector>
#include <cstdint>
#include <cassert>
using namespace std;
#include "framebuffer.h"
#include "utils.h"


FrameBuffer::FrameBuffer()
    : w(1000), h(900), img(1920 * 1080, pack_color(255, 255, 255)) {}

void FrameBuffer::set_pixel(const short x, const short y, const uint32_t color) {
    img[x + y * w ] = color;
}

void FrameBuffer::draw_rectangle(const short rect_x, const short rect_y, const short rect_w, const short rect_h, const uint32_t color) {
    assert(img.size() == w * h);
    for (short i = 0; i < rect_w; i++) {
        for (short j = 0; j < rect_h; j++) {
            short cx = rect_x + i;
            short cy = rect_y + j;
            if (cx < w && cy < h) // проверять отрицательность не нужно т.к. тип данных располагает (unsigned variables)
                set_pixel(cx, cy, color);
        }
    }   
}

void FrameBuffer::clear(const uint32_t color) {
    img = vector<uint32_t>(w * h, color);
}