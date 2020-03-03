//  video.hpp
//  E64
//
//  Copyright © 2020 elmerucr. All rights reserved.

#include <SDL2/SDL.h>

#ifndef VIDEO_HPP
#define VIDEO_HPP

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
    
    /* These are host framebuffers for double buffering. We need two
     * buffers as the calls for refreshing the screen might happen when
     * the next screen is already being drawn.
     * To the outside world only host_front_buffer and host_back_buffer
     * are known.
     */
    uint32_t *buffer_0;
    uint32_t *buffer_1;
public:
    video();
    ~video();
    
    // pointer to the buffer that currently can be shown
    uint32_t *frontbuffer;
    // pointer to the buffer that's currently being written to
    uint32_t *backbuffer;
    
    // pointer to debug screen buffer
    uint32_t *debug_screen_buffer;
    
    inline void swap_buffers()
    {
        uint32_t *temp = frontbuffer;
        frontbuffer = backbuffer;
        backbuffer = temp;
    }
    
    // must be called upon machine reset
    void reset();
    
    void update_screen();
    void update_title();
    void reset_window_size();
    void increase_window_size();
    void decrease_window_size();
    void toggle_fullscreen();
};

}

#endif // VIDEO_HPP
