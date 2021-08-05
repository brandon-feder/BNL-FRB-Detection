/*
    Handles FRB detection (Header)

    Author: Brandon E. Feder <brandon.e.feder@gmail.com>
*/

#ifndef STDINT_INCLUDE
    #define STDINT_INCLUDE
    #include <stdint.h>
#endif

// Structure contain the FRB detection code
struct DETECTFRB
{
    // # of frequency/time channels
    uint32_t nt;
    uint32_t nf;

    // Detection angle chandge and number
    float dtheta;
    uint32_t nAngles;

    // Position of the next data
    uint32_t p = 0;

    // Array of the angles
    float *angles;

    // Array of the length of each array of pixel intersections
    uint32_t *lengths;

    // The x and y pixel intersections
    int32_t **xIntersections;
    int32_t **yIntersections;

    // Data buffer
    uint32_t *data;

    // The angle saves
    float  *prevWeights;
};

uint32_t pixelsOnSegment( uint32_t *x, uint32_t *y, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2 );
DETECTFRB initDetector( uint32_t nt_, uint32_t nf_, uint32_t dtheta_, uint32_t n_ );
uint32_t *nextData( DETECTFRB *detector );
bool detectFRB( DETECTFRB *detector, float *angle, uint32_t sigma, uint32_t count );
void freeDetector( DETECTFRB *detector );