/*
 * This file is part of the Trezor project, https://trezor.io/
 *
 * Copyright (c) SatoshiLabs
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <SDL.h>
#include <SDL_image.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL_render.h"
#include "SDL_surface.h"
#include "common.h"
#include "display_defs.h"
#include "display_interface.h"
#include "profile.h"

#define EMULATOR_BORDER 62

#define WINDOW_WIDTH    604
#define WINDOW_HEIGHT   979
#define TOUCH_OFFSET_X  0
#define TOUCH_OFFSET_Y  0
#if __APPLE__
  #define WINDOW_SCALE 1
#endif
static SDL_Window* WINDOW;
static SDL_Renderer* RENDERER;
static SDL_Surface* BUFFER;
static SDL_Texture *TEXTURE, *BACKGROUND;

static SDL_Surface* PREV_SAVED;

static int DISPLAY_BACKLIGHT = -1;
static int DISPLAY_ORIENTATION = -1;
int sdl_display_res_x = DISPLAY_RESX, sdl_display_res_y = DISPLAY_RESY;
int sdl_touch_offset_x, sdl_touch_offset_y;

// this is just for compatibility with DMA2D using algorithms
uint8_t* const DISPLAY_DATA_ADDRESS = 0;

static struct
{
    struct
    {
        uint16_t x, y;
    } start;
    struct
    {
        uint16_t x, y;
    } end;
    struct
    {
        uint16_t x, y;
    } pos;
} PIXELWINDOW;

void display_pixeldata(uint16_t c)
{
    if ( !RENDERER )
    {
        display_init();
    }
    if ( PIXELWINDOW.pos.x < PIXELWINDOW.end.x && PIXELWINDOW.pos.y < PIXELWINDOW.end.y )
    {
        int index = PIXELWINDOW.pos.x + PIXELWINDOW.pos.y * BUFFER->pitch / sizeof(uint16_t);
        ((uint16_t*)BUFFER->pixels)[index] = c;
    }
    PIXELWINDOW.pos.x++;
    if ( PIXELWINDOW.pos.x >= PIXELWINDOW.end.x )
    {
        PIXELWINDOW.pos.x = PIXELWINDOW.start.x;
        PIXELWINDOW.pos.y++;
    }
}

void display_reset_state()
{
}

void display_init_seq(void)
{
}

void display_deinit(void)
{
    SDL_FreeSurface(PREV_SAVED);
    SDL_FreeSurface(BUFFER);
    if ( BACKGROUND != NULL )
    {
        SDL_DestroyTexture(BACKGROUND);
    }
    if ( TEXTURE != NULL )
    {
        SDL_DestroyTexture(TEXTURE);
    }
    if ( RENDERER != NULL )
    {
        SDL_DestroyRenderer(RENDERER);
    }
    if ( WINDOW != NULL )
    {
        SDL_DestroyWindow(WINDOW);
    }
    SDL_Quit();
}

void display_init(void)
{
    if ( SDL_Init(SDL_INIT_VIDEO) != 0 )
    {
        printf("%s\n", SDL_GetError());
        ensure(secfalse, "SDL_Init error");
    }
    atexit(display_deinit);

    char* window_title = "hypermate^emu";

    int w, h;
#if __APPLE__
    w = WINDOW_WIDTH * WINDOW_SCALE;
    h = WINDOW_HEIGHT * WINDOW_SCALE;
#else
    w = WINDOW_WIDTH;
    h = WINDOW_HEIGHT;
#endif
    WINDOW = SDL_CreateWindow(
        window_title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN
    );

    if ( !WINDOW )
    {
        printf("%s\n", SDL_GetError());
        ensure(secfalse, "SDL_CreateWindow error");
    }
    RENDERER = SDL_CreateRenderer(WINDOW, -1, SDL_RENDERER_SOFTWARE);
    if ( !RENDERER )
    {
        printf("%s\n", SDL_GetError());
        SDL_DestroyWindow(WINDOW);
        ensure(secfalse, "SDL_CreateRenderer error");
    }

    // 设置逻辑分辨率大小
    SDL_RenderSetLogicalSize(RENDERER, WINDOW_WIDTH, WINDOW_HEIGHT);
    SDL_SetRenderDrawColor(RENDERER, 0, 0, 0, 255);
    SDL_RenderClear(RENDERER);
    BUFFER = SDL_CreateRGBSurface(0, MAX_DISPLAY_RESX, MAX_DISPLAY_RESY, 16, 0xF800, 0x07E0, 0x001F, 0x0000);
    TEXTURE = SDL_CreateTexture(
        RENDERER, SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_STREAMING, DISPLAY_RESX, DISPLAY_RESY
    );
    SDL_SetTextureBlendMode(TEXTURE, SDL_BLENDMODE_BLEND);
#ifdef __APPLE__
    // macOS Mojave SDL black screen workaround
    SDL_PumpEvents();
    SDL_SetWindowSize(WINDOW, w, h);
#endif
#include "background.h"
    BACKGROUND = IMG_LoadTexture_RW(RENDERER, SDL_RWFromMem(background_png, background_png_len), 0);
    if ( BACKGROUND )
    {
        SDL_SetTextureBlendMode(BACKGROUND, SDL_BLENDMODE_NONE);
    }
    else
    {
        SDL_SetWindowSize(WINDOW, DISPLAY_RESX + 2 * EMULATOR_BORDER, DISPLAY_RESY + 3 * EMULATOR_BORDER);
    }
    sdl_touch_offset_x = EMULATOR_BORDER;
    sdl_touch_offset_y = EMULATOR_BORDER;
    DISPLAY_BACKLIGHT = 60;
    DISPLAY_ORIENTATION = 0;
}

void display_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    if ( !RENDERER )
    {
        display_init();
    }
    PIXELWINDOW.start.x = x0;
    PIXELWINDOW.start.y = y0;
    PIXELWINDOW.end.x = x1;
    PIXELWINDOW.end.y = y1;
    PIXELWINDOW.pos.x = x0;
    PIXELWINDOW.pos.y = y0;
}

void log_pixels(uint16_t* data, size_t data_size)
{
#define __FRAME_LINE__ 16
    uint8_t count = 0;
    while ( data_size-- )
    {
        count++;
        printf("%04x ", *data++);
        if ( count == __FRAME_LINE__ )
        {
            printf("\n");
            count = 0;
        }
    }
    printf("\n");
}
void display_refresh(void)
{
    if ( !RENDERER )
    {
        display_init();
    }
    if ( BACKGROUND )
    {
        SDL_RenderCopy(RENDERER, BACKGROUND, NULL, NULL);
    }
    else
    {
        SDL_RenderClear(RENDERER);
    }
    SDL_UpdateTexture(TEXTURE, NULL, BUFFER->pixels, BUFFER->pitch);
    SDL_SetTextureAlphaMod(TEXTURE, MIN(255, 255 * DISPLAY_BACKLIGHT / 100));
    const SDL_Rect r = {EMULATOR_BORDER, EMULATOR_BORDER, DISPLAY_RESX, DISPLAY_RESY};
    // SDL_RenderCopyEx(RENDERER, TEXTURE, NULL, &r, DISPLAY_ORIENTATION, NULL, 0);
    SDL_RenderCopy(RENDERER, TEXTURE, NULL, &r);
    SDL_RenderPresent(RENDERER);
}

int display_orientation(int degrees)
{
    if ( degrees != DISPLAY_ORIENTATION )
    {
        if ( degrees == 0 || degrees == 90 || degrees == 180 || degrees == 270 )
        {
            DISPLAY_ORIENTATION = degrees;
            display_refresh();
        }
    }
    return DISPLAY_ORIENTATION;
}

int display_get_orientation(void)
{
    return DISPLAY_ORIENTATION;
}

int display_backlight(int val)
{
    if ( DISPLAY_BACKLIGHT != val && val >= 0 && val <= 100 )
    {
        DISPLAY_BACKLIGHT = val;
        display_refresh();
    }
    return DISPLAY_BACKLIGHT;
}

const char* display_save(const char* prefix)
{
    if ( !RENDERER )
    {
        display_init();
    }
    static int count;
    static char filename[256];
    // take a cropped view of the screen contents
    const SDL_Rect rect = {0, 0, DISPLAY_RESX, DISPLAY_RESY};
    SDL_Surface* crop = SDL_CreateRGBSurface(
        BUFFER->flags, rect.w, rect.h, BUFFER->format->BitsPerPixel, BUFFER->format->Rmask,
        BUFFER->format->Gmask, BUFFER->format->Bmask, BUFFER->format->Amask
    );
    SDL_BlitSurface(BUFFER, &rect, crop, NULL);
    // compare with previous screen, skip if equal
    if ( PREV_SAVED != NULL )
    {
        if ( memcmp(PREV_SAVED->pixels, crop->pixels, crop->pitch * crop->h) == 0 )
        {
            SDL_FreeSurface(crop);
            return filename;
        }
        SDL_FreeSurface(PREV_SAVED);
    }
    // save to png
    snprintf(filename, sizeof(filename), "%s%08d.png", prefix, count++);
    IMG_SavePNG(crop, filename);
    PREV_SAVED = crop;
    return filename;
}

void display_clear_save(void)
{
    SDL_FreeSurface(PREV_SAVED);
    PREV_SAVED = NULL;
}

void decode_to_lcd(const uint8_t* buf, size_t size)
{
    SDL_RWops* rw = SDL_RWFromConstMem(buf, size);
    if ( !rw )
        return;
    SDL_Surface* surface = IMG_Load_RW(rw, 1);
    if ( !surface )
        return;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(RENDERER, surface);
    SDL_FreeSurface(surface);
    if ( !texture )
        return;
    SDL_SetTextureAlphaMod(texture, MIN(255, 255 * DISPLAY_BACKLIGHT / 100));
    const SDL_Rect r = {EMULATOR_BORDER, EMULATOR_BORDER, DISPLAY_RESX, DISPLAY_RESY};
    SDL_RenderCopy(RENDERER, texture, NULL, &r);
    SDL_RenderPresent(RENDERER);
    SDL_DestroyTexture(texture);
}
