#include <stdio.h>
#define __USE_GNU
#include <math.h>
#include <stdint.h>
               
#define PI_STR "3.14159265358979323846264338327950288419716939937510582097494459230781640628"
// Compile with gcc -O3 search-for-pi.c
// Routine to peek inside float
void dump_float(float f)
{
    unsigned int sign, fraction, exp;

    unsigned int* f_as_i;

    f_as_i = (unsigned int *) &f;
    
    fraction = ((*f_as_i) & 0x007FFFFFU);
    exp      = ((*f_as_i) & 0x7F800000U) >> 23;
    sign     = ((*f_as_i) & 0x80000000U) >> 31;

    printf("(float, float_in_hex, sign, exp, fraction) = "
           "(%.10f, %.8X, %d, %d, %d)\n", f, *f_as_i, sign, exp, fraction);
    printf("Evaluate: (-1)^(%u)*2^(%u-127)*(1 + %d/%d)\n", sign, exp, fraction,
        (1U << 23));
}

// for given b, finds best value of a and best value of pi
// Returns best_a and stores best value of pi in bestPi address.
uint64_t bisectSearch(uint64_t b, long double *bestPi)
{
    uint64_t aLeft = 3*b;
    uint64_t aRight = 4*b;
    uint64_t a = (aLeft + aRight) / 2;
    uint64_t best_a = 0;
    int count = 0;
    while (a != aLeft) {
        // printf("Iteration: %d\n", count++); 
        long double current_pi = ((long double)(a)) / b;
        if (best_a == 0) {
            best_a = a;
            *bestPi = current_pi;
        } else {
            if(fabsl(M_PIl - current_pi) < fabsl(M_PIl - *bestPi)) {
                best_a = a;
                *bestPi = current_pi;
            }
        }
        if ((M_PIl - current_pi) > 0) {
            // current_pi < pi, a should be higher.
            aLeft = a;
        } else if ((M_PIl - current_pi) < 0) {
            // current_pi > pi, a should be lower.
            aRight = a;
        } 
        else {
            best_a = a;
            *bestPi = current_pi;
            return best_a;
        }
        a = (aLeft + aRight) / 2;
    }
    return best_a;
}

#define USE_BISECTION
int main(int argc, char const *argv[])
{
    // float pi = 3.141592653589793238462643383279502884197;
    // dump_float(pi);
    long double best_ratio = 3.0;
    uint64_t best_a = 3;
    uint64_t best_b = 1;

#ifndef USE_BISECTION
    for(uint64_t b = 1; b < 100000; b++)
    {
        uint64_t low = 3*b;
        uint64_t high = 4*b;

        for(uint64_t a = low; a < high; a++)
        {
            long double current_ratio = ((long double)(a)) / b;
            if(fabsl(M_PIl - current_ratio) < fabsl(M_PIl - best_ratio))
            {
                best_a = a;
                best_b = b;
                best_ratio = current_ratio;
                printf("Found better approximation %ld / %ld, E = %Le\n", a, b,
                    fabsl(M_PIl - current_ratio));
            }
        }
    }
#else
    for(uint64_t b = 1; b < 100000; b++)
    {
        long double current_ratio = 0.0;
        uint64_t a = bisectSearch(b, &current_ratio);
        if(fabsl(M_PIl - current_ratio) < fabsl(M_PIl - best_ratio))
        {
            best_a = a;
            best_b = b;
            best_ratio = current_ratio;
            printf("Found better approximation %ld / %ld, E = %Le\n", a, b,
                fabsl(M_PIl - current_ratio));
        }
        // break;
    }
#endif
    return 0;
}
