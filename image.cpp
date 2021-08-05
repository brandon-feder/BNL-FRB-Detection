/*
    PGM image output

    Author: Brandon E. Feder <brandon.e.feder@gmail.com>
*/

#ifndef STDIO_INCLUDE
    #define STDIO_INCLUDE
    #include <stdio.h>
#endif

#ifndef STDINT_INCLUDE
    #define STDINT_INCLUDE
    #include <stdint.h>
#endif

// Output a grayscale image using the pgm format. Coloumn-major order
void saveP2Image( 
    const char *fp, 
    uint32_t w, uint32_t h, 
    uint32_t *image, 
    uint32_t M, uint32_t p )
{
    // Open the image for writing
    FILE *f = fopen(fp, "w");

    // Write header
    fprintf(f, "P2 %u %u %u ", w, h, M );

    // Write grayscale values
    for( uint32_t x = 0; x < h; ++x )
        for( uint32_t y = 0; y < w; ++y )
            fprintf(f, "%u ", image[x + ( (y+p)%w )*h] );
}
