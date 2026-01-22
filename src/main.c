#include <stdio.h>
#include "cplot.h"
#include "cpfunc.h"

int main(int argc, char* argv[]) {
    float x[] = {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9};
    
    // Calculate count here: (Total Bytes / Bytes per element)
    size_t count = sizeof(x) / sizeof(x[0]); 

    CP_InitSDL("Plot", 1280, 720, CP_2D);
    CP_SetLinearFunction(NULL, x, count, 5.0f, 0.0f);

    return 0;
}