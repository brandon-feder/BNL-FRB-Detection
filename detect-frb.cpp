/*
    Handles FRB detection

    Author: Brandon E. Feder <brandon.e.feder@gmail.com>
*/

#ifndef STDINT_INCLUDE
    #define STDINT_INCLUDE
    #include <stdint.h>
#endif

#ifndef MATH_INCLUDE
    #define MATH_INCLUDE
    #include <math.h>
#endif

#ifndef STDEXCEPT_INCLUDE
    #define STDEXCEPT_INCLUDE
    #include <stdexcept>
#endif

#ifndef DETECTFRB_H_INCLUDE
    #define DETECTFRB_H_INCLUDE
    #include "detect-frb.h"
#endif

// Writes all pixels on the segment (x1, y1) to (x2, y2) to the X and Y arrays.
// X and Y should have length at least `max( |x2 - x1|, |y2 - y1| )-1`
void pixelsOnSegment( 
    int32_t *X, int32_t *Y,
    int32_t x1, int32_t y1, 
    int32_t x2, int32_t y2
)
{
    float dx, dy, x, y;
    uint32_t r, l;

    dx = x2 - x1; dy = y2 - y1;
    r = ( abs(dx) > abs(dy) ) ? abs(dx) : abs(dy);
    dx /= r; dy /= r;
    x = x1; y = y1;

    for( l = 0; l < r; ++l )
    {
        X[l] = round(x); Y[l] = round(y);
        x += dx; y += dy;
    }
}

// Initialize the detection structure.
DETECTFRB initDetector( uint32_t nt_, uint32_t nf_, uint32_t nAngles_ )
{
    // Declare variables
    float dt = M_PI/nAngles_;
    float *angles_ = ( float * ) malloc( nAngles_ * sizeof(float) );
    uint32_t *lengths_ = ( uint32_t * ) malloc( nAngles_ * sizeof(uint32_t) );
    int32_t **xIntersects_ = ( int32_t ** ) malloc( nAngles_ * sizeof(int32_t *) );
    int32_t **yIntersects_ = ( int32_t ** ) malloc( nAngles_ * sizeof(int32_t *) );
    
    float theta = 0;
    for( uint32_t i = 0; i < nAngles_; ++i ) // For every detection angle
    {
        // Calculate start and end of segment on data that line corresponds to
        float x = cos(theta);
        float y = sin(theta);

        if( abs( (x * nf_)/y ) > nt_ )
        {
            y *= nt_/x; x = nt_;
        } else
        {
            x *= nf_/y; y = nf_;
        }

        x /= 2; y /= 2;

        int32_t x1, y1, x2, y2;
        x1 = -x; y1 = -y;
        x2 = x; y2 = y;

        // Get the length of the arrays
        uint32_t L = std::max(abs(x2 - x1), abs(y2 - y1))-1;
        
        // Create arrays
        xIntersects_[i] = ( int32_t * ) malloc( L * sizeof( int32_t ) );
        yIntersects_[i] = ( int32_t * ) malloc( L * sizeof( int32_t ) );
        angles_[i] = theta;
        lengths_[i] = L;

        // Calculate all the pixel intersections
        pixelsOnSegment(xIntersects_[i], yIntersects_[i],  x1, y1, x2, y2 );

        // Get next angle
        theta += dt;
    }

    // Create structure
    DETECTFRB detector;
    detector.nt = nt_;
    detector.nf = nf_;
    detector.dtheta = dt;
    detector.nAngles = nAngles_;
    detector.angles = angles_;
    detector.lengths = lengths_;
    detector.xIntersections = xIntersects_;
    detector.yIntersections = yIntersects_;
    detector.data = ( uint32_t * ) malloc( sizeof(uint32_t) * nt_ * nf_ );
    detector.prevWeights = ( float * ) malloc( sizeof(float) * nt_ * nAngles_ );

    return detector;
}

uint32_t *nextData( DETECTFRB *detector )
{
    return detector->data + detector->nf * detector->p;
}

bool detectFRB( DETECTFRB *detector, float *angle, uint32_t sigma, uint32_t count )
{
    bool detected = false;
    
    for( uint32_t i = 0; i < detector->nAngles; ++i ) // For each angle
    {
        // Define some variables to make things easier
        int32_t *X = detector->xIntersections[i];
        int32_t *Y = detector->yIntersections[i];
        uint32_t L = detector->lengths[i];
        
        // Calculate the average sample value along current line
        float S = 0;
        for( uint32_t j = 0; j < L; ++j )
        {
            int32_t x = X[j];
            int32_t y = Y[j];

            S += detector->data[ 
                (uint32_t)( y + detector->nf/2 ) +
                (( (uint32_t)( x + detector->nt/2 ) + detector->p )%detector->nt ) * detector->nf 
            ];
        }
        S /= L;
        // printf("A\n");

        // Calculate the mean
        float mean = 0;
        for( uint32_t j = 0; j < detector->nt; ++j ) mean += detector->prevWeights[ j*detector->nAngles + i ];
        mean /= detector->nt;

        // printf("B\n");
        // Calculate the variance 
        float variance = 0;
        for( uint32_t j = 0; j < detector->nt; ++j ) variance += pow( detector->prevWeights[ j*detector->nAngles + i ] - mean , 2);
        variance /= detector->nt;

        // Update previous weights
        detector->prevWeights[ detector->p*detector->nAngles + i ] = S;

        // Was FRB detected
        if( S >= mean + sigma * sqrt( variance ) )
        {
            *angle = detector->angles[i];
            detected = true;
        }
        // printf("C\n");
    }

    detector->p += 1;
    detector->p %= detector->nt;

    return detected;
}

// Free the heap allocated memmory used by the detector structure
void freeDetector( DETECTFRB *detector )
{
    free( detector->angles );
    free( detector->lengths );

    for( uint32_t i = 0; i < detector->nAngles; ++i )
    {
        free( detector->xIntersections[i] ); 
        free( detector->yIntersections[i] ); 
    }   

    free( detector->xIntersections );
    free( detector->yIntersections );
    free( detector->data );
    free( detector->prevWeights );
}