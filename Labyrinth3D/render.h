#pragma once

#include <vector>
#include <cstdint>

#include "map.h"
#include "sprite.h"
#include "framebuffer.h"
#include "textures.h"
using namespace std;
class Player;

struct GameState {
    Map map;
    vector<Sprite> monsters;
    vector<Sprite> doors;
    Texture tex_walls;
    Texture tex_monst;
    Texture tex_doors;
    bool quit = false;
};

void render(FrameBuffer& fb, const GameState& gs, Player& player);

