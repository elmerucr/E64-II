//  video.hpp
//  E64
//
//  Copyright © 2020 elmerucr. All rights reserved.

#include <SDL2/SDL.h>

namespace E64 {

struct window_size
{
    uint16_t x;
    uint16_t y;
};

class video
{
private:
    const struct window_size window_sizes[4] =
    {
        {  512, 320 },
        {  768, 480 },
        { 1024, 640 },
        { 1280, 800 }
    };
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    uint8_t current_window_size;
    bool fullscreen;
    int window_width;
    int window_height;
    SDL_Rect destination;
public:
    video();
    ~video();
    
    void update_screen();
    void update_title();
    void reset_window_size();
    void increase_window_size();
    void decrease_window_size();
    void toggle_fullscreen();
};

}
