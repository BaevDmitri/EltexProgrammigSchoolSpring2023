#include <stdio.h>
#include <pthread.h>
#define N 5
#define M 200000000

int sum;

void *countFunc(void* numB)
{
    int *number = (int *) numB;
    for (int i = 0; i < M; i++)
    {
        sum++;
    }
    
    printf("%d\n", *number);

    return NULL;
}

int main()
{
    int i;
    int a[N];
    int *s;
    sum = 0;

    pthread_t thread[N];

    for (i = 0; i < N; i++)
    {
        a[i] = i;
        pthread_create(&thread[i], NULL, countFunc, (void*) &a[i]);
    }

    for (i = 0; i < N; i++)
    {
        pthread_join(thread[i], (void **) &s);
    }

    printf("%d\n", sum);
    
    return 0;
}
