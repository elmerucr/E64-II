//  video.cpp
//  E64
//
//  Copyright © 2020 elmerucr. All rights reserved.

#include "video.hpp"
#include "common_defs.hpp"
#include <SDL2/SDL.h>

E64::video::video()
{
    SDL_version compiled;
    SDL_version linked;

    SDL_VERSION(&compiled);
    SDL_GetVersion(&linked);
    printf("[SDL] compiled against SDL version %d.%d.%d\n",
           compiled.major, compiled.minor, compiled.patch);
    printf("[SDL] linked against SDL version %d.%d.%d\n",
           linked.major, linked.minor, linked.patch);
    
    SDL_Init(SDL_INIT_VIDEO);
    
    // print the list of video backends
    int numVideoDrivers = SDL_GetNumVideoDrivers();
    printf("[SDL] %d video backend(s) compiled into SDL: ", numVideoDrivers);
    for(int i=0; i<numVideoDrivers; i++)
    {
        printf(" \'%s\' ", SDL_GetVideoDriver(i));
    }
    printf("\n");
    printf("[SDL] now using backend '%s'\n", SDL_GetCurrentVideoDriver());
    
    current_window_size = 2;
    fullscreen = false;
    
    // create window - title will be set later by function E64::sdl2_update_title()
    window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_sizes[current_window_size].x, window_sizes[current_window_size].y, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE );
    // create renderer and link it to window
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    // create a texture that is able to refresh very frequently
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, VICV_PIXELS_PER_SCANLINE, VICV_SCANLINES);
    
    SDL_GetWindowSize(window, &window_width, &window_height);
    destination = { 0, 0, window_width, window_height };

    // make sure mouse cursor isn't visible
    SDL_ShowCursor(SDL_DISABLE);
}

E64::video::~video()
{
    // 
}
