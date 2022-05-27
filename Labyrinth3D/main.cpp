#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>
#include <cmath>
#include "SDL.h"
#include "utils.h"
#include "render.h"
#include "player.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"
 
 

 

SDL_Window* window = nullptr;
SDL_Renderer* sdl_renderer = nullptr;




struct Text_T{
    TTF_Font* font = TTF_OpenFont("Text.ttf", 50);
}Text_T;

struct Cont{
    int k = 0;
}Cont;


SDL_Texture* get_text_texture(SDL_Renderer*& renderer, char* text, TTF_Font* font) {
    SDL_Surface* textSurface = NULL;
    SDL_Color back_color1 = { 255, 0, 255,1 }; //вот эту строчку я украл, оказывается прозрачный фон сделать нужно так
    SDL_Color back_color = { 255,255,255 };
    textSurface = TTF_RenderText_Shaded(font, text, back_color, back_color1);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);
    return texture;
}
 
void draw_text(SDL_Renderer*& renderer, SDL_Texture* texture, int a)
{
    SDL_Rect rect;
    if (a == 0)
        rect = { 10,10, 40, 40 };
    else
        rect = { 720, 0, 20, 20 };
    SDL_RenderCopy(renderer, texture, NULL, &rect);
}


 

using namespace std;
int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO)) {
        cerr << "Failed to initialize SDL: " << SDL_GetError() << endl;
        return -1;
    }


     



    
    TTF_Init();
    Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 1024);
    Mix_Music* Idle = Mix_LoadMUS("minecraft.mp3"); //Фоновая музыка
    Mix_VolumeMusic(3);
    Mix_PlayMusic(Idle, 1);


    constexpr unsigned fps = 60;

    

    FrameBuffer fb{};
    Player player{ 1.2, 1.2, 0.3 , static_cast<float>((M_PI / 4.)), 0, 0 };// игрок
    GameState gs{ Map(),                                        // игровая карта
                {    {6, 1.5 ,0, 1},//возле входа в самом начале
                     {9, 1.5, 0, 1},
                     {10, 3.5, 0, 1},
                     {15, 1.5, 0, 1}},

                {    
                     {15.4, 15.4, 3, 1}},

                   Texture("./walls.bmp", SDL_PIXELFORMAT_ABGR8888),  //текстуры стен
                   Texture("./monsters.bmp", SDL_PIXELFORMAT_ABGR8888),  // текстуры предметов
                   Texture("./doors.bmp", SDL_PIXELFORMAT_ABGR8888),  // текстуры надписи у двери
    };
    if (!gs.tex_walls.count || !gs.tex_monst.count || !gs.tex_doors.count) {
        cerr << "Failed to load textures" << endl;
        return -1;
    }




    window = SDL_CreateWindow("Labyrinth 3D", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, fb.w, fb.h, SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_FOCUS);
    if (window == NULL) {
        cerr << "Failed to create sdl_window: " << SDL_GetError() << endl;
        return -1;
    }
    sdl_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (sdl_renderer == NULL) {
        cerr << "Failed to create sdl_renderer: " << SDL_GetError() << endl;
        return -1;
    }

    SDL_Texture* framebuffer_texture = SDL_CreateTexture(sdl_renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, fb.w, fb.h);
    if (!framebuffer_texture) {
        cerr << "Failed to create framebuffer texture : " << SDL_GetError() << endl;
        return -1;
    }

    Uint32 last_time = SDL_GetTicks();

    

    Text_T.font = TTF_OpenFont("Text.ttf", 40);
    

    
    char text[10];
   _itoa_s(Cont.k, text, 10);
    
    
    SDL_Surface* textSurf = TTF_RenderText_Solid(Text_T.font, " ", {255,255,255});
    

    SDL_Texture* textTexture = get_text_texture(sdl_renderer, text, Text_T.font);
    SDL_Rect textRect = {30,30,textSurf->w, textSurf->h};
    SDL_FreeSurface(textSurf);
    TTF_CloseFont(Text_T.font);
    
     


    // ИГРОВОЙ ЦИКЛ
    while (!gs.quit) {
      //  SDL_RenderClear(sdl_renderer);
          
      

        float dt = (SDL_GetTicks() - last_time) / 1000.0F;// в секундах
        last_time = SDL_GetTicks();
        if (dt == 0) dt = 1;

        player.handle_events(gs, dt);
        
        
        
        render(fb, gs, player); // визуализация сцены 
         
        

        // скопируйте содержимое фреймбуфера на экран
        SDL_UpdateTexture(framebuffer_texture, NULL, reinterpret_cast<void*>(fb.img.data()), fb.w * 4);
        SDL_RenderClear(sdl_renderer);
        SDL_RenderCopy(sdl_renderer, framebuffer_texture, NULL, NULL);


       
        // SDL_RenderCopy(sdl_renderer, textTexture, NULL, &textRect);
        SDL_RenderPresent(sdl_renderer);
        

         



        // частота кадров
        if ((1000.0 / fps) > (SDL_GetTicks() - last_time)) {
            SDL_Delay((1000.0 / fps) - (SDL_GetTicks() - last_time));
        }

        


    }
    TTF_Quit();
    SDL_DestroyTexture(framebuffer_texture);
    SDL_DestroyRenderer(sdl_renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    Mix_Quit();
    

    return 0;
}