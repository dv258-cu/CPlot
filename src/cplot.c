#define SDL_MAIN_HANDLED
#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>

#define MARGIN 100

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

    TTF_Font* font;

    GraphType gt;
} RenderContext;

int Graph2Window_X(int x) {
    return x + MARGIN;
}

int Graph2Window_Y(int y) {
    return -y + 720 - MARGIN;
}

// All plotting is done here
int RenderingThread(void* data) {
    RenderContext* ctx = (RenderContext*)data;
    Uint64 now = SDL_GetPerformanceCounter();
    Uint64 last = 0;

    // VSync in separate thread to prevent 100% CPU usage
    SDL_SetRenderVSync(ctx->renderer, 1);

    while (SDL_GetAtomicInt(&ctx->running)) {
        last = now;
        now = SDL_GetPerformanceCounter();
        double deltaTime = (double)((now - last) / (double)SDL_GetPerformanceFrequency());

        SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 0, 255);
        SDL_RenderClear(ctx->renderer);

        /// Start Plot Code ///

        switch(ctx->gt) {
            case CP_2D: {
                // Draw the axes of a plot
                SDL_SetRenderDrawColor(ctx->renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderLine(ctx->renderer, MARGIN, SDL_GetAtomicInt(&ctx->h) - MARGIN, SDL_GetAtomicInt(&ctx->w) - MARGIN, SDL_GetAtomicInt(&ctx->h) - MARGIN);
                SDL_RenderLine(ctx->renderer, MARGIN, SDL_GetAtomicInt(&ctx->h) - MARGIN, MARGIN, MARGIN);

                SDL_SetRenderDrawColor(ctx->renderer, 0x03, 0x78, 0x8A, 0xFF);
                SDL_RenderLine(ctx->renderer, Graph2Window_X(0), Graph2Window_Y(0), Graph2Window_X(10), Graph2Window_Y(75));

                break;
            }
            case CP_3D: {
                break;
            }
            case CP_HEATMAP_2D: {
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

    RenderContext ctx;
    SDL_SetAtomicInt(&ctx.w, plot_width);
    SDL_SetAtomicInt(&ctx.h, plot_height);

    //
    ctx.gt = gt;

    if (!SDL_CreateWindowAndRenderer(plot_name, SDL_GetAtomicInt(&ctx.w), SDL_GetAtomicInt(&ctx.h), SDL_WINDOW_BORDERLESS, &ctx.window, &ctx.renderer)) {
        return 1;
    }


    SDL_SetAtomicInt(&ctx.running, 1);
    SDL_Thread* renderThread = SDL_CreateThread(RenderingThread, "RenderThread", &ctx);

    SDL_Event event;
    while (SDL_GetAtomicInt(&ctx.running)) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) SDL_SetAtomicInt(&ctx.running, 0);
            if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.key == SDLK_ESCAPE) SDL_SetAtomicInt(&ctx.running, 0);
            }
        }
    }

    
    // Fully close and quit threads, renderer, window, and SDL3
    SDL_WaitThread(renderThread, NULL);
    SDL_DestroyRenderer(ctx.renderer);
    SDL_DestroyWindow(ctx.window);
    SDL_Quit();
    return 0;
}