#ifndef CPLOT_H
#define CPLOT_H

#include "cpfunc.h"

// Move these inside the guard!
void CP2D_RenderHorizontalAxis(RenderContext* ctx, int x_length);
int CP_InitSDL(char* plot_name, int plot_width, int plot_height, GraphType gt);

#endif