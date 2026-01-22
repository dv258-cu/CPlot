#define SDL_MAIN_HANDLED
#include <stdio.h>
#include <stdlib.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "cplot.h"
#include "cpfunc.h"


// Takes the window dimensions to transform horizontal axis values to actual graph values
int Graph2Window_X(int x) {
    
    return x + MARGIN;
}

// Takes the window dimensions to transform vertical axis values to actual graph values
int Graph2Window_Y(RenderContext* ctx, int y) {
    return -y + SDL_GetAtomicInt(&ctx->h) - MARGIN;
}

float CP_GetHorizontalResolution(RenderContext* ctx, int x_length) {
    int plot_width = SDL_GetAtomicInt(&ctx->w) - 2 * MARGIN;

    return (float)plot_width / x_length;
}
// Renders the horizontal axis for GraphType CP_2D
void CP2D_RenderHorizontalAxis(RenderContext* ctx, int x_length) {
    SDL_RenderLine(ctx->renderer, MARGIN, SDL_GetAtomicInt(&ctx->h) - MARGIN, SDL_GetAtomicInt(&ctx->w) - MARGIN, SDL_GetAtomicInt(&ctx->h) - MARGIN);

    float x_spacing = CP_GetHorizontalResolution(ctx, x_length);    // Pixel spacing between points
    for (int i = 0; i < x_length; i++) {
        SDL_RenderLine(ctx->renderer, Graph2Window_X(i * x_spacing), Graph2Window_Y(ctx, -5), Graph2Window_X(i * x_spacing), Graph2Window_Y(ctx, 5));
    }
}

// All plotting is done here
int RenderingThread(void* data) {
    RenderContext* ctx = (RenderContext*)data;  // Define the local RenderContext as a pointer for global access
    Uint64 now = SDL_GetPerformanceCounter();   // Initialize timer for framerate control (in case of any animations)
    Uint64 last = 0;

    SDL_SetRenderVSync(ctx->renderer, 1);       // VSync in seperate thread to prevent 100% CPU usage

    // Runtime loop
    while (SDL_GetAtomicInt(&ctx->running)) {
        last = now;
        now = SDL_GetPerformanceCounter();
        double deltaTime = (double)((now - last) / (double)SDL_GetPerformanceFrequency());

        // Set the frame clear color for animations
        SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 0, 255);
        SDL_RenderClear(ctx->renderer);

        /// Start Plot Code ///

        // Switch the GraphType to allow different code to run under certain types
        switch(ctx->gt) {
            case CP_2D: {
                // Draw the axes of a plot
                SDL_SetRenderDrawColor(ctx->renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                CP2D_RenderHorizontalAxis(ctx, 1000);
                SDL_RenderLine(ctx->renderer, MARGIN, SDL_GetAtomicInt(&ctx->h) - MARGIN, MARGIN, MARGIN);
                break;
            }
            case CP_3D: { 
                // Will include projection matrices and more complex math for graphing in 3D
                break;
            }
            case CP_HEATMAP_2D: {
                // Will include background texture and allow heatmap style data visualization for specific background images
                break;
            }
            
            default:
                break;
            
        }


        /// End Plot Code ///

        SDL_RenderPresent(ctx->renderer);
    }
    return 0;
}

// Initializes the SDL_Window with esc->quit and update loop. Takes window name, width, and height
int CP_InitSDL(char* plot_name, int plot_width, int plot_height, GraphType gt) {
    if (!SDL_Init(SDL_INIT_VIDEO)) return 1;
    if (!TTF_Init())               return 1;

    // Use malloc so this persists for the thread
    RenderContext* ctx = malloc(sizeof(RenderContext));
    if (!ctx) return 1;

    // Set window parameters to RenderContext ptr
    SDL_SetAtomicInt(&ctx->w, plot_width);
    SDL_SetAtomicInt(&ctx->h, plot_height);
    ctx->title = plot_name;
    ctx->gt = gt;

    // Match the prototype in cpfunc.h
    CP_InitFunc(ctx);

    if (!SDL_CreateWindowAndRenderer(plot_name, SDL_GetAtomicInt(&ctx->w), SDL_GetAtomicInt(&ctx->h), SDL_WINDOW_BORDERLESS, &ctx->window, &ctx->renderer)) {
        free(ctx); // Free the RenderContext in case of a crash
        return 1;
    }


    SDL_SetAtomicInt(&ctx->running, 1);
    SDL_Thread* renderThread = SDL_CreateThread(RenderingThread, "RenderThread", ctx);

    // User input loop running in main thread
    SDL_Event event;
    while (SDL_GetAtomicInt(&ctx->running)) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) SDL_SetAtomicInt(&ctx->running, 0);
            if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.key == SDLK_ESCAPE) SDL_SetAtomicInt(&ctx->running, 0);
            }
        }
    }

    
    // Fully close and quit threads, free RenderContext, quit and SDL3
    SDL_WaitThread(renderThread, NULL);
    SDL_DestroyRenderer(ctx->renderer);
    SDL_DestroyWindow(ctx->window);
    free(ctx);
    SDL_Quit();
    return 0;
}