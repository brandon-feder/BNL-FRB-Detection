/*
    FRB detector test code.

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

#ifndef STDIO_INCLUDE
    #define STDIO_INCLUDE
    #include <stdio.h>
#endif

#ifndef TIME_INCLUDE
    #define TIME_INCLUDE
    #include <time.h>
#endif

#ifndef ASSERT_INCLUDE
    #define ASSERT_INCLUDE
    #include <assert.h>
#endif

#ifndef IMAGE_INCLUDE
    #define IMAGE_INCLUDE
    #include "image.cpp"
#endif

#ifndef DETECTFRB_INCLUDE
    #define DETECTFRB_INCLUDE
    #include "detect-frb.cpp"
#endif

bool isOnSegment( uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2);
void brandonNthSample( uint32_t *sample, uint32_t n, uint32_t nf, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2);

#define SAMPLE_FUNCTION brandonNthSample // The test function
#define MAX_SAMPLE_WEIGHT 256 // The max weight of any given sample
#define FRB_FACTOR 3 // The factor to multiply the FRB by so that it stands out ( > 1 )
#define TEST_SIZE 1000 // The number of time samples to run the test through ( > NTS )
#define NTS 256 // Number of time samples in buffer
#define NFS 256 // Number of frequency samples per time sample
#define N_ANGLES 100 // The number of angles used when detecting the FRB
#define OUTPUT_DATA_IMAGE true // Whether to output the data as an image
#define DATA_IMAGE_PATH "./test-data.pgm" // Where to output the data image

int main(int argc, char *argv[])
{
    printf("Info: Program started\n");

    // Some usefull checks
    assert(FRB_FACTOR > 1);
    assert( TEST_SIZE > NTS );

    // Init random number generator
    printf("Info: Generating 'random' seed\n");
    srand(time(NULL));

    // Initialize the detector
    printf("Info: Creating instance of the detector\n");
    DETECTFRB detector = initDetector( NTS, NFS, N_ANGLES );

    // Generate a line segment
    printf("Info: Getting FRB line segment\n");
    uint32_t x1, y1, x2, y2;
    float theta = M_PI * (float)(rand()%10000)/10000;
    uint32_t offset = rand()%(TEST_SIZE - NTS);

    float xl = 0.3*(float)NFS * cos(theta);
    float yl = 0.3*(float)NFS * sin(theta);
    
    x1 = offset + (uint32_t)(NTS/2 + -xl); y1 = (uint32_t)(NFS/2 + -yl);
    x2 = offset + (uint32_t)(NTS/2 + xl); y2 = (uint32_t)(NFS/2 + yl);

    printf("Info: FRB at %u at an angle of %f \n", offset+NTS/2, theta);
    
    // If true, outputs data in pgm format at DATA_IMAGE_PATH
    if(OUTPUT_DATA_IMAGE)
    {
        printf("Info: Drawing data image\n");
        uint32_t *test_data = (uint32_t *)malloc(sizeof(uint32_t)*TEST_SIZE*NFS);
        for( uint32_t x = 0; x < TEST_SIZE; ++x )
            SAMPLE_FUNCTION( test_data+x*NFS, x, NFS, x1, y1, x2, y2 );

        saveP2Image( DATA_IMAGE_PATH, TEST_SIZE, NFS, test_data, MAX_SAMPLE_WEIGHT );

        free(test_data);
    }

    // Test the detector
    float angle;
    for( uint32_t x = 0; x < TEST_SIZE; ++x )
    {
        uint32_t *next = nextData( &detector );
        SAMPLE_FUNCTION( next, x, NFS, x1, y1, x2, y2 );
        bool isFRB = detectFRB( &detector, &angle );

        // printf("%p\n", next);
        if( isFRB ) printf( "FRB at %f!\n", angle );
    }

    freeDetector( &detector );
}

// Test if (x0, y0) is ~on [(x1, y1), (x2, y2)]
bool isOnSegment(    
    uint32_t x0, uint32_t y0,
    uint32_t x1, uint32_t y1,
    uint32_t x2, uint32_t y2
)
{
    // Get distance to the line
    float N = pow(((float)x2 - (float)x1)*((float)y1 - (float)y0) - ((float)x1 - (float)x0)*((float)y2 - (float)y1), 2);
    float D = (float)( pow((float)x2 - (float)x1, 2 ) + pow( (float)y2 - (float)y1, 2 ) ); 
    float dist = N/D;

    // Test whether the point is on the line-segment ( heuristic )
    return dist <= 1 && (
        (x0 >= x1 && x0 <= x2) || 
        (x0 >= x2 && x0 <= x1)
    );
}

// Get the nth time sample and write it to sample
void brandonNthSample( 
    uint32_t *sample, 
    uint32_t n, 
    uint32_t nf,
    uint32_t x1, uint32_t y1, 
    uint32_t x2, uint32_t y2
)
{
    // For each frequency sample in the new time sample
    for( uint32_t i = 0; i < nf; ++i )
    {
        // Set the sample to some random value in [0, MAX_SAMPLE_WEIGHT/FRB_FACTOR)
        sample[i] =  ((uint32_t)rand()%MAX_SAMPLE_WEIGHT)/FRB_FACTOR;

        // If the point is on the FRB, linearly map it to [0, MAX_SAMPLE_WEIGHT)
        if( isOnSegment(n, i, x1, y1, x2, y2 ) ) sample[i] *= FRB_FACTOR;
    }
}