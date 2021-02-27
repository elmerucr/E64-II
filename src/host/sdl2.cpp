//  sdl2.cpp
//  E64
//
//  Copyright © 2017-2021 elmerucr. All rights reserved.

#include <cstdio>
#include <thread>
#include <chrono>
#include <SDL2/SDL.h>
#include "common.hpp"
#include "sdl2.hpp"
#include "tty.hpp"
#include "command.hpp"

SDL_AudioDeviceID E64_sdl2_audio_dev;
SDL_AudioSpec want, have;
bool audio_running;

const uint8_t *E64_sdl2_keyboard_state;

void E64::sdl2_init()
{
    SDL_Init(SDL_INIT_AUDIO);
    
    // each call to SDL_PollEvent invokes SDL_PumpEvents() that updates this array
    E64_sdl2_keyboard_state = SDL_GetKeyboardState(NULL);

    // INIT AUDIO STUFF
    // print the list of audio backends
    int numAudioDrivers = SDL_GetNumAudioDrivers();
    printf("[SDL] %d audio backend(s) compiled into SDL: ", numAudioDrivers);
    for(int i=0; i<numAudioDrivers; i++)
    {
        printf(" \'%s\' ", SDL_GetAudioDriver(i));
    }
    printf("\n");
    // What's this all about???
    SDL_zero(want);
    // audio specification
    want.freq = SAMPLE_RATE;
    // signed 16bit, little endian
    want.format = AUDIO_S16LSB;
    // 1, 2, 4, or 6  -  1 is mono, 2 is stereo...
    want.channels = 2;
    // power of 2, or 0 then env SDL_AUDIO_SAMPLES is used
    want.samples = 512;
    // can't be NULL
    want.callback = NULL;
    /* open audio device, allowing any changes to the specification */
    E64_sdl2_audio_dev = SDL_OpenAudioDevice(NULL, 0, &want, &have, SDL_AUDIO_ALLOW_ANY_CHANGE);
    if(!E64_sdl2_audio_dev)
    {
        printf("[SDL] failed to open audio device: %s\n", SDL_GetError());
        // this is not enough and even wrong...
        // consider a system without audio support and a frame delay based on FPS?
        SDL_Quit();
    }
    printf("[SDL] now using backend '%s'\n", SDL_GetCurrentAudioDriver());
    printf("[SDL] audio information\n");
    printf("\t\t\twant\thave\n");
    printf("frequency\t%d\t%d\n",want.freq,have.freq);
    printf("format\n float\t\t%s\t\t%s\n",SDL_AUDIO_ISFLOAT(want.format) ? "yes" : "no" ,SDL_AUDIO_ISFLOAT(have.format) ? "yes" : "no");
    printf(" signed\t\t%s\t\t%s\n", SDL_AUDIO_ISSIGNED(want.format) ? "yes" : "no", SDL_AUDIO_ISSIGNED(have.format) ? "yes" : "no");
    printf(" big endian\t%s\t\t%s\n", SDL_AUDIO_ISBIGENDIAN(want.format) ? "yes" : "no", SDL_AUDIO_ISBIGENDIAN(have.format) ? "yes" : "no");
    printf(" bitsize\t%d\t\t%d\n", SDL_AUDIO_BITSIZE(want.format), SDL_AUDIO_BITSIZE(have.format));
    printf("channels\t%d\t\t%d\n", want.channels, have.channels);
    printf("samples\t\t%d\t\t%d\n", want.samples, have.samples);
    audio_running = false;
}

int E64::sdl2_process_events()
{
    int return_value = NO_EVENT;
    
    SDL_Event event;

    bool shift_pressed = E64_sdl2_keyboard_state[SDL_SCANCODE_LSHIFT] | E64_sdl2_keyboard_state[SDL_SCANCODE_RSHIFT];
    bool alt_pressed   = E64_sdl2_keyboard_state[SDL_SCANCODE_LALT]   | E64_sdl2_keyboard_state[SDL_SCANCODE_RALT];
    //bool gui_pressed   = E64_sdl2_keyboard_state[SDL_SCANCODE_LGUI]   | E64_sdl2_keyboard_state[SDL_SCANCODE_RGUI];

    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_KEYDOWN:
                
                return_value = KEYPRESS_EVENT;          // default at keydown, may change to QUIT_EVENT
                
                if(event.key.keysym.sym == SDLK_F9)
                {
                    E64::sdl2_wait_until_f9_released();
			machine.toggle_mode();
                }
                else if( (event.key.keysym.sym == SDLK_f) && alt_pressed )
                {
                    E64::sdl2_wait_until_f_released();
                    host.video->toggle_fullscreen();
                }
                else if( (event.key.keysym.sym == SDLK_r) && alt_pressed )
                {
                    E64::sdl2_wait_until_r_released();
			machine.reset();
                    stats.reset();
                }
                else if( (event.key.keysym.sym == SDLK_q) && alt_pressed )
                {
                    E64::sdl2_wait_until_q_released();
                    return_value = QUIT_EVENT;
                }
                else if(machine.mode == RUNNING)
                {
                    if(event.key.keysym.sym == SDLK_F10)
                    {
			    machine.vicv->toggle_stats();
                    }
                }
                else if(machine.mode == MONITOR)
                {
                    switch(event.key.keysym.sym)
                    {
                        case SDLK_0:
                            monitor.tty->putchar(shift_pressed ? ')' : '0');
                            break;
                        case SDLK_1:
                            monitor.tty->putchar(shift_pressed ? '!' : '1');
                            break;
                        case SDLK_2:
                            monitor.tty->putchar(shift_pressed ? '@' : '2');
                            break;
                        case SDLK_3:
                            monitor.tty->putchar(shift_pressed ? '#' : '3');
                            break;
                        case SDLK_4:
                            monitor.tty->putchar(shift_pressed ? '$' : '4');
                            break;
                        case SDLK_5:
                            monitor.tty->putchar(shift_pressed ? '%' : '5');
                            break;
                        case SDLK_6:
                            monitor.tty->putchar(shift_pressed ? '^' : '6');
                            break;
                        case SDLK_7:
                            monitor.tty->putchar(shift_pressed ? '&' : '7');
                            break;
                        case SDLK_8:
                            monitor.tty->putchar(shift_pressed ? '*' : '8');
                            break;
                        case SDLK_9:
                            monitor.tty->putchar(shift_pressed ? '(' : '9');
                            break;
                        case SDLK_a:
                            monitor.tty->putchar(shift_pressed ? 'A' : 'a');
                            break;
                        case SDLK_b:
                            monitor.tty->putchar(shift_pressed ? 'B' : 'b');
                            break;
                        case SDLK_c:
                            monitor.tty->putchar(shift_pressed ? 'C' : 'c');
                            break;
                        case SDLK_d:
                            monitor.tty->putchar(shift_pressed ? 'D' : 'd');
                            break;
                        case SDLK_e:
                            monitor.tty->putchar(shift_pressed ? 'E' : 'e');
                            break;
                        case SDLK_f:
                            monitor.tty->putchar(shift_pressed ? 'F' : 'f');
                            break;
                        case SDLK_g:
                            monitor.tty->putchar(shift_pressed ? 'G' : 'g');
                            break;
                        case SDLK_h:
                            monitor.tty->putchar(shift_pressed ? 'H' : 'h');
                            break;
                        case SDLK_i:
                            monitor.tty->putchar(shift_pressed ? 'I' : 'i');
                            break;
                        case SDLK_j:
                            monitor.tty->putchar(shift_pressed ? 'J' : 'j');
                            break;
                        case SDLK_k:
                            monitor.tty->putchar(shift_pressed ? 'K' : 'k');
                            break;
                        case SDLK_l:
                            monitor.tty->putchar(shift_pressed ? 'L' : 'l');
                            break;
                        case SDLK_m:
                            monitor.tty->putchar(shift_pressed ? 'M' : 'm');
                            break;
                        case SDLK_n:
                            monitor.tty->putchar(shift_pressed ? 'N' : 'n');
                            break;
                        case SDLK_o:
                            monitor.tty->putchar(shift_pressed ? 'O' : 'o');
                            break;
                        case SDLK_p:
                            monitor.tty->putchar(shift_pressed ? 'P' : 'p');
                            break;
                        case SDLK_q:
                            monitor.tty->putchar(shift_pressed ? 'Q' : 'q');
                            break;
                        case SDLK_r:
                            monitor.tty->putchar(shift_pressed ? 'R' : 'r');
                            break;
                        case SDLK_s:
                            monitor.tty->putchar(shift_pressed ? 'S' : 's');
                            break;
                        case SDLK_t:
                            monitor.tty->putchar(shift_pressed ? 'T' : 't');
                            break;
                        case SDLK_u:
                            monitor.tty->putchar(shift_pressed ? 'U' : 'u');
                            break;
                        case SDLK_v:
                            monitor.tty->putchar(shift_pressed ? 'V' : 'v');
                            break;
                        case SDLK_w:
                            monitor.tty->putchar(shift_pressed ? 'W' : 'w');
                            break;
                        case SDLK_x:
                            monitor.tty->putchar(shift_pressed ? 'X' : 'x');
                            break;
                        case SDLK_y:
                            monitor.tty->putchar(shift_pressed ? 'Y' : 'y');
                            break;
                        case SDLK_z:
                            monitor.tty->putchar(shift_pressed ? 'Z' : 'z');
                            break;
                        case SDLK_SPACE:
                            monitor.tty->putchar(' ');
                            break;
                        case SDLK_BACKQUOTE:
                            monitor.tty->putchar(shift_pressed ? '~' : '`');
                            break;
                        case SDLK_COMMA:
                            monitor.tty->putchar(shift_pressed ? '<' : ',');
                            break;
                        case SDLK_PERIOD:
                            monitor.tty->putchar(shift_pressed ? '>' : '.');
                            break;
                        case SDLK_SLASH:
                            monitor.tty->putchar(shift_pressed ? '?' : '/');
                            break;
                        case SDLK_SEMICOLON:
                            monitor.tty->putchar(shift_pressed ? ':' : ';');
                            break;
                        case SDLK_QUOTE:
                            monitor.tty->putchar(shift_pressed ? '"' : '\'');
                            break;
                        case SDLK_BACKSLASH:
                            monitor.tty->putchar(shift_pressed ? '|' : '\\');
                            break;
                        case SDLK_LEFTBRACKET:
                            monitor.tty->putchar(shift_pressed ? '{' : '[');
                            break;
                        case SDLK_RIGHTBRACKET:
                            monitor.tty->putchar(shift_pressed ? '}' : ']');
                            break;
                        case SDLK_MINUS:
                            monitor.tty->putchar(shift_pressed ? '_' : '-');
                            break;
                        case SDLK_EQUALS:
                            monitor.tty->putchar(shift_pressed ? '+' : '=');
                            break;
                        case SDLK_RETURN:
				    monitor.tty->enter();
                            break;
                        case SDLK_BACKSPACE:
                            if(shift_pressed)
                            {
				    monitor.tty->insert();
                            }
                            else
                            {
				    monitor.tty->backspace();
                            }
                            break;
                        case SDLK_LEFT:
				    monitor.tty->arrow_left();
                            break;
                        case SDLK_RIGHT:
				    monitor.tty->arrow_right();
                            break;
                        case SDLK_UP:
				    monitor.tty->arrow_up();
                            break;
                        case SDLK_DOWN:
				    monitor.tty->arrow_down();
                            break;
                        case SDLK_F1:
				    monitor.command->single_step_cpu();
                            if (monitor.tty->status_bar_active == false) {
				    monitor.tty->cursor_deactivate();
				    monitor.command->dump_cpu_status();
				    monitor.tty->prompt();
				    monitor.tty->cursor_activate();
                            }
                            break;
                        case SDLK_F2:
				    monitor.tty->toggle_status_bar();
                            break;
                        case SDLK_F3:
				    monitor.tty->status_bar_hex_view = !monitor.tty->status_bar_hex_view;
                            break;
                    }
                }
                break;
            case SDL_WINDOWEVENT:
                if(event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    printf("[SDL] window resize event\n");
                }
                break;
            case SDL_DROPFILE:
			machine.fd->insert_disk(event.drop.file, false, false);
                SDL_free(event.drop.file);
		break;
            case SDL_QUIT:
                return_value = QUIT_EVENT;
                break;
        }
    }

    // update keystates in cia chip, regardless if machine is in debug or running mode
    if( !alt_pressed )
    {
	    machine.cia->keys_last_known_state[SCANCODE_ESCAPE] = E64_sdl2_keyboard_state[SDL_SCANCODE_ESCAPE] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_F1] = E64_sdl2_keyboard_state[SDL_SCANCODE_F1] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_F2] = E64_sdl2_keyboard_state[SDL_SCANCODE_F2] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_F3] = E64_sdl2_keyboard_state[SDL_SCANCODE_F3] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_F4] = E64_sdl2_keyboard_state[SDL_SCANCODE_F4] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_F5] = E64_sdl2_keyboard_state[SDL_SCANCODE_F5] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_F6] = E64_sdl2_keyboard_state[SDL_SCANCODE_F6] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_F7] = E64_sdl2_keyboard_state[SDL_SCANCODE_F7] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_F8] = E64_sdl2_keyboard_state[SDL_SCANCODE_F8] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_GRAVE] = (machine.cia->registers[SCANCODE_GRAVE] << 1) | (E64_sdl2_keyboard_state[SDL_SCANCODE_GRAVE] ? 0x01 : 0x00);
	    machine.cia->keys_last_known_state[SCANCODE_1] = E64_sdl2_keyboard_state[SDL_SCANCODE_1] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_2] = E64_sdl2_keyboard_state[SDL_SCANCODE_2] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_3] = E64_sdl2_keyboard_state[SDL_SCANCODE_3] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_4] = E64_sdl2_keyboard_state[SDL_SCANCODE_4] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_5] = E64_sdl2_keyboard_state[SDL_SCANCODE_5] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_6] = E64_sdl2_keyboard_state[SDL_SCANCODE_6] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_7] = E64_sdl2_keyboard_state[SDL_SCANCODE_7] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_8] = E64_sdl2_keyboard_state[SDL_SCANCODE_8] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_9] = E64_sdl2_keyboard_state[SDL_SCANCODE_9] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_0] = E64_sdl2_keyboard_state[SDL_SCANCODE_0] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_MINUS] = E64_sdl2_keyboard_state[SDL_SCANCODE_MINUS] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_EQUALS] = E64_sdl2_keyboard_state[SDL_SCANCODE_EQUALS] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_BACKSPACE] = E64_sdl2_keyboard_state[SDL_SCANCODE_BACKSPACE] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_TAB] = E64_sdl2_keyboard_state[SDL_SCANCODE_TAB] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_Q] = E64_sdl2_keyboard_state[SDL_SCANCODE_Q] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_W] = E64_sdl2_keyboard_state[SDL_SCANCODE_W] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_E] = E64_sdl2_keyboard_state[SDL_SCANCODE_E] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_R] = E64_sdl2_keyboard_state[SDL_SCANCODE_R] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_T] = E64_sdl2_keyboard_state[SDL_SCANCODE_T] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_Y] = E64_sdl2_keyboard_state[SDL_SCANCODE_Y] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_U] = E64_sdl2_keyboard_state[SDL_SCANCODE_U] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_I] = E64_sdl2_keyboard_state[SDL_SCANCODE_I] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_O] = E64_sdl2_keyboard_state[SDL_SCANCODE_O] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_P] = E64_sdl2_keyboard_state[SDL_SCANCODE_P] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_LEFTBRACKET] = E64_sdl2_keyboard_state[SDL_SCANCODE_LEFTBRACKET] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_RIGHTBRACKET] = E64_sdl2_keyboard_state[SDL_SCANCODE_RIGHTBRACKET] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_RETURN] = E64_sdl2_keyboard_state[SDL_SCANCODE_RETURN] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_A] = E64_sdl2_keyboard_state[SDL_SCANCODE_A] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_S] = E64_sdl2_keyboard_state[SDL_SCANCODE_S] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_D] = E64_sdl2_keyboard_state[SDL_SCANCODE_D] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_F] = E64_sdl2_keyboard_state[SDL_SCANCODE_F] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_G] = E64_sdl2_keyboard_state[SDL_SCANCODE_G] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_H] = E64_sdl2_keyboard_state[SDL_SCANCODE_H] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_J] = E64_sdl2_keyboard_state[SDL_SCANCODE_J] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_K] = E64_sdl2_keyboard_state[SDL_SCANCODE_K] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_L] = E64_sdl2_keyboard_state[SDL_SCANCODE_L] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_SEMICOLON] = E64_sdl2_keyboard_state[SDL_SCANCODE_SEMICOLON] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_APOSTROPHE] = E64_sdl2_keyboard_state[SDL_SCANCODE_APOSTROPHE] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_BACKSLASH] = E64_sdl2_keyboard_state[SDL_SCANCODE_BACKSLASH] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_LSHIFT] = E64_sdl2_keyboard_state[SDL_SCANCODE_LSHIFT] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_Z] = E64_sdl2_keyboard_state[SDL_SCANCODE_Z] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_X] = E64_sdl2_keyboard_state[SDL_SCANCODE_X] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_C] = E64_sdl2_keyboard_state[SDL_SCANCODE_C] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_V] = E64_sdl2_keyboard_state[SDL_SCANCODE_V] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_B] = E64_sdl2_keyboard_state[SDL_SCANCODE_B] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_N] = E64_sdl2_keyboard_state[SDL_SCANCODE_N] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_M] = E64_sdl2_keyboard_state[SDL_SCANCODE_M] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_COMMA] = E64_sdl2_keyboard_state[SDL_SCANCODE_COMMA] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_PERIOD] = E64_sdl2_keyboard_state[SDL_SCANCODE_PERIOD] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_SLASH] = E64_sdl2_keyboard_state[SDL_SCANCODE_SLASH] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_RSHIFT] = E64_sdl2_keyboard_state[SDL_SCANCODE_RSHIFT] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_LCTRL] = E64_sdl2_keyboard_state[SDL_SCANCODE_LCTRL] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_SPACE] = E64_sdl2_keyboard_state[SDL_SCANCODE_SPACE] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_RCTRL] = E64_sdl2_keyboard_state[SDL_SCANCODE_RCTRL] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_LEFT] = E64_sdl2_keyboard_state[SDL_SCANCODE_LEFT] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_UP] = E64_sdl2_keyboard_state[SDL_SCANCODE_UP] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_DOWN] = E64_sdl2_keyboard_state[SDL_SCANCODE_DOWN] ? 0x01 : 0x00;
	    machine.cia->keys_last_known_state[SCANCODE_RIGHT] = E64_sdl2_keyboard_state[SDL_SCANCODE_RIGHT] ? 0x01 : 0x00;
	}
	if (return_value == QUIT_EVENT)
		printf("[SDL] detected quit event\n");
	return return_value;
}

void E64::sdl2_wait_until_enter_released()
{
	SDL_Event event;
	bool wait = true;
	while (wait) {
		SDL_PollEvent(&event);
		if ((event.type == SDL_KEYUP) && (event.key.keysym.sym == SDLK_RETURN))
			wait = false;
		std::this_thread::sleep_for(std::chrono::microseconds(40000));
	}
}

void E64::sdl2_wait_until_f9_released()
{
    SDL_Event event;
    bool wait = true;
    while(wait) {
        SDL_PollEvent(&event);
        if( (event.type == SDL_KEYUP) && (event.key.keysym.sym == SDLK_F9) ) wait = false;
        std::this_thread::sleep_for(std::chrono::microseconds(40000));
    }
}

void E64::sdl2_wait_until_f_released()
{
    SDL_Event event;
    bool wait = true;
    while(wait) {
        SDL_PollEvent(&event);
        if( (event.type == SDL_KEYUP) && (event.key.keysym.sym == SDLK_f) ) wait = false;
        std::this_thread::sleep_for(std::chrono::microseconds(40000));
    }
}


void E64::sdl2_wait_until_q_released()
{
    SDL_Event event;
    bool wait = true;
    while(wait) {
        SDL_PollEvent(&event);
        if( (event.type == SDL_KEYUP) && (event.key.keysym.sym == SDLK_q) ) wait = false;
        std::this_thread::sleep_for(std::chrono::microseconds(40000));
    }
}


void E64::sdl2_wait_until_r_released()
{
    SDL_Event event;
    bool wait = true;
    while(wait) {
        SDL_PollEvent(&event);
        if( (event.type == SDL_KEYUP) && (event.key.keysym.sym == SDLK_r) ) wait = false;
        std::this_thread::sleep_for(std::chrono::microseconds(40000));
    }
}


void E64::sdl2_queue_audio(void *buffer, unsigned size)
{
    SDL_QueueAudio(E64_sdl2_audio_dev, buffer, size);
}

unsigned int E64::sdl2_get_queued_audio_size()
{
    return SDL_GetQueuedAudioSize(E64_sdl2_audio_dev);
}

void E64::sdl2_start_audio()
{
    if(!audio_running)
    {
        printf("[SDL] start audio\n");
        // Unpause audiodevice, and process audiostream
        SDL_PauseAudioDevice(E64_sdl2_audio_dev, 0);
        audio_running = true;
    }
}

void E64::sdl2_stop_audio()
{
    if(audio_running)
    {
        printf("[SDL] stop audio\n");
        // Pause audiodevice
        SDL_PauseAudioDevice(E64_sdl2_audio_dev, 1);
        audio_running = false;
    }
}

void E64::sdl2_cleanup()
{
    printf("[SDL] cleaning up\n");
    E64::sdl2_stop_audio();
    SDL_CloseAudioDevice(E64_sdl2_audio_dev);
    //SDL_Quit();
}
