#include<stdio.h>

#define GET_CNTFRQ(cntfrq) asm volatile ("" : : : "memory");\
                           asm volatile("mrs %0, cntfrq_el0" : "=r" (cntfrq));

#define GETTIME(count) asm volatile ("" : : : "memory");\
                       asm volatile("mrs %0, cntpct_el0" : "=r" (count));
                       
int main()
{
        long frq = 0;
        GET_CNTFRQ(frq)
        printf("Timer frequency is %ld[Hz]\n", frq);

        for(int i=0; i < 10; i++){
                long old = 0;
                long elpsd = 0;
                GETTIME(old)

                // wait 1 second
                do{
                        GETTIME(elpsd)
                        elpsd = elpsd - old;
                }while(elpsd < frq);

                printf("1s tick.\n");
        }

        return 0;
}