#include <iostream>
#include <vector>
#include <cstdint>
#include <cassert>
#include "SDL.h"
using namespace std;
#include "utils.h"
#include "textures.h"

Texture::Texture(const std::string filename, const uint32_t format) : img_w(0), img_h(0), count(0), size(0), img() {
    SDL_Surface* tmp = SDL_LoadBMP(filename.c_str());
    if (!tmp) {
        cerr << "Error in SDL_LoadBMP: " << SDL_GetError() << endl;
        return;
    }

    SDL_Surface* surface = SDL_ConvertSurfaceFormat(tmp, format, 0);    // make SDL_PIXELFORMAT_ABGR8888 texture format
    SDL_FreeSurface(tmp);
    if (!surface) {
        cerr << "Error in SDL_ConvertSurfaceFormat: " << SDL_GetError() << endl;
        return;
    }
    int w = surface->w;
    int h = surface->h;

    //check if loaded image is 32bit -- each pixel is 4 bytes then
    //SDL defines pitch as "the length of a row of pixels in bytes"
    if (w * 4 != surface->pitch) {
        cerr << "Error, loaded BMP image needs to be 32 bit" << endl;
        SDL_FreeSurface(surface);
        return;
    }

    if (w != h * int(w / h)) {
        cerr << "Error: the texture file must contain N square textures packed horizontally" << endl;
        SDL_FreeSurface(surface);
        return;
    }

    count = w / h;
    size = w / count;
    img_w = w;
    img_h = h;
    uint8_t* pixmap = reinterpret_cast<uint8_t*>(surface->pixels);

    // use loaded image's pixels to make new image that will be used
    img = std::vector<uint32_t>(w * h);
    for (short j = 0; j < h; j++) {
        for (short i = 0; i < w; i++) {
            uint8_t r = pixmap[(i + j * w) * 4 + 0];
            uint8_t g = pixmap[(i + j * w) * 4 + 1];
            uint8_t b = pixmap[(i + j * w) * 4 + 2];
            uint8_t a = pixmap[(i + j * w) * 4 + 3];
            img[i + j * w] = pack_color(r, g, b, a);
        }
    }
    SDL_FreeSurface(surface);


}

uint32_t Texture::get(const short i, const short j, const short idx) const{
    assert(i < size && j < size && idx < count);
    return img[i + idx * size + j * img_w];
}

  vector<uint32_t> Texture::get_scaled_column(const short texture_id, const short tex_coord, const short column_height) const{
    assert(tex_coord < size && texture_id < count);
     vector<uint32_t> column(column_height);
    for (short y = 0; y < column_height; y++) {
        column[y] = get(tex_coord, (y * size) / column_height, texture_id);
    }
    return column;
}

