#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <vector>
#include <cstdint>
#include <cassert>
#include <sstream>
#include <iomanip>
#include <algorithm>

#include "map.h"
#include "utils.h"
#include "player.h"
#include "framebuffer.h"
#include "textures.h"
#include "sprite.h"
#include "render.h"
using namespace std;

int wall_x_texcoord(const double wallX, const Texture& tex_walls) {
    int x_texcoord = static_cast<int>(wallX * tex_walls.size);

    if (x_texcoord < 0) x_texcoord += static_cast<int>(tex_walls.size); 
    assert(x_texcoord >= 0 && x_texcoord < static_cast<int>(tex_walls.size));
    return x_texcoord;
}




void draw_sprite(FrameBuffer& fb, const Sprite& sprite, const std::vector<float>& depth_buffer, Player& player, const Texture& tex_sprites) {
  
    float sprite_dir = atan2(sprite.y - player.get_y(), sprite.x - player.get_x());
    while (sprite_dir - player.get_angle() > static_cast<float>(M_PI)) sprite_dir -= 2 * static_cast<float>(M_PI); 
    while (sprite_dir - player.get_angle() < static_cast<float>(-M_PI)) sprite_dir += 2 * static_cast<float>(M_PI);

    size_t sprite_screen_size = min(2000, static_cast<int>(fb.h / sprite.player_dist / 2)); // screen sprite size
    int h_offset = static_cast<int>((sprite_dir - player.get_angle()) / player.get_fov() * (fb.w )  + (fb.w) / 2 - tex_sprites.size / 2); 
    int v_offset = static_cast<int>(fb.h / 2 - sprite_screen_size / 10);

    uint32_t color;
    uint8_t r, g, b, a;
    for (short i = 0; i < sprite_screen_size; ++i) {
     if (h_offset + int(i) < 0 || h_offset + i >= fb.w  ) continue;
     if (depth_buffer[h_offset + i] < sprite.player_dist) continue; 
        for (short j = 0; j < sprite_screen_size; ++j) {
            if (v_offset + int(j) < 0 || v_offset + j >= fb.h) continue;
            color = tex_sprites.get(i * tex_sprites.size / sprite_screen_size, j * tex_sprites.size / sprite_screen_size, sprite.tex_id);
            unpack_color(color, r, g, b, a);
            if (a > 128)
                fb.set_pixel(fb.w  + h_offset + i, v_offset + j, color);


            
        }
    }
}





void render(FrameBuffer& fb, const GameState& gs, Player& player) {
    const Map& map = gs.map;
    const vector<Sprite>& sprites = gs.monsters;
    const vector<Sprite>& sprites2 = gs.doors;

    const Texture& tex_walls = gs.tex_walls;
    const Texture& tex_monst = gs.tex_monst;
    const Texture& tex_doors = gs.tex_doors;
  
    //при использовании этой строчки меньше шанс ошибки переполнения вектора
    // но частота кадров заметно ниже. хз короче че делать
    //fb.clear(pack_color(255, 255, 255)); // очистка экрана



    // рисование стены
    vector<float> depth_buffer(fb.w  , 1e3);
    for (size_t i = 0; i < fb.w ; ++i) { // draw the visibility cone AND the "3D" view
        float angle = player.get_angle() - player.get_fov() / 2 + player.get_fov() * i / float(fb.w );

        float rayDirX = cos(angle);
        float rayDirY = sin(angle);
        float distance = 0;
        int stepX = 0, stepY = 0;
        

        float sideDistX, sideDistY;
        // square (x,y) in which player is
        size_t mapX = int(player.get_x());
        size_t mapY = int(player.get_y());
        bool side;
        size_t texid;
        
        bool hit = false;

        //начальный расчет следующего квадрата
        if (rayDirX > 0) {
            stepX = 1;
            sideDistX = (mapX + static_cast<float>(1) - player.get_x()) / rayDirX;
        }
        else {
            stepX = -1;
            sideDistX = (player.get_x() - mapX) / static_cast<float>(cos(angle - M_PI));
        }
        if (rayDirY > 0) {
            stepY = 1;
            sideDistY = (mapY + static_cast<float>(1) - player.get_y()) / rayDirY;
        }
        else {
            stepY = -1;
            sideDistY = (player.get_y() - mapY) / static_cast<float>(sin(angle - M_PI));
        }
        //using DDA (Digital Differential Analysis) algorithm 
        while (!hit) {
            if (sideDistX < sideDistY) {
                sideDistX += std::abs(1 / rayDirX);
                mapX += stepX;
                side = false;
            }
            else {
                sideDistY += std::abs(1 / rayDirY);
                mapY += stepY;
                side = true;
            }

            if (!map.is_empty(mapX, mapY)) {
                hit = true;
                texid = map.get(mapX, mapY);
            }
        }

        if (side) {   // если врезались в горизонтальную стену
            distance = (mapY - player.get_y() + (1 - stepY) / 2) / rayDirY;
            
        }
        else {       //удар по вертикальной стене
            distance = (mapX - player.get_x() + (1 - stepX) / 2) / rayDirX;
            
        }
        //вычисление высоты линии для рисования на экране
        int lineHeight = static_cast<int>((fb.h / (distance * cos(angle - player.get_angle()))));
        depth_buffer[i] = distance * cos(angle - player.get_angle());

        //calculate value of wallX
        float wallX; //где вы ударились в стену
        
        if (side == 0) {
            wallX = player.get_y() + distance * rayDirY;
        }
        else {
            wallX = player.get_x() + distance * rayDirX;
        }
        wallX -= floor((wallX));   
     


        int x_texcoord = wall_x_texcoord(wallX, tex_walls);
        vector<uint32_t> column = tex_walls.get_scaled_column(texid, x_texcoord, lineHeight);
        short pix_x =i + fb.w; //
        for (short j = 0; j < lineHeight; ++j) { // copy the texture column to the framebuffer
            short pix_y = j + fb.h / 2 - lineHeight / 2;
            if (pix_y >= 0 && pix_y < static_cast<short>(fb.h)) {
                fb.set_pixel(pix_x, pix_y, column[j]);
            }
        }
        
        // пол и потолок
        float floorXWall, floorYWall; //x, y position of the floor texel at the bottom of the wall

        //4 возможные  направления стенок
        if (side == 0 && rayDirX >= 0){
            floorXWall = static_cast<float>(mapX);
            floorYWall = static_cast<float>(mapY + wallX);
        }
        else if (side == 0 && rayDirX <= 0){
            floorXWall = static_cast<float>(mapX + 1.0);
            floorYWall = static_cast<float>(mapY + wallX);
        }
        else if (side == 1 && rayDirY >= 0){
            floorXWall = static_cast<float>(mapX + wallX);
            floorYWall = static_cast<float>(mapY);
        }
        else{
            floorXWall = static_cast<float>(mapX + wallX);
            floorYWall = static_cast<float>(mapY + 1.0);
        }

        float distWall, distPlayer, currentDist;

        distWall = distance;
        distPlayer = 0.0;

        short drawEnd = lineHeight / 2 + fb.h / 2;
        if (drawEnd < 0) drawEnd = fb.h; // 
        for (short j = drawEnd ; j < fb.h; ++j){
            
            currentDist = fb.h / (2.0 * j - fb.h); 

            float weight = (currentDist - distPlayer) / (distWall - distPlayer);

            float currentFloorX = static_cast<float>(weight * floorXWall + (1.0 - weight) * player.get_x());
            float currentFloorY = static_cast<float>(weight * floorYWall + (1.0 - weight) * player.get_y());

            short floorTexX, floorTexY;
            floorTexX = static_cast<short>(currentFloorX * tex_walls.size) % tex_walls.size;
            floorTexY = static_cast<short>(currentFloorY * tex_walls.size) % tex_walls.size;
            

             
            int floorTexture;
            if (currentFloorY >=15 && currentFloorX >= 15) floorTexture = 3;
            else floorTexture = 2;

            
            
            //пол
            fb.set_pixel(fb.w + i, j, tex_walls.get(floorTexX, floorTexY, floorTexture));
           
            //потолок
            fb.set_pixel(fb.w + i, fb.h - j, tex_walls.get(floorTexX, floorTexY, 1));

        
        }
        
    }

 
    
    
    for (short i = 0; i < sprites.size(); ++i) { // отрисовываем спрайты
        draw_sprite(fb, sprites[i], depth_buffer, player, tex_monst);
        draw_sprite(fb, sprites2[i], depth_buffer, player, tex_doors);
    }

    
  
}