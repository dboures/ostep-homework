#include <stdio.h>
#include <sys/time.h>

#include <assert.h>
#include <CoreServices/CoreServices.h>
#include <mach/mach.h>
#include <mach/mach_time.h>
#include <unistd.h>



struct timeval tval;

void trial()
{
    int i = 0;
    while (i < 100) {
        gettimeofday(&tval, NULL);
        i += 1;
    };
}

//https://developer.apple.com/library/archive/qa/qa1398/_index.html
uint64_t GetTimeInNanoseconds(void)
{
    uint64_t        start;
    uint64_t        end;
    uint64_t        elapsed;
    uint64_t        elapsedNano;
    static mach_timebase_info_data_t    sTimebaseInfo;

    // Start the clock.

    start = mach_absolute_time();

    trial();

    // Stop the clock.

    end = mach_absolute_time();

    // Calculate the duration.

    elapsed = end - start;

    // Convert to nanoseconds.

    // If this is the first time we've run, get the timebase.
    // We can use denom == 0 to indicate that sTimebaseInfo is 
    // uninitialised because it makes no sense to have a zero 
    // denominator is a fraction.

    if ( sTimebaseInfo.denom == 0 ) {
        (void) mach_timebase_info(&sTimebaseInfo);
    }

    // Do the maths. We hope that the multiplication doesn't 
    // overflow; the price you pay for working in fixed point.

    elapsedNano = elapsed * sTimebaseInfo.numer / sTimebaseInfo.denom;

    return elapsedNano;
}

// now we need to measure the cost of a context switch

// what would for sure cause a context switch??

void piped()
{
    int pipeA[2];
    int pipeB[2];
    char ba;

    uint64_t        startP;
    uint64_t        endP;

    uint64_t        startC;
    uint64_t        endC;

    uint64_t        elapsedC;
    uint64_t        elapsedNanoC;
    static mach_timebase_info_data_t    sTimebaseInfoC;

    uint64_t        elapsedP;
    uint64_t        elapsedNanoP;
    static mach_timebase_info_data_t    sTimebaseInfoP;

    pipe(pipeA);
    pipe(pipeB);

    printf("\n\n");
    int rc = fork();
    if (rc < 0) {
        // fork failed
        fprintf(stderr, "fork failed\n");
    } else if (rc == 0) {
        close(pipeA[0]);
        int i = 0;
        while (i < 10) {
            startP = mach_absolute_time();

            write(pipeA[1], "L", strlen("L"));
            read(pipeB[0], &ba, 1);
            
            endP = mach_absolute_time();
            
            elapsedP = endP - startP;
            if (sTimebaseInfoP.denom == 0 ) {
                (void) mach_timebase_info(&sTimebaseInfoP);
            }
            if (i == 0) {
                elapsedNanoP = (elapsedP * sTimebaseInfoP.numer / sTimebaseInfoP.denom);
            } else {
                elapsedNanoP = elapsedNanoP + (elapsedP * sTimebaseInfoP.numer / sTimebaseInfoP.denom);
            }
            
            // printf("parent run : %llu\n\n", elapsedNanoP + (elapsedP * sTimebaseInfoP.numer / sTimebaseInfoP.denom));
            i += 1;
        };
        printf("parent: %llu\n", elapsedNanoP / 10);
        
        close(pipeA[1]);
        wait(NULL);
        
    }
    else {
        close(pipeB[0]);
        int j = 0;
        while (j < 10) {
            startC = mach_absolute_time();

            read(pipeA[0], &ba, 1);
            write(pipeB[1], &ba, 1);

            endC = mach_absolute_time();
            
            elapsedC = endC - startC;
            if (sTimebaseInfoC.denom == 0 ) {
                (void) mach_timebase_info(&sTimebaseInfoC);
            }
            if (j == 0) {
                elapsedNanoC = (elapsedC * sTimebaseInfoC.numer / sTimebaseInfoC.denom);
            } else {
                elapsedNanoC = elapsedNanoC + (elapsedC * sTimebaseInfoC.numer / sTimebaseInfoC.denom);
            }
            // printf("child run : %llu\n\n", elapsedNanoC + (elapsedC * sTimebaseInfoC.numer / sTimebaseInfoC.denom));
            j += 1;
        };
        printf("child: %llu\n", elapsedNanoC / 10);

        close(pipeB[1]);

    }
}




int main()
{
    // uint64_t x = GetTimeInNanoseconds();
    // printf("%llu\n", x);

    piped();
    return 0;
}