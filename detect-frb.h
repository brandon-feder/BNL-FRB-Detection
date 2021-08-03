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

    // Detection angle change and number
    float dtheta;
    uint32_t nAngles;

    // Whether to start the detection ( whether the data buffer is full )
    bool detect = false;

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
};

uint32_t pixelsOnSegment( uint32_t *x, uint32_t *y, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2 );
DETECTFRB initDetector( uint32_t nt_, uint32_t nf_, uint32_t dtheta_ );
uint32_t *nextData( DETECTFRB *detector );
bool detectFRB( DETECTFRB *detector, float *angle );
void freeDetector( DETECTFRB *detector );