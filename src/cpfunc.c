#define SDL_MAIN_HANDLED
#include <stdio.h>
#include <stdlib.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "cpfunc.h"

void CP_InitFunc(RenderContext* ctx) {
    return;
}

void CP_SetLinearFunction(RenderContext* ctx, float* x, size_t count, float m, float b) {
    // No more sizeof(float) math here!
    for (size_t i = 0; i < count; i++) {
        float y = m * x[i] + b; 
        printf("Point %zu -> X: %.2f, Y: %.2f\n", i, x[i], y);
    }
}