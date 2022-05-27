#ifndef ADD_H
#define ADD_H

#include "render.h"
 

 
class Player {
public:
    Player(float pos_x, float pos_y, float ang, float fov, float turn, float walk);
    void handle_events(GameState& gs, float dt);
    float get_x();
    float get_y();
    float get_angle();
    float get_fov();
     
private:
    float x, y; // позиция
    float a;    // направление взгляда
    float fov;  // поле зрения
    float turn, walk;

};

 


#endif