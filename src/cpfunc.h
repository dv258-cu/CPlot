#ifndef CPFUNC_H
#define CPFUNC_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#define MARGIN 50

typedef enum {
    CP_2D,
    CP_3D,
    CP_HEATMAP_2D
} GraphType;

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Surface* surface;
    SDL_AtomicInt running;
    SDL_AtomicInt w;
    SDL_AtomicInt h;
    char* title;
    TTF_Font* font;
    GraphType gt;
} RenderContext;

// Pass number of elements instead of byte size
void CP_InitFunc(RenderContext* ctx);
void CP_SetLinearFunction(RenderContext* ctx, float* x, size_t count, float m, float b);

#endif