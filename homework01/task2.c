#include <stdio.h>
#define N 10

void main()
{
    int mas[N];
    
    for (int i = 0; i < N; i++)
    {
        mas[i] = i+1;
    }
    
    for (int i = 0; i < N; i++)
    {
        printf("%d ", mas[i]);
    }
    
    printf("\n");
    
    int temp;
    
    for (int i = 0; i < N/2; i++)
    {
        temp = mas[i];
        mas[i] = mas[N-1-i];
        mas[N-1-i] = temp;
    }
    
    for (int i = 0; i < N; i++)
    {
        printf("%d ", mas[i]);
    }
    
    printf("\n");

}
