#ifndef CPLOT_H
#define CPLOT_H

typedef enum {
    CP_2D,
    CP_3D,
    CP_HEATMAP_2D
} GraphType;

#endif


int CP_InitSDL(char* plot_name, int plot_width, int plot_height, GraphType gt);