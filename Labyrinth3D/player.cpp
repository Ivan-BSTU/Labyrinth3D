#include <cstdlib>
#include <cstddef>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cassert>
#include "SDL.h"
#include "player.h"
#include "map.h"
#include <iostream>
#include <map>
#include "SDL_mixer.h"
#include <SDL_events.h>
#include <SDL_ttf.h>
 

#include <Windows.h>
SDL_Renderer* sdl_renderer2 = nullptr;
using namespace std;
const Uint8* state = SDL_GetKeyboardState(NULL);
 
Player::Player(float pos_x = 2., float pos_y = 2., float ang = 1., float fov = 1., float t = .0, float w = .0) :
    x(pos_x), y(pos_y), a(ang), fov(fov), turn(t), walk(w) {}


struct Tumb1{
    bool tumb = false; //включена или выключена музыка
    int vol = 3;//громкость 
    int papers = 0; //количество пергаментов
    bool FULL = false;


}Tumb1;


 
Mix_Chunk* Open;
Mix_Chunk* Step_run ;
Mix_Chunk* Take;
Mix_Chunk* Locked;
Mix_Chunk* Full_SCR;

void Full_Scrolls() {
    Mix_FreeChunk(Full_SCR);
    Full_SCR = Mix_LoadWAV("full_scrolls.wav");
    Mix_PlayChannel(-1, Full_SCR, 0);
}


void Open_Door() {
    Mix_FreeChunk(Open);
    Open = Mix_LoadWAV("door_unlock.wav");
    Mix_PlayChannel(2, Open, 0);

}


void Take_Sound() {
   
    Take = Mix_LoadWAV("take.wav");
    Mix_PlayChannel(-1,Take, 0);

}
 

 
//звук ходьбы
void Sound2() { 
 
    Mix_FreeChunk(Step_run);
   
     Step_run = Mix_LoadWAV("steps_run.wav");
     Mix_VolumeChunk(Step_run, MIX_MAX_VOLUME / 5); //громкость шагов
     Mix_PlayChannel(-1, Step_run, 0);
}





void Sound_Locked() {

    Mix_FreeChunk(Locked);
    
    Locked = Mix_LoadWAV("lock_door.wav");\
        Mix_VolumeChunk(Locked, MIX_MAX_VOLUME / 4);
    Mix_VolumeChunk(Locked, MIX_MAX_VOLUME / 2 ); //
    Mix_PlayChannel(  1, Locked, 1);

}


float nx2 = 0;
float ny2 = 0;
float nx = 0;
float ny = 0;
void Player::handle_events(GameState& gs, float dt) {
    SDL_Event event;
    
   

   


    int step_long = 0;

    
     
    

    Mix_VolumeMusic(Tumb1.vol);
    
    
    while (SDL_PollEvent(&event)) {
        
        if (SDL_QUIT == event.type || (SDL_KEYDOWN == event.type && SDLK_ESCAPE == event.key.keysym.sym)) { gs.quit = true;}
        if (SDL_KEYUP == event.type) {
            
             
            
            
            if ('a' == event.key.keysym.sym || 'd' == event.key.keysym.sym) {
                turn = 0;
                 
                
            }
            if ('w' == event.key.keysym.sym || 's' == event.key.keysym.sym || SDLK_LSHIFT == event.key.keysym.sym) { walk = 0;   }
        }

        if (SDL_KEYDOWN == event.type) {
            
           
            if ('a' == event.key.keysym.sym) {
                turn = -1.5;  
            }//влево
            if ('d' == event.key.keysym.sym) {
                turn = 1.5;  
            }//вправо
            if ('w' == event.key.keysym.sym) {
                walk = 1; 
                
            
                 
            
            }//вперед;
         
            if ('s' == event.key.keysym.sym) {
                walk = -1;
            }//назад
              

            if (state[SDL_SCANCODE_W] && state[SDL_SCANCODE_LSHIFT]) {
                walk = 2; //бег 
                
                
            }

            if (state[SDL_SCANCODE_W] && state[SDL_SCANCODE_LCTRL]) {
                
               walk = 0.4; //красться
         
            }


            if ('m' == event.key.keysym.sym) {
                if (Tumb1.tumb == false) { Tumb1.tumb = true; Mix_PauseMusic(); }
                else { Tumb1.tumb = false; Mix_ResumeMusic(); }
            
            }//фоновая музыка


            //громкость
            if ('-' == event.key.keysym.sym) {
              
                if (Tumb1.vol>0) Tumb1.vol -= 2;
                cout << "volume: " << Tumb1.vol << endl;
                
            }
            if ('=' == event.key.keysym.sym) {
               
                if (Tumb1.vol < 128) Tumb1.vol += 2;
                cout << "volume: " << Tumb1.vol << endl;
            }
   
        }

        

        
       //Mix_HaltChannel(1);//вот эта штука - вроде как очистка канала, но работает она только при очистке канала 1, когда основной звук в канале -1, почему - я не знаю.
       //upd - верхняя штука уже не нужна, все исправлено, но пусть будет на память


       //управление мышью 
        SDL_SetRelativeMouseMode(SDL_TRUE);
        if (SDL_MOUSEMOTION == event.type) {
            Sint32 mouse_rel_x = event.motion.xrel;
            float xpos = 0;
            xpos += mouse_rel_x;
 
            a += xpos * dt * 0.04F;
        }
    }
  
    //чтобы повернуться при помощи кнопок мы угол умножаем на +0.04 или -0.04
          a += float(turn) * 0.02; 
          nx = x + walk * cos(a) * dt * 2;
          ny = y + walk * sin(a) * dt * 2;


          if (walk == 0){
              nx2 = nx;
              ny2 = ny;

          }

          //ГОСПОДИ, Я ПОЧИНИЛ ЗВУК. КОРОЧЕ ТЕПЕРЬ ЗВУК ШАГА БУДЕТ ТОЛЬКО ПОСЛЕ ОПРЕДЕЛЕННОГО РАССТОЯНИЯ ОТ ИСХОДНОЙ ТОЧКИ
          
          if (sqrt((nx - nx2) * (nx - nx2) + (ny - ny2) * (ny - ny2)) >= 0.9) {
              
          Sound2();
          nx2 = nx; ny2 = ny;

          }
 
          
           
          

          ////////////////дверь/////////////////////////////////////////////////////
          if (nx > 15 && ny > 15 && state[SDL_SCANCODE_E] ) {
              if (Tumb1.papers >= 4) {

                  Open_Door();
                  SDL_Delay(750);
                      gs.quit = true;
               
              }
              else{

                 ///Где же вы были раньше боооооожее//////////////////////////
                  if (Mix_Playing(1) != 1) {
                      Sound_Locked();
                  }
                    

              }
               

              } //дверь 
              
            

          
          
    
   
   //  cout << nx << " " << ny << endl;
   
          

    if (static_cast<size_t>(nx) >= 0 && static_cast<size_t>(nx) < static_cast<int>(gs.map.w) && static_cast<size_t>(ny) >= 0 && static_cast<size_t>(ny) < static_cast<size_t>(gs.map.h)) {
        if (gs.map.is_empty(nx, static_cast<size_t>(y))) x = nx;
        if (gs.map.is_empty(static_cast<size_t>(x), ny)) y = ny;
        
        
    }
 
     
    
    for (size_t i = 0; i < gs.monsters.size(); ++i) {
        // обновляем расстояния от игрока до каждого спрайта
        gs.monsters[i].player_dist = sqrt(pow(x - gs.monsters[i].x, 2) + pow(y - gs.monsters[i].y, 2));
        gs.doors[i].player_dist = sqrt(pow(x - gs.doors[i].x, 2) + pow(y - gs.doors[i].y, 2));
        
        

        if (Tumb1.papers >= 4) { gs.doors[i].tex_id = 2; }


        ///////////////////////////////вот здесь подбираем записки/////////////////////////////////////////

        
         if (gs.monsters[i].player_dist < 1.5){gs.monsters[i].tex_id = 1;}
         else {gs.monsters[i].tex_id = 0;}

         
        if (gs.monsters[i].player_dist < 1.5 && state[SDL_SCANCODE_E] && gs.monsters[i].tex_id != 2) { gs.monsters[i].tex_id = 1;
        Tumb1.papers++;
        
        gs.monsters[i].y = 10000;
         
        Mix_FreeChunk(Take);

        if (Tumb1.papers == 4 && Tumb1.FULL == false) {
            Full_Scrolls();
            Tumb1.FULL = true;
        }
        else {
            Take_Sound();
        }
        
        cout << Tumb1.papers << endl;
        
        }
    }   

 

    
   




      sort(gs.monsters.begin(), gs.monsters.end()); //сортируем от самого дальнего спрайта к самому ближнему
      sort(gs.doors.begin(), gs.doors.end());
 
}
 
float Player::get_x() {
    return x;
}
float Player::get_y() {
    return y;
}
float Player::get_angle() {
    return a;
}
float Player::get_fov() {
    return fov;
}
